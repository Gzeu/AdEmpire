#pragma once
#include "../core/GameState.h"

class SeasonalEvents {
public:
    // Apply monthly seasonal modifiers to gs.channelModifiers
    static void Apply(GameState& gs);
    // Returns human-readable note for current month
    static const char* GetNote(int month);
};
