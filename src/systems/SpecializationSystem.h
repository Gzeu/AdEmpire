#pragma once
#include "../core/GameState.h"

class SpecializationSystem {
public:
    static void Init(GameState& gs);
    static void Update(GameState& gs);       // call each month
    static float GetBonus(const Client& cl, const GameState& gs); // campaign multiplier
    static bool  IsUnlocked(ClientIndustry industry, const GameState& gs);
};
