#pragma once
#include "../core/GameState.h"

class Dashboard {
public:
    static void Render(GameState& gs);
private:
    static float s_revenueHistory[24];
    static int   s_histIdx;
};
