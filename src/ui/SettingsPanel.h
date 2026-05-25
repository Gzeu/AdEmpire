#pragma once
#include "../core/GameState.h"
#include "../core/Difficulty.h"

class SettingsPanel {
public:
    static void Render(GameState& gs);
    static Difficulty selectedDifficulty;
};
