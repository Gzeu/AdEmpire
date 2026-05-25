#pragma once
#include "GameState.h"
#include <vector>

// Clients unlocked progressively based on month played
struct GatedClient {
    Client client;
    int    unlockMonth;  // becomes available after this month
    float  minReputation; // and minimum reputation
};

class ClientGating {
public:
    static std::vector<GatedClient> BuildPool();
    static void UnlockClients(GameState& gs);
};
