#pragma once
#include "../core/GameState.h"

class StaffLeveling {
public:
    // Call each month for each staff member
    static void Tick(GameState& gs);
    // Returns level 1-5 based on skill
    static int  GetLevel(const StaffMember& s);
    static const char* GetLevelTitle(int level);
};
