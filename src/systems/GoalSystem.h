#pragma once
#include "../core/GameState.h"

class GoalSystem {
public:
    static void InitQuarterlyGoals(GameState& gs);
    static void GenerateGoalsForQuarter(int quarter, GameState& gs);
    static void UpdateGoals(GameState& gs);
    static void CheckCompletion(GameState& gs);
    static int  CurrentQuarter(const GameState& gs);
};
