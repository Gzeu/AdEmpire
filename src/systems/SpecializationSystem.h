#pragma once
#include "../core/GameState.h"

class SpecializationSystem {
public:
    static void Init(GameState& gs);
    static void Update(GameState& gs);
    static float GetBonus(ClientIndustry industry, const GameState& gs);
    static bool  IsUnlocked(ClientIndustry industry, const GameState& gs);
};
