#pragma once
#include "../core/GameState.h"

class NegotiationPanel {
public:
    static void Render(GameState& gs);
    static void StartNegotiation(int clientId, GameState& gs);
private:
    static int  s_clientId;
    static bool s_active;
};
