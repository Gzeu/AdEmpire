#include "AICompetitor.h"
#include <cstdlib>
#include <algorithm>

void AICompetitor::Init(GameState& gs) {
    gs.competitors = {
        {"MediaBlaze",    18.f, 50000.f, 70.f, 0.7f, 5, "volume"},
        {"PeakBrands",    15.f, 80000.f, 85.f, 0.4f, 4, "premium"},
        {"NicheNation",   10.f, 30000.f, 60.f, 0.9f, 3, "niche"},
    };
}

void AICompetitor::TryPoachClient(AIAgency& ai, GameState& gs) {
    if ((float)(rand() % 100) / 100.f > ai.aggressiveness) return;
    for (auto& cl : gs.clients) {
        if (!cl.active) continue;
        if (cl.satisfaction < 50.f) {
            float stealChance = (50.f - cl.satisfaction) / 50.f * ai.aggressiveness;
            if ((float)(rand() % 100) / 100.f < stealChance) {
                cl.active = false;
                gs.stats.clientsLost++;
                ai.clientCount++;
                ai.marketShare += 1.f;
                gs.playerMarketShare = std::max(0.f, gs.playerMarketShare - 1.f);
                break;
            }
        }
    }
}

void AICompetitor::ProcessTurn(GameState& gs) {
    for (auto& ai : gs.competitors) {
        TryPoachClient(ai, gs);
        // Grow AI naturally
        float growth = (float)(rand() % 5) * 0.1f * ai.aggressiveness;
        ai.budget *= (1.f + growth * 0.01f);
    }
}
