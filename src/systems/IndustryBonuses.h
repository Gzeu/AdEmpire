#pragma once
#include "../core/GameState.h"

// Per-industry channel multipliers
// Applied when calculating campaign results for a specific client
struct IndustryChannelBonus {
    float social, seo, email, influencer, pr, paid;
};

class IndustryBonuses {
public:
    static IndustryChannelBonus Get(ClientIndustry ind);
    static float GetBonus(ClientIndustry ind, ChannelType ch);
};
