#pragma once

enum class DifficultyLevel { Easy, Normal, Hard };

struct DifficultyConfig {
    float startingBudget;
    float aiAggressiveness;  // multiplier on AI base aggressiveness
    float revenueMultiplier; // global revenue modifier
    float eventFrequency;    // 0-1 probability per month
    float clientSatisfactionDecay; // extra decay per month
    const char* label;
    const char* description;
};

namespace Difficulty {
    inline DifficultyConfig Get(DifficultyLevel lvl) {
        switch (lvl) {
            case DifficultyLevel::Easy:
                return {20000.f, 0.4f, 1.30f, 0.20f, 0.0f,
                    "Easy",
                    "$20,000 start. Forgiving economy, slow AI rivals."};
            case DifficultyLevel::Hard:
                return {5000.f, 1.0f, 0.80f, 0.50f, 2.0f,
                    "Hard",
                    "$5,000 start. Aggressive AI, frequent market events."};
            default: // Normal
                return {12500.f, 0.65f, 1.00f, 0.30f, 0.5f,
                    "Normal",
                    "$12,500 start. Balanced economy, adaptive AI rivals."};
        }
    }
}
