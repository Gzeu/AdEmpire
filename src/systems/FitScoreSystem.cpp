#include "FitScoreSystem.h"
#include <algorithm>

// Best channel per industry
static const ChannelType INDUSTRY_BEST_CHANNEL[] = {
    ChannelType::Social,     // Food
    ChannelType::Influencer, // Fashion
    ChannelType::SEO,        // Tech
    ChannelType::PaidSearch, // Finance
    ChannelType::SEO,        // Health
    ChannelType::Email,      // Education
    ChannelType::PaidSearch, // Retail
    ChannelType::Social      // Gaming
};

FitScore FitScoreSystem::Calculate(const Client& cl, const GameState& gs) {
    FitScore score;

    // 1. Channel fit (0-25): does agency have staff for best channel?
    ChannelType bestCh = INDUSTRY_BEST_CHANNEL[(int)cl.industry];
    float channelSkill = 0.f;
    for (auto& s : gs.staff)
        if ((int)s.role == (int)bestCh)
            channelSkill = std::max(channelSkill, s.skill);
    score.channel = channelSkill * 25.f;

    // 2. Industry experience (0-25): prior clients in same industry
    int sameIndustry = 0;
    for (auto& c : gs.clients)
        if (c.industry == cl.industry && !c.available && c.id != cl.id)
            sameIndustry++;
    score.industry = std::min(sameIndustry * 5.f, 25.f);

    // 3. Reputation fit (0-25): big clients need established agencies
    float repRequired = cl.budget / 500.f; // $500 budget = 1 rep needed
    float repRatio = (repRequired > 0)
        ? std::min(gs.stats.reputation / repRequired, 1.f) : 1.f;
    score.reputation = repRatio * 25.f;

    // 4. Capacity fit (0-25): penalty if overloaded
    int activeClients = 0;
    for (auto& c : gs.clients) if (c.active) activeClients++;
    int maxCap = 2 + (int)gs.staff.size();
    float capRatio = (maxCap > 0)
        ? 1.f - ((float)activeClients / (float)maxCap)
        : 0.f;
    score.capacity = std::max(0.f, capRatio * 25.f);

    return score;
}

void FitScoreSystem::UpdateCapacity(GameState& gs) {
    int active = 0;
    for (auto& c : gs.clients) if (c.active) active++;
    int maxCap = 2 + (int)gs.staff.size();
    gs.capacity.maxClients     = maxCap;
    gs.capacity.utilizationPct = maxCap > 0 ? (float)active / maxCap * 100.f : 0.f;
    gs.capacity.overloaded     = active > maxCap;
    gs.capacity.burnoutRisk    = gs.capacity.utilizationPct > 80.f;
}

float FitScoreSystem::GetWinProbability(const Client& cl, const GameState& gs) {
    FitScore fit = Calculate(cl, gs);
    float total  = fit.total();
    // Base 20% + fit contribution up to 70% + rep bonus up to 10%
    float prob = 0.20f + (total / 100.f) * 0.70f + gs.stats.reputation * 0.001f;
    return std::clamp(prob, 0.05f, 0.95f);
}
