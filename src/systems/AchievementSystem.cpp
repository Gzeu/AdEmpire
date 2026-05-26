#include "AchievementSystem.h"
#include "../ui/ToastSystem.h"
#include <vector>
#include <functional>

struct AchievDef {
    std::string id, name, description;
    std::function<bool(const GameState&)> condition;
};

static std::vector<AchievDef> s_defs;

void AchievementSystem::Init(GameState& gs) {
    s_defs = {
        {"first_blood",   "First Client",       "Win your first client",
            [](const GameState& g){ return g.stats.clientsAcquired >= 1; }},
        {"three_clients", "Growing Roster",     "Have 3 active clients",
            [](const GameState& g){
                int n=0; for(auto& c:g.clients) if(c.active) n++; return n>=3; }},
        {"five_clients",  "Full House",         "Have 5 active clients simultaneously",
            [](const GameState& g){
                int n=0; for(auto& c:g.clients) if(c.active) n++; return n>=5; }},
        {"first_campaign","Campaign Launched",  "Launch your first campaign",
            [](const GameState& g){ return !g.campaigns.empty(); }},
        {"ten_campaigns", "Ad Machine",         "Complete 10 campaigns",
            [](const GameState& g){ return g.stats.campaignsCompleted >= 10; }},
        {"first_hire",    "Growing Team",       "Hire your first staff member",
            [](const GameState& g){ return !g.staff.empty(); }},
        {"full_team",     "Dream Team",         "Have 6 staff members",
            [](const GameState& g){ return g.staff.size() >= 6; }},
        {"5pct_share",    "Rising Agency",      "Reach 5% market share",
            [](const GameState& g){ return g.playerMarketShare >= 5.f; }},
        {"10pct_share",   "Market Contender",   "Reach 10% market share",
            [](const GameState& g){ return g.playerMarketShare >= 10.f; }},
        {"20pct_share",   "Major Player",       "Reach 20% market share",
            [](const GameState& g){ return g.playerMarketShare >= 20.f; }},
        {"100k_revenue",  "Six Figures",        "Earn $100,000 total revenue",
            [](const GameState& g){ return g.stats.totalRevenue >= 100000.f; }},
        {"500k_revenue",  "Half Million Club",  "Earn $500,000 total revenue",
            [](const GameState& g){ return g.stats.totalRevenue >= 500000.f; }},
        {"12_months",     "One Year Strong",    "Survive 12 months",
            [](const GameState& g){ return g.stats.monthsPlayed >= 12; }},
        {"rep_80",        "Trusted Agency",     "Reach 80 reputation",
            [](const GameState& g){ return g.stats.reputation >= 80.f; }},
        {"all_channels",  "Omnichannel",        "Have an active campaign on all 6 channels",
            [](const GameState& g){
                bool ch[6]={};
                for(auto& c:g.campaigns) if(c.active) ch[(int)c.channel]=true;
                for(int i=0;i<6;i++) if(!ch[i]) return false;
                return true; }},
    };

    gs.achievements.clear();
    for (auto& d : s_defs) {
        Achievement a;
        try {
            a.id = std::stoi(d.id);
        } catch (...) {
            a.id = 0; // fallback if id is not a valid number
        }
        a.title = d.name;
        a.description = d.description;
        a.category = "";
        a.unlocked = false;
        a.condition = d.condition;
        gs.achievements.push_back(a);
    }
}

void AchievementSystem::Check(GameState& gs) {
    for (int i = 0; i < (int)gs.achievements.size(); i++) {
        auto& a = gs.achievements[i];
        if (a.unlocked) continue;
        if (i < (int)s_defs.size() && s_defs[i].condition(gs)) {
            a.unlocked = true;
            TOAST_SUCCESS("Achievement: " + a.title);
        }
    }
}
