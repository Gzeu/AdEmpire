#pragma once
#include "../core/GameState.h"

class MainMenu {
public:
    static bool Render(GameState& gs); // returns true when game should start
    static char s_agencyNameBuf[64];
    static bool s_showMenu;
};
