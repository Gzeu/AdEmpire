#include "EventSystem.h"
#include "../network/MarketFeed.h"
#include "../network/MarketEventBridge.h"
#include "../../vendor/nlohmann/json.hpp"
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <iostream>

using json = nlohmann::json;

// ---------------------------------------------------------------------------
// Load market events from JSON (data/market_events.json).
// Falls back to empty if file missing — static events still apply.
// ---------------------------------------------------------------------------
static std::vector<NewsEvent> LoadMarketEventsFromJSON() {
    std::vector<NewsEvent> result;
    std::ifstream f("data/market_events.json");
    if (!f.is_open()) return result;

    try {
        json arr = json::parse(f);
        for (const auto& ev : arr) {
            NewsEvent ne{};
            ne.title        = ev.value("title",       "Market Event");
            ne.description  = ev.value("description", "");
            ne.effectLabel  = ev.value("effect_label","");
            ne.socialMult   = ev.value("social_mult",   1.0f);
            ne.seoMult      = ev.value("seo_mult",      1.0f);
            ne.emailMult    = ev.value("email_mult",    1.0f);
            ne.influencerMult = ev.value("influencer_mult", 1.0f);
            ne.prMult       = ev.value("pr_mult",       1.0f);
            ne.paidMult     = ev.value("paid_mult",     1.0f);
            ne.cashBonus    = ev.value("cash_bonus",    0);
            ne.durationMonths = ev.value("duration_months", 2);
            ne.active       = false;
            ne.monthsLeft   = 0;
            result.push_back(ne);
        }
    } catch (const std::exception& e) {
        std::cerr << "[EventSystem] Failed to parse market_events.json: " << e.what() << "\n";
    }
    return result;
}

std::vector<NewsEvent> EventSystem::GetAllEvents() {
    // Static built-in events (always available)
    std::vector<NewsEvent> all = {
        {"Algorithm Update",
         "Major social platform changed its algorithm. Organic reach drops 40%.",
         "Social -40%", 0.6f,1,1,1,1,1, 0,2,false,0},
        {"Viral Trend Surge",
         "Viral challenge boosts engagement across all brands.",
         "Social +60%", 1.6f,1,1,1.3f,1,1, 0,1,false,0},
        {"Platform Outage",
         "Major social platform 24h outage.",
         "Social -80%", 0.2f,1,1,1,1,1, -500,1,false,0},
        {"TikTok Boom",
         "Short-form video sees 3x engagement spike.",
         "Social+Influencer +50%", 1.5f,1,1,1.5f,1,1, 0,2,false,0},
        {"Google Core Update",
         "Search engine reshuffles rankings.",
         "SEO -30%", 1,0.7f,1,1,1,1, 0,2,false,0},
        {"Featured Snippet Goldmine",
         "New SERP features boost organic CTR.",
         "SEO +40%", 1,1.4f,1,1,1,1, 0,2,false,0},
        {"GDPR Fine Wave",
         "Mass email non-compliance fines. Lists shrink.",
         "Email -50%", 1,1,0.5f,1,1,1, -2000,1,false,0},
        {"Newsletter Renaissance",
         "Consumers prefer email over social. Open rates +30%.",
         "Email +30%", 1,1,1.3f,1,1,1, 0,3,false,0},
        {"Influencer Scandal",
         "Top influencer caught in controversy.",
         "Influencer -60%", 1,1,1,0.4f,1,1, -1000,2,false,0},
        {"Micro-Influencer Wave",
         "Brands shift to micro-influencers with higher trust.",
         "Influencer +70%", 1,1,1,1.7f,1,1, 0,3,false,0},
        {"Industry Award Season",
         "PR visibility peaks during awards.",
         "PR +40%", 1,1,1,1,1.4f,1, 0,1,false,0},
        {"Media Crisis",
         "Negative press cycle reduces PR effectiveness.",
         "PR -40%", 1,1,1,1,0.6f,1, 0,2,false,0},
        {"Recession Warning",
         "Clients cut ad budgets by 20%.",
         "All -20%", 0.8f,0.8f,0.8f,0.8f,0.8f,0.8f, -3000,3,false,0},
        {"Economic Boom",
         "Businesses increase marketing spending.",
         "All +25%", 1.25f,1.25f,1.25f,1.25f,1.25f,1.25f, 2000,2,false,0},
        {"VC Funding Wave",
         "Tech startups flush with cash seek agencies.",
         "+$5000", 1,1,1,1,1,1, 5000,1,false,0},
        {"Rival Agency Scandal",
         "Competitor caught faking metrics.",
         "+clients available", 1,1,1,1,1,1, 1000,1,false,0},
        {"New Competitor Entry",
         "Well-funded agency enters market.",
         "Rivals aggressive", 0.9f,0.9f,0.9f,0.9f,0.9f,0.9f, 0,3,false,0},
        {"AI Ads Revolution",
         "AI-generated ads outperform manual. All efficiency +30%.",
         "All +30%", 1.3f,1.3f,1.3f,1.3f,1.3f,1.3f, 0,2,false,0},
        {"Cookie Apocalypse",
         "Third-party cookies deprecated. Targeting harder.",
         "Paid -50%, Email +20%", 1,1,1.2f,1,1,0.5f, 0,3,false,0},
        {"Streaming Ad Surge",
         "Connected TV ad placements show 2x ROI.",
         "Paid Search +40%", 1,1,1,1,1,1.4f, 0,2,false,0},
    };

    // Append live market events from JSON
    auto marketEvents = LoadMarketEventsFromJSON();
    all.insert(all.end(), marketEvents.begin(), marketEvents.end());

    return all;
}

void EventSystem::Init(GameState& /*gs*/) {
    srand(static_cast<unsigned>(time(nullptr)));
}

void EventSystem::TryTriggerEvent(GameState& gs) {
    // 1. Fire market-driven events from live MarketState
    const MarketState& ms = MarketFeed::Get().GetState();
    if (!ms.isOffline) {
        auto triggered = MarketEventBridge::EvaluateTriggers(ms);
        auto allEvents = GetAllEvents();
        for (const auto& evId : triggered) {
            // Find matching event by lowercase title search
            for (auto& ev : allEvents) {
                std::string titleLower = ev.title;
                for (auto& c : titleLower) c = (char)tolower(c);
                if (titleLower.find(evId.substr(0, 6)) != std::string::npos) {
                    // Check not already active
                    bool alreadyActive = false;
                    for (const auto& ae : gs.activeEvents)
                        if (ae.title == ev.title) { alreadyActive = true; break; }
                    if (!alreadyActive) {
                        ev.active     = true;
                        ev.monthsLeft = ev.durationMonths;
                        gs.activeEvents.push_back(ev);
                    }
                    break;
                }
            }
        }
    }

    // 2. Random static event (30% chance per month — unchanged)
    if (rand() % 100 >= 30) return;
    auto all = GetAllEvents();
    auto ev  = all[rand() % static_cast<int>(all.size())];
    ev.active     = true;
    ev.monthsLeft = ev.durationMonths;
    gs.activeEvents.push_back(ev);
}
