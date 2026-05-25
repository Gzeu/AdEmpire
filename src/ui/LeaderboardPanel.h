#pragma once
#include "../core/GameState.h"

class LeaderboardPanel {
public:
    static void Render(GameState& gs, bool& show);
    static void SubmitScore(const GameState& gs, const std::string& difficulty);
};
