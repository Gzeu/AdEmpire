#include "CampaignEngine.h"
#include "../core/GameState.h"
#include "../core/Campaign.h"
#include <algorithm>
#include <cmath>

void CampaignEngine::Tick(GameState& gs, float deltaTime) {
    for (auto& campaign : gs.activeCampaigns) {
        if (!campaign.active) continue;

        campaign.daysRemaining -= deltaTime;

        // A: revenueMultiplier now ACTIVE — market state drives real income
        float marketMod = std::clamp(gs.revenueMultiplier, 0.3f, 2.0f);
        float diffMod   = gs.difficultyMultiplier > 0.0f ? gs.difficultyMultiplier : 1.0f;
        float staffMod  = 1.0f + (campaign.assignedStaffCount * 0.05f);

        float earned = campaign.revenue * marketMod * diffMod * staffMod;
        gs.money += earned * deltaTime;

        // Track stats
        gs.stats.totalRevenue   += earned * deltaTime;
        gs.stats.monthlyRevenue += earned * deltaTime;

        // Client satisfaction decay on low budget
        if (campaign.budget < campaign.minBudget) {
            campaign.clientSatisfaction -= 0.5f * deltaTime;
        }

        // Market bonus feedback in stats
        if (marketMod > 1.2f)
            gs.stats.marketBonusTicks++;
        else if (marketMod < 0.7f)
            gs.stats.marketPenaltyTicks++;

        if (campaign.daysRemaining <= 0.0f) {
            campaign.active = false;
            gs.stats.campaignsCompleted++;
            gs.completedCampaigns.push_back(campaign);
        }
    }

    // Remove finished
    gs.activeCampaigns.erase(
        std::remove_if(gs.activeCampaigns.begin(), gs.activeCampaigns.end(),
            [](const Campaign& c){ return !c.active; }),
        gs.activeCampaigns.end()
    );
}

float CampaignEngine::ComputeROI(const Campaign& c, const GameState& gs) {
    if (c.budget <= 0.0f) return 0.0f;
    float gross = c.revenue * std::clamp(gs.revenueMultiplier, 0.3f, 2.0f);
    return (gross - c.budget) / c.budget * 100.0f;
}
