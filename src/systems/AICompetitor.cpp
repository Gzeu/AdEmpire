#include "AICompetitor.h"
#include "../ui/ToastSystem.h"
#include <cstdlib>
#include <algorithm>
#include <string>

void AICompetitor::Init(GameState& gs) {
    gs.competitors = {
        {"MediaBlaze",    18.f, 50000.f, 70.f, 0.70f, 5, "volume"},
        {"PeakBrands",    15.f, 80000.f, 85.f, 0.45f, 4, "premium"},
        {"NicheNation",   10.f, 30000.f, 60.f, 0.90f, 3, "niche"},
    };
}

void AICompetitor::TryPoachClient(AIAgency& ai, GameState& gs) {
    if ((float)(rand() % 100) / 100.f > ai.aggressiveness * 0.5f) return;
    for (auto& cl : gs.clients) {
        if (!cl.active) continue;
        if (cl.satisfaction < 55.f) {
            float stealChance = (55.f - cl.satisfaction) / 55.f * ai.aggressiveness * 0.6f;
            if ((float)(rand() % 100) / 100.f < stealChance) {
                std::string msg = ai.name + " poached client: " + cl.name;
                TOAST_WARN(msg);
                cl.active = false;
                gs.stats.clientsLost++;
                ai.clientCount++;
                ai.marketShare = std::min(ai.marketShare + 1.2f, 35.f);
                gs.playerMarketShare = std::max(0.f, gs.playerMarketShare - 1.2f);
                break; // one per turn
            }
        }
    }
}

void AICompetitor::ProcessTurn(GameState& gs) {
    for (auto& ai : gs.competitors) {
        TryPoachClient(ai, gs);
        float growth = (float)(rand() % 5) * 0.08f * ai.aggressiveness;
        ai.budget *= (1.f + growth * 0.01f);
        ai.reputation = std::min(ai.reputation + 0.2f, 100.f);
    }
}
