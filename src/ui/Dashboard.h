#pragma once
#include "../core/GameState.h"
#include "../network/MarketFeed.h"
#include "../network/MarketEventBridge.h"
#include "LiveMarketPanel.h"

class Dashboard {
public:
    static void Render(GameState& gs);
private:
    static float s_revenueHistory[24];
    static int   s_histIdx;
    // v1.0: active tab memory (persists across frames)
    static int   s_activeTab;
};
