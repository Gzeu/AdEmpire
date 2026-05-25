#pragma once
#include "../core/GameState.h"

class GoalSystem {
public:
    static void InitQuarterlyGoals(GameState& gs);
    static void UpdateGoals(GameState& gs);
    static void CheckCompletion(GameState& gs);
    static void GenerateNewQuarter(GameState& gs);
};
