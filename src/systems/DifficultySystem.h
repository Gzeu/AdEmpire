#pragma once
#include <string>

enum class Difficulty { Easy, Normal, Hard };

struct DifficultyConfig {
    std::string label;
    float startBudget;          // initial player budget
    float aiAggressiveness;     // multiplier on AI poach chance
    float eventFrequency;       // 0-1, chance per month for market event
    float clientSatisfactionDecay; // extra decay per month
    float reputationGrowth;     // multiplier on reputation gain
    float campaignCostMod;      // multiplier on campaign budget deduction
    std::string description;
};

class DifficultySystem {
public:
    static DifficultyConfig Get(Difficulty d);
    static const char* Label(Difficulty d);

    // Applied once at game start
    static void Apply(Difficulty d, struct GameState& gs);
};
