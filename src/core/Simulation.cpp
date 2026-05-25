#include "Simulation.h"
#include <cmath>
#include <cstdlib>
#include <algorithm>

// Rebalanced v0.3: reach, ctr, conv tuned for 18% agency fee
static const float BASE_REACH_PER_DOLLAR[] = { 45.f, 18.f, 70.f, 28.f, 12.f, 38.f };
static const float BASE_CTR[]              = { 0.028f, 0.045f, 0.022f, 0.055f, 0.012f, 0.038f };
static const float BASE_CONV[]             = { 0.018f, 0.032f, 0.022f, 0.028f, 0.012f, 0.032f };
static const float AGENCY_FEE_RATE        = 0.18f;  // 18% (was 15%)

float Simulation::GetChannelMod(ChannelType ch, const GameState& gs) {
    auto it = gs.channelModifiers.find(ch);
    return (it != gs.channelModifiers.end()) ? it->second : 1.0f;
}

float Simulation::CalcReach(const Campaign& c, const GameState& gs) {
    int idx = (int)c.channel;
    float staffBonus = 1.0f;
    for (auto& s : gs.staff)
        if ((int)s.role == idx) staffBonus += s.skill * 0.25f;
    return c.budget * BASE_REACH_PER_DOLLAR[idx] * GetChannelMod(c.channel, gs) * staffBonus;
}

float Simulation::CalcCTR(const Campaign& c, const GameState& gs) {
    int idx = (int)c.channel;
    float base = BASE_CTR[idx] * GetChannelMod(c.channel, gs);
    float qualMod = 0.5f + (c.qualityScore / 10.f) * 0.5f;
    return std::min(base * qualMod, 0.18f);
}

float Simulation::CalcConversionRate(const Campaign& c, const GameState& gs) {
    int idx = (int)c.channel;
    return BASE_CONV[idx] * GetChannelMod(c.channel, gs);
}

float Simulation::CalcRevenue(const Campaign& c, const GameState& gs) {
    float r    = CalcReach(c, gs);
    float ctr  = CalcCTR(c, gs);
    float conv = CalcConversionRate(c, gs);
    float aov  = 50.f;
    for (auto& cl : gs.clients)
        if (cl.id == c.clientId) { aov = cl.budget * 0.12f; break; }
    return r * ctr * conv * aov;
}

void Simulation::ProcessCampaign(Campaign& c, GameState& gs) {
    if (!c.active || c.completed) return;
    c.reach          = CalcReach(c, gs);
    c.ctr            = CalcCTR(c, gs);
    c.conversionRate = CalcConversionRate(c, gs);
    c.revenue        = CalcRevenue(c, gs);
    c.agencyFee      = c.revenue * AGENCY_FEE_RATE;
    c.monthsLeft--;
    if (c.monthsLeft <= 0) {
        c.active    = false;
        c.completed = true;
        gs.stats.campaignsCompleted++;
    }
}

void Simulation::UpdateClientSatisfaction(Client& cl, const GameState& gs) {
    float totalRevenue = 0.f;
    int activeCamps = 0;
    for (auto& c : gs.campaigns)
        if (c.clientId == cl.id && c.active) {
            totalRevenue += c.revenue;
            activeCamps++;
        }
    float expected = cl.budget * 0.55f;
    float delta = (totalRevenue - expected) / (expected + 1.f) * 18.f;
    cl.satisfaction = std::clamp(cl.satisfaction + delta - 0.5f, 0.f, 100.f);
    if (activeCamps == 0) cl.satisfaction = std::max(0.f, cl.satisfaction - 5.5f);
    cl.contractMonths--;
    if (cl.contractMonths <= 0 && cl.satisfaction < 50.f) {
        cl.active = false;
        gs.stats.clientsLost++;
    }
}

void Simulation::ProcessAICompetitors(GameState& gs) {
    for (auto& ai : gs.competitors) {
        // AI grows naturally
        float growth = (float)(rand() % 8 - 2) * 0.12f * ai.aggressiveness;
        ai.marketShare = std::max(1.f, ai.marketShare + growth);
        ai.marketShare = std::min(ai.marketShare, 35.f);
    }
    float repGrowth    = gs.stats.reputation * 0.012f;
    float clientGrowth = (float)gs.stats.clientsAcquired * 0.06f;
    gs.playerMarketShare = std::clamp(
        gs.playerMarketShare + repGrowth + clientGrowth * 0.08f, 0.f, 100.f);
}

void Simulation::AdvanceMonth(GameState& gs) {
    gs.month++;
    if (gs.month > 12) { gs.month = 1; gs.year++; }
    gs.stats.monthsPlayed++;

    // Decay and remove expired events
    for (auto& ev : gs.activeEvents) {
        ev.monthsLeft--;
        if (ev.monthsLeft <= 0) ev.active = false;
    }
    gs.activeEvents.erase(
        std::remove_if(gs.activeEvents.begin(), gs.activeEvents.end(),
            [](const NewsEvent& e){ return !e.active; }),
        gs.activeEvents.end());

    // Rebuild channel modifiers from events
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

    // Staff salaries (base 2800 + 9% growth per 3 staff over 3)
    gs.monthlyExpenses = 0.f;
    int staffCount = (int)gs.staff.size();
    float overhead = staffCount > 3 ? (staffCount - 3) * 280.f : 0.f;
    for (auto& s : gs.staff) {
        gs.monthlyExpenses += s.salary;
        s.monthsHired++;
    }
    gs.monthlyExpenses += overhead; // rent + overhead
    if (staffCount == 0) gs.monthlyExpenses += 800.f; // base rent even with no staff
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
    float repDelta = gs.monthlyRevenue > gs.monthlyExpenses ? 1.2f : -1.f;
    gs.stats.reputation = std::clamp(gs.stats.reputation + repDelta, 0.f, 100.f);

    // Soft loan protection: don't instantly bankrupt at 0
    if (gs.budget < 0 && gs.budget > -50000.f) {
        // still alive — just in the red
    }
    if (gs.budget <= -50000.f) gs.gameOver = true;
    if (gs.playerMarketShare >= 35.f) gs.victory = true;
}
