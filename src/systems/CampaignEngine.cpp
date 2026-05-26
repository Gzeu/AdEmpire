#include "CampaignEngine.h"
#include "../core/GameState.h"
#include "ToastSystem.h"
#include <algorithm>
#include <cmath>

// ============================================================
//  CampaignEngine — monthly revenue calculation
//  v1.1: revenueMultiplier (real-market signal) applied
//        Range: 0.3 (crash/panic) – 2.0 (bull+euphoria)
// ============================================================

namespace CampaignEngine {

// Staff skill multiplier for the assigned channel
static float CalcStaffMod(const GameState& gs, const Campaign& c) {
    float best = 0.f;
    for (const auto& s : gs.staff) {
        // Match role to channel
        bool match = false;
        switch (c.channel) {
            case ChannelType::Social:     match = (s.role == StaffRole::SocialMediaManager); break;
            case ChannelType::SEO:        match = (s.role == StaffRole::SEOSpecialist);      break;
            case ChannelType::Email:      match = (s.role == StaffRole::ContentCreator);     break;
            case ChannelType::Influencer: match = (s.role == StaffRole::AccountManager);     break;
            case ChannelType::PR:         match = (s.role == StaffRole::PRManager);          break;
            case ChannelType::PaidSearch: match = (s.role == StaffRole::DataAnalyst);        break;
        }
        if (match) best = std::max(best, s.skill);
    }
    // 0.8 base when no matching staff; up to 1.3 at skill 1.0
    return 0.8f + best * 0.5f;
}

// Difficulty modifier from DifficultySystem
static float CalcDiffMod(const GameState& gs) {
    // Difficulty stored as a simple float in [0.5, 1.0, 1.5]
    // Default 1.0 if not set
    return 1.0f; // DifficultySystem patches this via GameState::difficultyMod if needed
}

void ProcessMonth(GameState& gs) {
    // Clamp revenueMultiplier to safe range
    float marketMod = std::clamp(gs.revenueMultiplier, 0.3f, 2.0f);
    bool  isBull    = marketMod > 1.1f;
    bool  isBear    = marketMod < 0.85f;

    for (auto& campaign : gs.campaigns) {
        if (!campaign.active || campaign.completed) continue;

        float staffMod = CalcStaffMod(gs, campaign);
        float diffMod  = CalcDiffMod(gs);

        // ── v1.1: real-market multiplier applied here ──────────────────────
        float earned = campaign.revenue * marketMod * diffMod * staffMod;

        // Campaign quality bonus
        earned *= (0.7f + campaign.qualityScore * 0.6f);

        // Channel event modifier
        auto it = gs.channelModifiers.find(campaign.channel);
        if (it != gs.channelModifiers.end())
            earned *= it->second;

        campaign.agencyFee  = earned;
        gs.budget          += earned;
        gs.monthlyRevenue  += earned;
        gs.stats.totalRevenue += earned;

        // ── Track market influence on stats ───────────────────────────────
        if (isBull)  gs.stats.marketBonusTicks++;
        if (isBear)  gs.stats.marketPenaltyTicks++;

        // Advance campaign duration
        campaign.monthsLeft--;
        if (campaign.monthsLeft <= 0) {
            campaign.active    = false;
            campaign.completed = true;
            gs.stats.campaignsCompleted++;

            // Update client satisfaction
            for (auto& cl : gs.clients) {
                if (cl.id == campaign.clientId) {
                    float satisfactionDelta = (campaign.qualityScore - 0.5f) * 20.f;
                    satisfactionDelta += (marketMod - 1.0f) * 10.f; // market context
                    cl.satisfaction = std::clamp(cl.satisfaction + satisfactionDelta, 0.f, 100.f);
                    cl.totalRevenue += earned;
                    break;
                }
            }
        }
    }

    // Best month tracking
    if (gs.monthlyRevenue > gs.stats.bestMonthRevenue)
        gs.stats.bestMonthRevenue = gs.monthlyRevenue;
}

Campaign CreateCampaign(const std::string& name, int clientId, ChannelType channel,
                       float budget, int duration, GameState& gs) {
    Campaign c;
    c.id = gs.nextCampaignId++;
    c.name = name;
    c.clientId = clientId;
    c.channel = channel;
    c.budget = budget;
    c.durationMonths = duration;
    c.monthsLeft = duration;
    c.active = false;
    c.completed = false;
    c.qualityScore = 0.5f; // default

    // Calculate initial metrics based on budget and channel
    float staffMod = CalcStaffMod(gs, c);
    c.agencyFee = budget * 0.15f * staffMod;
    c.revenue = budget * 0.3f * staffMod;
    c.reach = budget * 10.f * staffMod;
    c.ctr = 0.02f + (staffMod - 0.8f) * 0.01f;
    c.conversionRate = 0.01f + (staffMod - 0.8f) * 0.005f;

    return c;
}

float EstimateROI(const Campaign& c, const GameState& gs) {
    float staffMod = CalcStaffMod(gs, c);
    float estimatedRevenue = c.budget * 0.3f * staffMod * c.durationMonths;
    float totalCost = c.budget * c.durationMonths;
    if (totalCost <= 0.f) return 0.f;
    return (estimatedRevenue / totalCost) * 100.f;
}

} // namespace CampaignEngine
