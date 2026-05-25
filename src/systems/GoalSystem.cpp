#include "GoalSystem.h"
#include <algorithm>
#include <cmath>

void GoalSystem::InitQuarterlyGoals(GameState& gs) {
    gs.quarterlyGoals.clear();
    float diff = 1.0f;

    QuarterlyGoal g1;
    g1.type    = GoalType::Revenue;
    g1.target  = 5000.f * diff;
    g1.current = 0.f;
    g1.reward  = 2000.f;
    g1.quarter = 1;
    g1.completed = g1.failed = false;
    gs.quarterlyGoals.push_back(g1);

    QuarterlyGoal g2;
    g2.type    = GoalType::ClientCount;
    g2.target  = 2.f;
    g2.current = 0.f;
    g2.reward  = 1500.f;
    g2.quarter = 1;
    g2.completed = g2.failed = false;
    gs.quarterlyGoals.push_back(g2);

    QuarterlyGoal g3;
    g3.type    = GoalType::CampaignCount;
    g3.target  = 3.f;
    g3.current = 0.f;
    g3.reward  = 1000.f;
    g3.quarter = 1;
    g3.completed = g3.failed = false;
    gs.quarterlyGoals.push_back(g3);
}

void GoalSystem::UpdateGoals(GameState& gs) {
    int activeClients = 0;
    for (auto& c : gs.clients) if (c.active) activeClients++;

    for (auto& goal : gs.quarterlyGoals) {
        if (goal.completed || goal.failed) continue;
        switch (goal.type) {
            case GoalType::Revenue:
                goal.current = gs.monthlyRevenue; break;
            case GoalType::ClientCount:
                goal.current = (float)activeClients; break;
            case GoalType::MarketShare:
                goal.current = gs.playerMarketShare; break;
            case GoalType::CampaignCount:
                goal.current = (float)gs.stats.campaignsCompleted; break;
        }
    }
}

void GoalSystem::CheckCompletion(GameState& gs) {
    // Check if quarter ended (every 3 months)
    bool quarterEnd = (gs.month % 3 == 0);

    for (auto& goal : gs.quarterlyGoals) {
        if (goal.completed || goal.failed) continue;
        if (goal.current >= goal.target) {
            goal.completed = true;
            gs.budget += goal.reward;
            gs.stats.reputation = std::clamp(gs.stats.reputation + 5.f, 0.f, 100.f);
        } else if (quarterEnd) {
            goal.failed = true;
        }
    }

    if (quarterEnd) GenerateNewQuarter(gs);
}

void GoalSystem::GenerateNewQuarter(GameState& gs) {
    int q = (gs.month / 3) + 1;
    float diff = std::pow(1.3f, q - 1); // 30% harder each quarter

    gs.quarterlyGoals.clear();

    QuarterlyGoal g1;
    g1.type = GoalType::Revenue;     g1.target = 5000.f * diff;
    g1.current = 0.f; g1.reward = 2000.f * diff; g1.quarter = q;
    g1.completed = g1.failed = false;
    gs.quarterlyGoals.push_back(g1);

    QuarterlyGoal g2;
    g2.type = GoalType::ClientCount; g2.target = 2.f + q;
    g2.current = 0.f; g2.reward = 1500.f * diff; g2.quarter = q;
    g2.completed = g2.failed = false;
    gs.quarterlyGoals.push_back(g2);

    QuarterlyGoal g3;
    g3.type = GoalType::MarketShare; g3.target = 5.f + q * 2.f;
    g3.current = 0.f; g3.reward = 2500.f * diff; g3.quarter = q;
    g3.completed = g3.failed = false;
    gs.quarterlyGoals.push_back(g3);

    QuarterlyGoal g4;
    g4.type = GoalType::CampaignCount; g4.target = 3.f + q;
    g4.current = 0.f; g4.reward = 1200.f * diff; g4.quarter = q;
    g4.completed = g4.failed = false;
    gs.quarterlyGoals.push_back(g4);
}
