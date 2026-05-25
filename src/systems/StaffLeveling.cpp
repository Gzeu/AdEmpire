#include "StaffLeveling.h"
#include "ToastSystem.h"
#include <algorithm>

static const float SKILL_GROWTH_PER_MONTH = 0.008f; // +0.8% per month

int StaffLeveling::GetLevel(float skill) {
    if (skill >= 0.90f) return 4; // Principal
    if (skill >= 0.75f) return 3; // Lead
    if (skill >= 0.55f) return 2; // Senior
    if (skill >= 0.35f) return 1; // Mid
    return 0;                     // Junior
}

const char* StaffLeveling::GetLevelName(float skill) {
    return LevelNames[GetLevel(skill)];
}

void StaffLeveling::ProcessMonth(GameState& gs) {
    for (auto& s : gs.staff) {
        int prevLevel = GetLevel(s.skill);
        s.skill = std::min(s.skill + SKILL_GROWTH_PER_MONTH, 1.0f);
        s.monthsHired++;
        int newLevel = GetLevel(s.skill);
        if (newLevel > prevLevel) {
            s.level  = newLevel;
            s.salary *= 1.08f; // +8% on promotion
            TOAST_SUCCESS(s.name + " promoted to " + LevelNames[newLevel] + "!");
        }
    }
}
