#pragma once
#include "../core/GameState.h"
#include <string>
#include <fstream>
#include <sstream>

// Loads custom events from assets/data/custom_events.json
// Format: same structure as EventSystem events
class ModLoader {
public:
    static void LoadCustomEvents(GameState& gs,
        const std::string& path = "assets/data/custom_events.json") {
        std::ifstream f(path);
        if (!f.is_open()) return; // no mod file = no problem

        // Simple JSON parser for event arrays
        // Each event object: { "title": "...", "description": "...",
        //   "impact": "...", "socialMod": 1.2, "seoMod": 1.0, ...,
        //   "budgetImpact": 0, "durationMonths": 2 }
        std::string content((std::istreambuf_iterator<char>(f)),
                             std::istreambuf_iterator<char>());

        // Store raw JSON for display; actual parsing requires nlohmann/json
        // When nlohmann is available, replace this stub:
        /*
        #include <nlohmann/json.hpp>
        auto j = nlohmann::json::parse(content);
        for (auto& ev : j) {
            NewsEvent e;
            e.title        = ev["title"];
            e.description  = ev["description"];
            e.impact       = ev.value("impact", "");
            e.socialMod    = ev.value("socialMod",    1.f);
            e.seoMod       = ev.value("seoMod",       1.f);
            e.emailMod     = ev.value("emailMod",     1.f);
            e.influencerMod= ev.value("influencerMod",1.f);
            e.prMod        = ev.value("prMod",        1.f);
            e.paidMod      = ev.value("paidMod",      1.f);
            e.budgetImpact = ev.value("budgetImpact", 0.f);
            e.durationMonths = ev.value("durationMonths", 1);
            e.active = false; e.monthsLeft = 0;
            customEvents.push_back(e);
        }
        */
        (void)gs; // suppress unused warning on stub
    }

    static std::vector<NewsEvent> customEvents;
};
