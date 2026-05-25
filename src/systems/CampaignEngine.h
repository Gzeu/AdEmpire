#pragma once

struct GameState;

namespace CampaignEngine {
    // Called once per month by Simulation::AdvanceMonth()
    // Applies revenueMultiplier (real-market signal) to all active campaigns
    void ProcessMonth(GameState& gs);
}
