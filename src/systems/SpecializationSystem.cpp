#include "SpecializationSystem.h"
#include <algorithm>

// clients required to unlock per industry
static const int UNLOCK_THRESHOLD[] = { 2, 2, 3, 3, 2, 2, 2, 2 };
// bonus multiplier per industry
static const float SPEC_BONUS[]     = { 1.25f, 1.30f, 1.35f, 1.40f, 1.30f, 1.25f, 1.25f, 1.45f };

void SpecializationSystem::Init(GameState& gs) {
    gs.specializations.clear();
    for (int i = 0; i < 8; i++) {
        Specialization s;
        s.industry         = (ClientIndustry)i;
        s.bonusMultiplier  = SPEC_BONUS[i];
        s.clientsRequired  = UNLOCK_THRESHOLD[i];
        s.clientsServed    = 0;
        s.unlocked         = false;
        gs.specializations.push_back(s);
    }
}

void SpecializationSystem::Update(GameState& gs) {
    // Count completed/active clients per industry
    for (auto& spec : gs.specializations) {
        int count = 0;
        for (auto& cl : gs.clients)
            if (cl.industry == spec.industry && !cl.available)
                count++;
        spec.clientsServed = count;
        if (!spec.unlocked && count >= spec.clientsRequired)
            spec.unlocked = true;
    }
}

float SpecializationSystem::GetBonus(ClientIndustry industry, const GameState& gs) {
    for (auto& s : gs.specializations)
        if (s.industry == industry && s.unlocked)
            return s.bonusMultiplier;
    return 1.0f;
}

bool SpecializationSystem::IsUnlocked(ClientIndustry industry, const GameState& gs) {
    for (auto& s : gs.specializations)
        if (s.industry == industry) return s.unlocked;
    return false;
}
