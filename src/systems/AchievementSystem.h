#pragma once
#include "../core/GameState.h"

class AchievementSystem {
public:
    static void Init(GameState& gs);  // populate gs.achievements
    static void Check(GameState& gs); // call each month — unlocks + toasts
};
