#include "GoalSystem.h"
#include <algorithm>
#include <cstdio>

int GoalSystem::CurrentQuarter(const GameState& gs) {
    return ((gs.month - 1) / 3) + 1; // 1-4
}

void GoalSystem::GenerateGoalsForQuarter(int quarter, GameState& gs) {
    gs.quarterlyGoals.clear();
    float difficulty = 1.f + (quarter - 1) * 0.3f; // harder each quarter
    float revTarget  = 3000.f * difficulty;
    int   cliTarget  = 1 + quarter;
    float shareTarget = 2.f + quarter * 2.f;
    int   campTarget  = 2 + quarter;

    gs.quarterlyGoals.push_back({
        GoalType::RevenueTarget, revTarget, 0.f, false, false,
        revTarget * 0.5f, 5.f, quarter,
        "Earn $" + std::to_string((int)revTarget) + " in agency fees"
    });
    gs.quarterlyGoals.push_back({
        GoalType::ClientCount, (float)cliTarget, 0.f, false, false,
        2000.f * quarter, 3.f, quarter,
        "Have " + std::to_string(cliTarget) + " active clients"
    });
    gs.quarterlyGoals.push_back({
        GoalType::CampaignsLaunched, (float)campTarget, 0.f, false, false,
        1000.f * quarter, 2.f, quarter,
        "Launch " + std::to_string(campTarget) + " campaigns"
    });
    if (quarter >= 2) {
        gs.quarterlyGoals.push_back({
            GoalType::MarketShare, shareTarget, 0.f, false, false,
            3000.f * quarter, 5.f, quarter,
            "Reach " + std::to_string((int)shareTarget) + "% market share"
        });
    }
}

void GoalSystem::InitQuarterlyGoals(GameState& gs) {
    GenerateGoalsForQuarter(1, gs);
}

void GoalSystem::UpdateGoals(GameState& gs) {
    int activeClients = 0;
    for (auto& c : gs.clients) if (c.active) activeClients++;

    for (auto& g : gs.quarterlyGoals) {
        if (g.completed || g.failed) continue;
        switch (g.type) {
            case GoalType::RevenueTarget:
                g.current = gs.stats.totalRevenue; break;
            case GoalType::ClientCount:
                g.current = (float)activeClients; break;
            case GoalType::MarketShare:
                g.current = gs.playerMarketShare; break;
            case GoalType::CampaignsLaunched:
                g.current = (float)gs.stats.campaignsCompleted; break;
            case GoalType::SatisfactionAvg: {
                float sum = 0.f; int cnt = 0;
                for (auto& c : gs.clients)
                    if (c.active) { sum += c.satisfaction; cnt++; }
                g.current = cnt > 0 ? sum / cnt : 0.f;
                break;
            }
        }
    }
}

void GoalSystem::CheckCompletion(GameState& gs) {
    int quarter = CurrentQuarter(gs);
    for (auto& g : gs.quarterlyGoals) {
        if (g.completed || g.failed) continue;
        if (g.current >= g.target) {
            g.completed = true;
            gs.budget += g.rewardBudget;
            gs.stats.reputation += g.rewardReputation;
            gs.stats.quarterlyGoalsMet++;
        }
        // Fail check: end of quarter and not met
        if (quarter > g.quarterDue && !g.completed)
            g.failed = true;
    }
    // Start new quarter goals
    bool allDone = true;
    for (auto& g : gs.quarterlyGoals)
        if (!g.completed && !g.failed) { allDone = false; break; }
    if (allDone && quarter <= 4)
        GenerateGoalsForQuarter(quarter + 1, gs);
}
