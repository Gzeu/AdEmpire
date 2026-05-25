#include "DifficultySystem.h"
#include "../core/GameState.h"

DifficultyConfig DifficultySystem::Get(Difficulty d) {
    switch (d) {
    case Difficulty::Easy:
        return {
            "Easy",
            20000.f,   // $20K start
            0.3f,      // AI low aggression
            0.15f,     // 15% event chance/month
            0.f,       // no extra satisfaction decay
            1.5f,      // reputation grows faster
            1.0f,
            "Relaxed competition. Great for learning the game."
        };
    case Difficulty::Normal:
        return {
            "Normal",
            10000.f,
            0.6f,
            0.30f,
            0.f,
            1.0f,
            1.0f,
            "Balanced challenge. Recommended for new players."
        };
    case Difficulty::Hard:
        return {
            "Hard",
            5000.f,    // $5K start
            1.0f,      // AI max aggression
            0.50f,     // 50% event chance/month
            2.f,       // faster satisfaction decay
            0.6f,      // reputation grows slower
            1.2f,      // campaigns cost 20% more
            "Aggressive AI, frequent crises, low starting capital."
        };
    }
    return Get(Difficulty::Normal);
}

const char* DifficultySystem::Label(Difficulty d) {
    switch (d) {
    case Difficulty::Easy:   return "Easy";
    case Difficulty::Normal: return "Normal";
    case Difficulty::Hard:   return "Hard";
    }
    return "Normal";
}

void DifficultySystem::Apply(Difficulty d, GameState& gs) {
    auto cfg = Get(d);
    gs.budget = cfg.startBudget;
    for (auto& ai : gs.competitors)
        ai.aggressiveness *= cfg.aiAggressiveness;
}
