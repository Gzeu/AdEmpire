#include "FitScoreSystem.h"
#include <algorithm>
#include <cmath>

bool FitScoreSystem::HasChannelExpertise(const Client& cl, const GameState& gs) {
    ChannelType best = IndustryBestChannel[(int)cl.industry];
    for (auto& s : gs.staff)
        if ((int)s.role == (int)best && s.skill > 0.5f)
            return true;
    return false;
}

float FitScoreSystem::IndustryBonus(const Client& cl, const GameState& gs) {
    auto it = gs.stats.industryClientCount.find((int)cl.industry);
    if (it == gs.stats.industryClientCount.end()) return 1.0f;
    int count = it->second;
    return std::min(1.0f + count * 0.10f, 1.50f); // +10% per past client, max +50%
}

FitScore FitScoreSystem::Calculate(const Client& cl, const GameState& gs) {
    FitScore fs;

    // Channel fit: do we have the right staff?
    fs.channel = HasChannelExpertise(cl, gs) ? 1.0f : 0.4f;

    // Industry fit: experience with this industry
    fs.industry = std::min((IndustryBonus(cl, gs) - 1.0f) / 0.5f, 1.0f);

    // Reputation fit: client expects minimum reputation
    float repRequired = cl.budget / 500.f; // $500 budget needs rep 1, $15000 needs rep 30
    fs.reputation = std::min(gs.stats.reputation / std::max(repRequired, 1.f), 1.f);

    // Capacity fit: do we have room?
    int activeClients = 0;
    for (auto& c : gs.clients) if (c.active) activeClients++;
    fs.capacity = gs.capacity.maxClients > 0
        ? std::max(0.f, 1.f - (float)activeClients / gs.capacity.maxClients)
        : 0.f;

    return fs;
}

float FitScoreSystem::WinChance(const FitScore& fs, float reputation) {
    float base = fs.total() / 100.f;    // 0-1
    float repBonus = reputation * 0.003f;
    return std::clamp(base * 0.7f + repBonus + 0.15f, 0.05f, 0.97f);
}

void FitScoreSystem::UpdateCapacity(GameState& gs) {
    gs.capacity.maxClients = 1; // always at least 1 (solo founder)
    for (auto& s : gs.staff)
        gs.capacity.maxClients += RoleCapacity[(int)s.role];

    gs.capacity.currentClients = 0;
    for (auto& c : gs.clients) if (c.active) gs.capacity.currentClients++;

    gs.capacity.utilizationPct = gs.capacity.maxClients > 0
        ? (float)gs.capacity.currentClients / gs.capacity.maxClients
        : 1.f;
    gs.capacity.overloaded = gs.capacity.utilizationPct > 1.0f;

    // Agency burnout = average staff burnout
    float totalBurnout = 0.f;
    for (auto& s : gs.staff) {
        // Burnout rises when overloaded, falls when normal
        float targetBurnout = gs.capacity.overloaded
            ? s.burnout + 0.05f
            : std::max(0.f, s.burnout - 0.02f);
        s.burnout = std::clamp(targetBurnout, 0.f, 1.f);
        totalBurnout += s.burnout;
    }
    gs.capacity.burnoutRisk = gs.staff.empty() ? 0.f
        : totalBurnout / gs.staff.size();
}
