#include "StaffLeveling.h"
#include "../ui/ToastSystem.h"
#include <algorithm>
#include <cstdio>

static const char* LEVEL_TITLES[] = { "", "Junior", "Mid", "Senior", "Lead", "Principal" };
static const float SKILL_THRESHOLDS[] = { 0.0f, 0.0f, 0.35f, 0.55f, 0.75f, 0.90f };

int StaffLeveling::GetLevel(float skill) {
    if (skill >= 0.90f) return 5;
    if (skill >= 0.75f) return 4;
    if (skill >= 0.55f) return 3;
    if (skill >= 0.35f) return 2;
    return 1;
}

float StaffLeveling::GetSkillThreshold(int level) {
    if (level < 1 || level > 5) return 0.f;
    return SKILL_THRESHOLDS[level];
}

LevelInfo StaffLeveling::GetLevelInfo(const StaffMember& s) {
    int lv = GetLevel(s.skill);
    return { lv, LEVEL_TITLES[lv], SKILL_GAIN_PER_MONTH, 1.f + (lv - 1) * SALARY_BUMP_PER_LEVEL };
}

void StaffLeveling::AdvanceMonth(GameState& gs) {
    for (auto& s : gs.staff) {
        int prevLevel = GetLevel(s.skill);
        s.skill = std::min(s.skill + SKILL_GAIN_PER_MONTH, 1.0f);
        int newLevel = GetLevel(s.skill);
        if (newLevel > prevLevel) {
            // Salary bump
            float baseSalary = RoleSalaries[(int)s.role];
            s.salary = baseSalary * (1.f + (newLevel - 1) * SALARY_BUMP_PER_LEVEL);
            // Toast
            char msg[128];
            snprintf(msg, 128, "🚀 %s leveled up to %s!",
                s.name.c_str(), LEVEL_TITLES[newLevel]);
            ToastSystem::Push(msg, ToastType::Success);
        }
    }
}
