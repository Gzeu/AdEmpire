#include "EventSystem.h"
#include <cstdlib>
#include <ctime>

std::vector<NewsEvent> EventSystem::GetAllEvents() {
    return {
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
}

void EventSystem::Init(GameState& /*gs*/) {
    srand(static_cast<unsigned>(time(nullptr)));
}

void EventSystem::TryTriggerEvent(GameState& gs) {
    if (rand() % 100 >= 30) return;
    auto all = GetAllEvents();
    auto ev  = all[rand() % static_cast<int>(all.size())];
    ev.active     = true;
    ev.monthsLeft = ev.durationMonths;
    gs.activeEvents.push_back(ev);
}
