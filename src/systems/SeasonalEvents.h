#pragma once
#include "../core/GameState.h"

// Multipliers applied on top of regular channel mods each month
struct SeasonalModifiers {
    float social;
    float seo;
    float email;
    float influencer;
    float pr;
    float paidSearch;
    const char* description;
};

class SeasonalEvents {
public:
    static SeasonalModifiers GetForMonth(int month); // 1-12
    static void Apply(GameState& gs);                // stacks into channelModifiers
    static const char* GetSeasonLabel(int month);
};
