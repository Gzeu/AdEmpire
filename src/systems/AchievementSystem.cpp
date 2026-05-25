#include "AchievementSystem.h"
#include "../ui/ToastSystem.h"
#include <algorithm>

static std::vector<Achievement> s_achievements;

void AchievementSystem::Init() {
    s_achievements = {
        {"first_client",     "First Contract",      "Win your first client.",                  "🤝", false, -1,
            [](const GameState& gs){ return gs.stats.clientsAcquired >= 1; }},
        {"five_clients",     "Popular Agency",      "Have 5 active clients simultaneously.",   "⭐", false, -1,
            [](const GameState& gs){
                int c=0; for(auto& cl:gs.clients) if(cl.active) c++; return c>=5; }},
        {"ten_clients",      "Power Player",        "Have 10 active clients simultaneously.",  "🏆", false, -1,
            [](const GameState& gs){
                int c=0; for(auto& cl:gs.clients) if(cl.active) c++; return c>=10; }},
        {"first_campaign",   "Campaign Manager",    "Launch your first campaign.",             "🚀", false, -1,
            [](const GameState& gs){ return gs.stats.campaignsCompleted >= 1; }},
        {"ten_campaigns",    "Campaign Veteran",    "Complete 10 campaigns.",                  "📈", false, -1,
            [](const GameState& gs){ return gs.stats.campaignsCompleted >= 10; }},
        {"budget_50k",       "Flush With Cash",     "Reach $50,000 budget.",                   "💰", false, -1,
            [](const GameState& gs){ return gs.budget >= 50000.f; }},
        {"budget_100k",      "Six Figures",         "Reach $100,000 budget.",                  "🤑", false, -1,
            [](const GameState& gs){ return gs.budget >= 100000.f; }},
        {"market_10",        "On the Map",          "Reach 10% market share.",                 "🗺️", false, -1,
            [](const GameState& gs){ return gs.playerMarketShare >= 10.f; }},
        {"market_20",        "Major Player",        "Reach 20% market share.",                 "📅", false, -1,
            [](const GameState& gs){ return gs.playerMarketShare >= 20.f; }},
        {"market_35",        "Market Leader",       "Reach 35% market share — you win!",       "🌟", false, -1,
            [](const GameState& gs){ return gs.playerMarketShare >= 35.f; }},
        {"rep_80",           "Trusted Brand",       "Reach 80% reputation.",                   "💯", false, -1,
            [](const GameState& gs){ return gs.stats.reputation >= 80.f; }},
        {"first_staff",      "Growing Team",        "Hire your first staff member.",            "👥", false, -1,
            [](const GameState& gs){ return gs.staff.size() >= 1; }},
        {"five_staff",       "Full Agency",         "Have 5 staff members.",                   "🏢", false, -1,
            [](const GameState& gs){ return gs.staff.size() >= 5; }},
        {"survive_12",       "Year One",            "Survive 12 months.",                      "📆", false, -1,
            [](const GameState& gs){ return gs.stats.monthsPlayed >= 12; }},
        {"best_month_5k",    "Best Month",          "Earn $5,000 in a single month.",           "📉", false, -1,
            [](const GameState& gs){ return gs.stats.bestMonthRevenue >= 5000.f; }},
    };
}

void AchievementSystem::Check(GameState& gs) {
    for (auto& a : s_achievements) {
        if (!a.unlocked && a.condition && a.condition(gs)) {
            a.unlocked      = true;
            a.monthUnlocked = gs.stats.monthsPlayed;
            char msg[128];
            snprintf(msg, 128, "%s Achievement Unlocked: %s",
                a.icon.c_str(), a.title.c_str());
            ToastSystem::Push(msg, ToastType::Success);
        }
    }
}

std::vector<Achievement>& AchievementSystem::All() { return s_achievements; }
int AchievementSystem::UnlockedCount() {
    return (int)std::count_if(s_achievements.begin(), s_achievements.end(),
        [](const Achievement& a){ return a.unlocked; });
}
