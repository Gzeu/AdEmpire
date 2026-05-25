#pragma once
#include "../core/GameState.h"

class FitScoreSystem {
public:
    static FitScore  Calculate(const Client& cl, const GameState& gs);
    static void      UpdateCapacity(GameState& gs);
    static float     GetWinProbability(const Client& cl, const GameState& gs);
};
