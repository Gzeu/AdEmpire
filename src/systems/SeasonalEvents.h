#pragma once
#include "../core/GameState.h"

class SeasonalEvents {
public:
    // Call at start of each month
    static void Apply(GameState& gs);
    static const char* GetSeasonName(int month);
};
