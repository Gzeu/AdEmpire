#include "SpecializationSystem.h"
#include <algorithm>

void SpecializationSystem::Init(GameState& gs) {
    gs.specializations = {
        {"Food & Bev Expert",   "Bonus on all Food campaigns",   ClientIndustry::Food,      1.25f, 2, false},
        {"Fashion Authority",   "Bonus on Fashion campaigns",    ClientIndustry::Fashion,   1.30f, 2, false},
        {"Tech Agency Pro",     "Bonus on Tech campaigns",       ClientIndustry::Tech,      1.35f, 3, false},
        {"FinTech Specialist",  "Bonus on Finance campaigns",    ClientIndustry::Finance,   1.40f, 3, false},
        {"Health Marketer",     "Bonus on Health campaigns",     ClientIndustry::Health,    1.30f, 2, false},
        {"EduTech Partner",     "Bonus on Education campaigns",  ClientIndustry::Education, 1.25f, 2, false},
        {"Retail Dominator",    "Bonus on Retail campaigns",     ClientIndustry::Retail,    1.35f, 3, false},
        {"Gaming Agency",       "Bonus on Gaming campaigns",     ClientIndustry::Gaming,    1.45f, 2, false},
    };
}

void SpecializationSystem::Update(GameState& gs) {
    for (auto& sp : gs.specializations) {
        if (sp.unlocked) continue;
        auto it = gs.stats.industryClientCount.find((int)sp.industry);
        if (it != gs.stats.industryClientCount.end()
            && it->second >= sp.clientsRequired)
            sp.unlocked = true;
    }
}

bool SpecializationSystem::IsUnlocked(ClientIndustry industry, const GameState& gs) {
    for (auto& sp : gs.specializations)
        if (sp.industry == industry && sp.unlocked) return true;
    return false;
}

float SpecializationSystem::GetBonus(const Client& cl, const GameState& gs) {
    for (auto& sp : gs.specializations)
        if (sp.industry == cl.industry && sp.unlocked)
            return sp.bonusMultiplier;
    return 1.0f;
}
