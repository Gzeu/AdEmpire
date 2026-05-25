#pragma once
#include "../core/GameState.h"

class AchievementsPanel {
public:
    static void Render(GameState& gs);
    static void RenderPopupOverlay(); // call every frame
};
