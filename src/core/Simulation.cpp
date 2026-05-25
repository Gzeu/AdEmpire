#include "Simulation.h"
#include <cmath>
#include <cstdlib>
#include <algorithm>

static const float BASE_REACH_PER_DOLLAR[6] = { 50.f, 20.f, 80.f, 30.f, 15.f, 40.f };
static const float BASE_CTR[6]              = { 0.025f, 0.04f, 0.02f, 0.05f, 0.01f, 0.035f };
static const float BASE_CONV[6]             = { 0.015f, 0.03f, 0.02f, 0.025f, 0.01f, 0.03f };

float Simulation::GetChannelMod(ChannelType ch, const GameState& gs) {
    auto it = gs.channelModifiers.find(ch);
    return (it != gs.channelModifiers.end()) ? it->second : 1.0f;
}

float Simulation::CalcReach(const Campaign& c, const GameState& gs) {
    int   idx        = static_cast<int>(c.channel);
    float staffBonus = 1.0f;
    for (const auto& s : gs.staff)
        if (static_cast<int>(s.role) == idx)
            staffBonus += s.skill * 0.2f;
    return c.budget * BASE_REACH_PER_DOLLAR[idx] * GetChannelMod(c.channel, gs) * staffBonus;
}

float Simulation::CalcCTR(const Campaign& c, const GameState& gs) {
    int   idx     = static_cast<int>(c.channel);
    float base    = BASE_CTR[idx] * GetChannelMod(c.channel, gs);
    float qualMod = 0.5f + (c.qualityScore / 10.f) * 0.5f;
    return std::min(base * qualMod, 0.15f);
}

float Simulation::CalcConversionRate(const Campaign& c, const GameState& gs) {
    return BASE_CONV[static_cast<int>(c.channel)] * GetChannelMod(c.channel, gs);
}

float Simulation::CalcRevenue(const Campaign& c, const GameState& gs) {
    float aov = 50.f;
    for (const auto& cl : gs.clients)
        if (cl.id == c.clientId) { aov = cl.budget * 0.1f; break; }
    return CalcReach(c, gs) * CalcCTR(c, gs) * CalcConversionRate(c, gs) * aov;
}

void Simulation::ProcessCampaign(Campaign& c, GameState& gs) {
    if (!c.active || c.completed) return;
    c.reach          = CalcReach(c, gs);
    c.ctr            = CalcCTR(c, gs);
    c.conversionRate = CalcConversionRate(c, gs);
    c.revenue        = CalcRevenue(c, gs);
    c.agencyFee      = c.revenue * 0.15f;
    if (--c.monthsLeft <= 0) {
        c.active    = false;
        c.completed = true;
        gs.stats.campaignsCompleted++;
    }
}

void Simulation::UpdateClientSatisfaction(Client& cl, const GameState& gs) {
    float totalRev    = 0.f;
    int   activeCamps = 0;
    for (const auto& c : gs.campaigns)
        if (c.clientId == cl.id && c.active) { totalRev += c.revenue; activeCamps++; }

    float expected = cl.budget * 0.5f;
    float delta    = (expected > 0.f) ? (totalRev - expected) / expected * 20.f : 0.f;
    cl.satisfaction = std::clamp(cl.satisfaction + delta, 0.f, 100.f);
    if (activeCamps == 0) cl.satisfaction = std::max(0.f, cl.satisfaction - 5.f);

    if (--cl.contractMonths <= 0 && cl.satisfaction < 50.f) {
        cl.active = false;
        gs.stats.clientsLost++;
    }
}

void Simulation::ProcessAICompetitors(GameState& gs) {
    for (auto& ai : gs.competitors) {
        float growth = (static_cast<float>(rand() % 10) - 3.f) * 0.1f * ai.aggressiveness;
        ai.marketShare = std::max(1.f, ai.marketShare + growth);
    }
    gs.playerMarketShare = std::clamp(
        gs.playerMarketShare
            + gs.stats.reputation * 0.01f
            + static_cast<float>(gs.stats.clientsAcquired) * 0.005f,
        0.f, 100.f);
}

void Simulation::AdvanceMonth(GameState& gs) {
    if (++gs.month > 12) { gs.month = 1; gs.year++; }
    gs.stats.monthsPlayed++;

    for (auto& ev : gs.activeEvents) if (--ev.monthsLeft <= 0) ev.active = false;
    gs.activeEvents.erase(
        std::remove_if(gs.activeEvents.begin(), gs.activeEvents.end(),
            [](const NewsEvent& e){ return !e.active; }),
        gs.activeEvents.end());

    for (int i = 0; i < 6; i++)
        gs.channelModifiers[static_cast<ChannelType>(i)] = 1.0f;
    for (const auto& ev : gs.activeEvents) {
        gs.channelModifiers[ChannelType::Social]     *= ev.socialMod;
        gs.channelModifiers[ChannelType::SEO]        *= ev.seoMod;
        gs.channelModifiers[ChannelType::Email]      *= ev.emailMod;
        gs.channelModifiers[ChannelType::Influencer] *= ev.influencerMod;
        gs.channelModifiers[ChannelType::PR]         *= ev.prMod;
        gs.channelModifiers[ChannelType::PaidSearch] *= ev.paidMod;
        gs.budget += ev.budgetImpact;
    }

    gs.monthlyRevenue = 0.f;
    for (auto& c : gs.campaigns)
        if (c.active) {
            ProcessCampaign(c, gs);
            gs.monthlyRevenue     += c.agencyFee;
            gs.stats.totalRevenue += c.agencyFee;
        }

    gs.monthlyExpenses = 0.f;
    for (auto& s : gs.staff) { gs.monthlyExpenses += s.salary; s.monthsHired++; }
    gs.budget += gs.monthlyRevenue - gs.monthlyExpenses;

    for (auto& cl : gs.clients) if (cl.active) UpdateClientSatisfaction(cl, gs);
    ProcessAICompetitors(gs);

    if (gs.monthlyRevenue > gs.stats.bestMonthRevenue)
        gs.stats.bestMonthRevenue = gs.monthlyRevenue;

    float repDelta = (gs.monthlyRevenue > 0.f ? 1.f : -1.f)
                   * static_cast<float>(gs.stats.monthsPlayed) * 0.1f;
    gs.stats.reputation = std::clamp(gs.stats.reputation + repDelta, 0.f, 100.f);

    if (gs.budget < -50000.f)         gs.gameOver = true;
    if (gs.playerMarketShare >= 35.f) gs.victory  = true;
}
