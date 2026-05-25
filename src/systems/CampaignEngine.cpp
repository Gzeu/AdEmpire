#include "CampaignEngine.h"
#include "../core/Simulation.h"
#include <algorithm>

float CampaignEngine::CalcQualityScore(const Campaign& c, const GameState& gs) {
    float score = 5.0f;
    for (const auto& s : gs.staff)
        if (static_cast<int>(s.role) == static_cast<int>(c.channel))
            score += s.skill * 3.0f;
    score += gs.stats.reputation * 0.03f;
    return std::clamp(score, 0.f, 10.f);
}

Campaign CampaignEngine::CreateCampaign(
    const std::string& name, int clientId,
    ChannelType channel, float budget,
    int durationMonths, const GameState& gs)
{
    static int nextId = 1;
    Campaign c;
    c.id             = nextId++;
    c.name           = name;
    c.clientId       = clientId;
    c.channel        = channel;
    c.budget         = budget;
    c.durationMonths = durationMonths;
    c.monthsLeft     = durationMonths;
    c.active         = true;
    c.completed      = false;
    c.qualityScore   = CalcQualityScore(c, gs);
    return c;
}

float CampaignEngine::EstimateROI(const Campaign& c, const GameState& gs) {
    if (c.budget <= 0.f) return 0.f;
    return (Simulation::CalcRevenue(c, gs) * 0.15f / c.budget) * 100.f;
}
