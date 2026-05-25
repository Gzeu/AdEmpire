#include "EventSystem.h"
#include <cstdlib>
#include <ctime>

std::vector<NewsEvent> EventSystem::GetAllEvents() {
    return {
        // ─ SOCIAL MEDIA EVENTS ─
        {"Algorithm Update", "A major social platform changed its algorithm. Organic reach drops 40%.",
         "Social campaigns -40%", 0.6f,1,1,1,1,1, 0, 2, false, 0},
        {"Viral Trend Surge", "A viral challenge boosts social media engagement across all brands.",
         "Social campaigns +60%", 1.6f,1,1,1.3f,1,1, 0, 1, false, 0},
        {"Platform Outage", "Major social platform has 24-hour outage. Campaigns paused.",
         "Social campaigns -80%", 0.2f,1,1,1,1,1, -500, 1, false, 0},
        {"TikTok Boom", "Short-form video content sees 3x engagement spike.",
         "Social + Influencer +50%", 1.5f,1,1,1.5f,1,1, 0, 2, false, 0},
        // ─ SEO EVENTS ─
        {"Google Core Update", "Search engine major update reshuffles rankings.",
         "SEO campaigns -30% for 2 months", 1,0.7f,1,1,1,1, 0, 2, false, 0},
        {"Featured Snippet Goldmine", "New SERP features boost organic CTR.",
         "SEO +40%", 1,1.4f,1,1,1,1, 0, 2, false, 0},
        // ─ EMAIL EVENTS ─
        {"GDPR Fine Wave", "Mass email non-compliance fines. Email lists shrink.",
         "Email campaigns -50%", 1,1,0.5f,1,1,1, -2000, 1, false, 0},
        {"Newsletter Renaissance", "Consumers prefer email over social. Email open rates +30%.",
         "Email +30%", 1,1,1.3f,1,1,1, 0, 3, false, 0},
        // ─ INFLUENCER EVENTS ─
        {"Influencer Scandal", "Top influencer caught in controversy. Brands pull back.",
         "Influencer campaigns -60%", 1,1,1,0.4f,1,1, -1000, 2, false, 0},
        {"Micro-Influencer Wave", "Brands shift to micro-influencers with higher trust.",
         "Influencer +70%", 1,1,1,1.7f,1,1, 0, 3, false, 0},
        // ─ PR EVENTS ─
        {"Industry Award Season", "PR visibility peaks during awards. Coverage +40%.",
         "PR +40%", 1,1,1,1,1.4f,1, 0, 1, false, 0},
        {"Media Crisis", "Negative press cycle. PR campaigns less effective.",
         "PR -40%", 1,1,1,1,0.6f,1, 0, 2, false, 0},
        // ─ ECONOMY EVENTS ─
        {"Recession Warning", "Clients cut ad budgets by 20%.",
         "All channels -20%, budget impact", 0.8f,0.8f,0.8f,0.8f,0.8f,0.8f, -3000, 3, false, 0},
        {"Economic Boom", "Businesses increase marketing spending. All channels +25%.",
         "All channels +25%", 1.25f,1.25f,1.25f,1.25f,1.25f,1.25f, 2000, 2, false, 0},
        {"VC Funding Wave", "Tech startups flush with cash seek agencies.",
         "+$5000 bonus", 1,1,1,1,1,1, 5000, 1, false, 0},
        // ─ COMPETITION EVENTS ─
        {"Rival Agency Scandal", "Competitor caught faking metrics. Clients seek alternatives.",
         "+3 available clients", 1,1,1,1,1,1, 1000, 1, false, 0},
        {"New Competitor Entry", "A well-funded agency enters the market.",
         "AI rivals +aggressiveness", 0.9f,0.9f,0.9f,0.9f,0.9f,0.9f, 0, 3, false, 0},
        // ─ TECHNOLOGY EVENTS ─
        {"AI Ads Revolution", "AI-generated ads outperform manual. All channel efficiency +30%.",
         "All channels +30%", 1.3f,1.3f,1.3f,1.3f,1.3f,1.3f, 0, 2, false, 0},
        {"Cookie Apocalypse", "Third-party cookies fully deprecated. Targeting gets harder.",
         "Paid Search -50%, Email +20%", 1,1,1.2f,1,1,0.5f, 0, 3, false, 0},
        {"Streaming Ad Surge", "Connected TV ad placements show 2x ROI.",
         "Paid Search +40%", 1,1,1,1,1,1.4f, 0, 2, false, 0},
    };
}

void EventSystem::Init(GameState& gs) {
    srand((unsigned)time(nullptr));
}

void EventSystem::TryTriggerEvent(GameState& gs) {
    // 30% chance each month
    if (rand() % 100 >= 30) return;
    auto all = GetAllEvents();
    int idx = rand() % (int)all.size();
    auto ev = all[idx];
    ev.active = true;
    ev.monthsLeft = ev.durationMonths;
    gs.activeEvents.push_back(ev);
}
