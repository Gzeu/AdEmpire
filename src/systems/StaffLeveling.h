#pragma once
#include "../core/GameState.h"

class StaffLeveling {
public:
    // Call once per month — grows skill, promotes levels, adjusts salary
    static void ProcessMonth(GameState& gs);
    static int  GetLevel(float skill); // returns 0-4
    static const char* GetLevelName(float skill);
};
