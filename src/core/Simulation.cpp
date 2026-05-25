#include "Simulation.h"
#include <cmath>
#include <cstdlib>
#include <algorithm>

// Base stats per channel: {reach_per_$, base_ctr, base_conv}
static const float BASE_REACH_PER_DOLLAR[] = { 50.f, 20.f, 80.f, 30.f, 15.f, 40.f };
static const float BASE_CTR[]              = { 0.025f, 0.04f, 0.02f, 0.05f, 0.01f, 0.035f };
static const float BASE_CONV[]             = { 0.015f, 0.03f, 0.02f, 0.025f, 0.01f, 0.03f };

float Simulation::GetChannelMod(ChannelType ch, const GameState& gs) {
    auto it = gs.channelModifiers.find(ch);
    return (it != gs.channelModifiers.end()) ? it->second : 1.0f;
}

float Simulation::CalcReach(const Campaign& c, const GameState& gs) {
    int idx = (int)c.channel;
    float staffBonus = 1.0f;
    for (auto& s : gs.staff)
        if ((int)s.role == idx) staffBonus += s.skill * 0.2f;
    return c.budget * BASE_REACH_PER_DOLLAR[idx] * GetChannelMod(c.channel, gs) * staffBonus;
}

float Simulation::CalcCTR(const Campaign& c, const GameState& gs) {
    int idx = (int)c.channel;
    float base = BASE_CTR[idx] * GetChannelMod(c.channel, gs);
    float qualMod = 0.5f + (c.qualityScore / 10.f) * 0.5f;
    return std::min(base * qualMod, 0.15f);
}

float Simulation::CalcConversionRate(const Campaign& c, const GameState& gs) {
    int idx = (int)c.channel;
    return BASE_CONV[idx] * GetChannelMod(c.channel, gs);
}

float Simulation::CalcRevenue(const Campaign& c, const GameState& gs) {
    float r = CalcReach(c, gs);
    float ctr = CalcCTR(c, gs);
    float conv = CalcConversionRate(c, gs);
    // Find client average order value
    float aov = 50.f;
    for (auto& cl : gs.clients)
        if (cl.id == c.clientId) { aov = cl.budget * 0.1f; break; }
    return r * ctr * conv * aov;
}

void Simulation::ProcessCampaign(Campaign& c, GameState& gs) {
    if (!c.active || c.completed) return;
    c.reach          = CalcReach(c, gs);
    c.ctr            = CalcCTR(c, gs);
    c.conversionRate = CalcConversionRate(c, gs);
    c.revenue        = CalcRevenue(c, gs);
    c.agencyFee      = c.revenue * 0.15f;
    c.monthsLeft--;
    if (c.monthsLeft <= 0) {
        c.active    = false;
        c.completed = true;
        gs.stats.campaignsCompleted++;
    }
}

void Simulation::UpdateClientSatisfaction(Client& cl, const GameState& gs) {
    float totalRevenue = 0.f;
    int   activeCamps  = 0;
    for (auto& c : gs.campaigns)
        if (c.clientId == cl.id && c.active) {
            totalRevenue += c.revenue;
            activeCamps++;
        }
    float expected = cl.budget * 0.5f;
    float delta = (totalRevenue - expected) / expected * 20.f;
    cl.satisfaction = std::clamp(cl.satisfaction + delta, 0.f, 100.f);
    if (activeCamps == 0) cl.satisfaction -= 5.f;
    cl.satisfaction = std::max(cl.satisfaction, 0.f);
    cl.contractMonths--;
    if (cl.contractMonths <= 0 && cl.satisfaction < 50.f) {
        cl.active = false;
        gs.stats.clientsLost++;
    }
}

void Simulation::ProcessAICompetitors(GameState& gs) {
    float totalShare = gs.playerMarketShare;
    for (auto& ai : gs.competitors) totalShare += ai.marketShare;
    float remaining = 100.f - totalShare;
    if (remaining < 0) remaining = 0;

    for (auto& ai : gs.competitors) {
        float growth = (float)(rand() % 10 - 3) * 0.1f * ai.aggressiveness;
        ai.marketShare = std::max(1.f, ai.marketShare + growth);
    }
    // Player market share grows with clients and reputation
    float repGrowth = gs.stats.reputation * 0.01f;
    float clientGrowth = (float)gs.stats.clientsAcquired * 0.05f;
    gs.playerMarketShare = std::clamp(
        gs.playerMarketShare + repGrowth + clientGrowth * 0.1f, 0.f, 100.f);
}

void Simulation::AdvanceMonth(GameState& gs) {
    gs.month++;
    if (gs.month > 12) { gs.month = 1; gs.year++; }
    gs.stats.monthsPlayed++;

    // Process active events
    for (auto& ev : gs.activeEvents) {
        ev.monthsLeft--;
        if (ev.monthsLeft <= 0) ev.active = false;
    }
    gs.activeEvents.erase(
        std::remove_if(gs.activeEvents.begin(), gs.activeEvents.end(),
            [](const NewsEvent& e) { return !e.active; }),
        gs.activeEvents.end());

    // Rebuild channel modifiers
    gs.channelModifiers.clear();
    for (int i = 0; i < 6; i++)
        gs.channelModifiers[(ChannelType)i] = 1.0f;
    for (auto& ev : gs.activeEvents) {
        gs.channelModifiers[ChannelType::Social]     *= ev.socialMod;
        gs.channelModifiers[ChannelType::SEO]        *= ev.seoMod;
        gs.channelModifiers[ChannelType::Email]      *= ev.emailMod;
        gs.channelModifiers[ChannelType::Influencer] *= ev.influencerMod;
        gs.channelModifiers[ChannelType::PR]         *= ev.prMod;
        gs.channelModifiers[ChannelType::PaidSearch] *= ev.paidMod;
        gs.budget += ev.budgetImpact;
    }

    // Process campaigns
    gs.monthlyRevenue = 0.f;
    for (auto& c : gs.campaigns)
        if (c.active) {
            ProcessCampaign(c, gs);
            gs.monthlyRevenue += c.agencyFee;
            gs.stats.totalRevenue += c.agencyFee;
        }

    // Monthly staff salaries
    gs.monthlyExpenses = 0.f;
    for (auto& s : gs.staff) {
        gs.monthlyExpenses += s.salary;
        s.monthsHired++;
    }
    gs.budget += gs.monthlyRevenue - gs.monthlyExpenses;

    // Update clients
    for (auto& cl : gs.clients)
        if (cl.active) UpdateClientSatisfaction(cl, gs);

    // AI
    ProcessAICompetitors(gs);

    // Update best month
    if (gs.monthlyRevenue > gs.stats.bestMonthRevenue)
        gs.stats.bestMonthRevenue = gs.monthlyRevenue;

    // Reputation
    float repDelta = (gs.monthlyRevenue > 0 ? 1.f : -1.f) * gs.stats.monthsPlayed * 0.1f;
    gs.stats.reputation = std::clamp(gs.stats.reputation + repDelta, 0.f, 100.f);

    // Win/lose conditions
    if (gs.budget < -50000.f) gs.gameOver = true;
    if (gs.playerMarketShare >= 35.f) gs.victory = true;
}
