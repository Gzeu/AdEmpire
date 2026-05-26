#pragma once
#include "../core/GameState.h"

namespace CampaignEngine {
    // Called once per month by Simulation::AdvanceMonth()
    // Applies revenueMultiplier (real-market signal) to all active campaigns
    void ProcessMonth(GameState& gs);

    // Create a campaign object with calculated initial metrics
    Campaign CreateCampaign(const std::string& name, int clientId, ChannelType channel,
                           float budget, int duration, GameState& gs);

    // Estimate ROI percentage for a campaign
    float EstimateROI(const Campaign& c, const GameState& gs);
}
