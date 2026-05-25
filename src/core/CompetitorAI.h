#pragma once
#include <string>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include "GameState.h"

// ─── Competitor (richer than AIAgency stub) ──────────────────────────────────
struct Competitor {
    std::string    name;
    float          marketShare;      // 0–100
    float          aggressiveness;   // 0.5 = passive, 2.0 = shark
    ClientIndustry specialization;   // industry they target
    float          reputation;       // 0–100, affects steal probability
    std::string    lastAction;       // shown in UI
    int            clientsStolen;    // lifetime counter
};

// ─── CompetitorAI ─────────────────────────────────────────────────────────────
class CompetitorAI {
public:
    std::vector<Competitor> competitors;

    // Initialise with 3 real rivals balanced per difficulty
    void Init(float difficultyMult = 1.0f) {
        competitors.clear();
        competitors.push_back({
            "Apex Media",
            8.0f * difficultyMult,
            1.2f * difficultyMult,
            ClientIndustry::Tech,
            55.f, "Observing market", 0
        });
        competitors.push_back({
            "BrandForce",
            5.5f * difficultyMult,
            0.9f * difficultyMult,
            ClientIndustry::Fashion,
            48.f, "Running SEO blitz", 0
        });
        competitors.push_back({
            "ViralEdge",
            3.0f * difficultyMult,
            1.5f * difficultyMult,
            ClientIndustry::Gaming,
            40.f, "Targeting your clients", 0
        });
    }

    // Returns event string if competitor steals a client (empty = nothing happened)
    // Call once per AdvanceMonth
    std::string Tick(GameState& gs) {
        std::string event;
        for (auto& comp : competitors) {
            // Natural market share growth / shrink
            float drift = ((rand() % 100) - 45) * 0.05f * comp.aggressiveness;
            comp.marketShare = std::clamp(comp.marketShare + drift, 1.0f, 40.0f);

            // Attempt to steal vulnerable client (satisfaction < 55)
            // Probability = aggressiveness * 8% per vulnerable client
            for (auto& cl : gs.clients) {
                if (!cl.active) continue;
                if (cl.satisfaction >= 55.f) continue;

                // Only target their specialization (or any if very aggressive)
                bool interested = (cl.industry == comp.specialization)
                               || (comp.aggressiveness > 1.4f && rand() % 3 == 0);
                if (!interested) continue;

                // Steal roll: base 12%, +4% per aggressiveness above 1.0
                float stealChance = 0.12f + (comp.aggressiveness - 1.0f) * 0.04f;
                if ((rand() % 100) < (int)(stealChance * 100)) {
                    cl.active = false;
                    gs.stats.clientsLost++;
                    comp.clientsStolen++;
                    comp.marketShare  = std::min(comp.marketShare + 1.5f, 40.0f);
                    comp.reputation   = std::min(comp.reputation + 3.f, 100.f);
                    comp.lastAction   = "Poached " + cl.name;
                    event = comp.name + " poached " + cl.name
                          + " (satisfaction was " + std::to_string((int)cl.satisfaction) + "%)";
                    break; // one steal per competitor per month
                }
            }

            // Random aggressive action flavour
            if (rand() % 5 == 0) {
                static const char* actions[] = {
                    "Launched undercutting campaign",
                    "Running Social blitz",
                    "Hiring star Account Manager",
                    "Targeting your niche",
                    "Offering 20% discount to prospects"
                };
                comp.lastAction = actions[rand() % 5];
            }
        }

        // Player market share also decays slightly if losing clients
        if (gs.stats.clientsLost > 0)
            gs.playerMarketShare = std::max(0.5f, gs.playerMarketShare - 0.3f);

        return event;
    }

    // Returns total competitor market share (for HUD pressure bar)
    float TotalCompetitorMS() const {
        float t = 0.f;
        for (auto& c : competitors) t += c.marketShare;
        return t;
    }
};
