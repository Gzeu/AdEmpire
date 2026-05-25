#pragma once
#include "../core/GameState.h"

class FitScoreSystem {
public:
    // Calculate how well the agency fits a potential client
    static FitScore Calculate(const Client& cl, const GameState& gs);

    // Win chance for pitch/negotiation (0-1)
    static float WinChance(const FitScore& fs, float agencyReputation);

    // Industry experience bonus (1.0 base, up to 1.5)
    static float IndustryBonus(const Client& cl, const GameState& gs);

    // Returns true if agency has channel expertise for client's best channel
    static bool HasChannelExpertise(const Client& cl, const GameState& gs);

    // Update capacity info
    static void UpdateCapacity(GameState& gs);
};
