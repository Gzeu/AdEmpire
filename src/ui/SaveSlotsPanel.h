#pragma once
#include "../core/GameState.h"

class SaveSlotsPanel {
public:
    static void Render(GameState& gs, bool& show);
    static int  activeSlot; // currently loaded slot (0 = none)
};
