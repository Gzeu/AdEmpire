#pragma once
#include "../core/GameState.h"
#include <string>

// Staff gains +0.8% skill per month; every 5 levels salary bumps +8%
struct LevelInfo {
    int         level;       // 1-5
    std::string title;       // Junior, Mid, Senior, Lead, Principal
    float       skillGain;   // per month
    float       salaryMod;   // multiplier at this level
};

class StaffLeveling {
public:
    static constexpr float SKILL_GAIN_PER_MONTH = 0.008f;
    static constexpr float SALARY_BUMP_PER_LEVEL = 0.08f;

    static LevelInfo GetLevelInfo(const StaffMember& s);
    static void      AdvanceMonth(GameState& gs); // call each month
    static int       GetLevel(float skill);       // 0.0-1.0 -> 1-5
    static float     GetSkillThreshold(int level);
};
