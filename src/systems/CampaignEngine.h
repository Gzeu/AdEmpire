#pragma once
#include "../core/GameState.h"
#include <string>

class CampaignEngine {
public:
    static Campaign CreateCampaign(
        const std::string& name,
        int clientId,
        ChannelType channel,
        float budget,
        int durationMonths,
        const GameState& gs
    );
    static float EstimateROI(const Campaign& c, const GameState& gs);
    static float CalcQualityScore(const Campaign& c, const GameState& gs);
};
