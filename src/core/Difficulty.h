#pragma once

enum class Difficulty { Easy, Normal, Hard };

struct DifficultySettings {
    float startBudget;        // starting $
    float revenueMultiplier;  // agency fee multiplier
    float aiAggressiveness;   // AI poach multiplier
    float eventFrequency;     // 0-1 chance per month
    float satisfactionDecay;  // extra monthly satisfaction drop
    int   startClients;       // clients available at start
    const char* label;
    const char* description;
};

static const DifficultySettings DIFFICULTIES[] = {
    // Easy
    { 20000.f, 1.3f, 0.4f, 0.20f, 0.f,  6, "Easy",
      "More starting budget, less aggressive AI, fewer events." },
    // Normal
    { 10000.f, 1.0f, 0.7f, 0.30f, 2.f,  6, "Normal",
      "Balanced experience. The intended way to play." },
    // Hard
    { 5000.f,  0.8f, 1.0f, 0.45f, 5.f,  4, "Hard",
      "Half the budget, ruthless AI, frequent events." },
};
