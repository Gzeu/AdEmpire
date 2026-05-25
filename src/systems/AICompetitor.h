#pragma once
#include "../core/GameState.h"

class AICompetitor {
public:
    static void Init(GameState& gs);
    static void ProcessTurn(GameState& gs);
    static void TryPoachClient(AIAgency& ai, GameState& gs);
};
