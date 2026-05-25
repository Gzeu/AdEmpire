#include "AchievementSystem.h"
#include "../ui/ToastSystem.h"

void AchievementSystem::Init() {
    achievements = {
        {1,  "First Client",       "Sign your first client",                "\xF0\x9F\xA4\x9D", false, 0,
            [](const GameState& g){ return g.stats.clientsAcquired >= 1; }},
        {2,  "Campaign Runner",    "Complete your first campaign",           "\xF0\x9F\x9A\x80", false, 0,
            [](const GameState& g){ return g.stats.campaignsCompleted >= 1; }},
        {3,  "Five Clients",       "Have 5 active clients simultaneously",   "\xF0\x9F\x8F\xA2", false, 0,
            [](const GameState& g){
                int n=0; for(auto& c:g.clients) if(c.active) n++;
                return n >= 5; }},
        {4,  "Ten Campaigns",      "Complete 10 campaigns",                  "\xF0\x9F\x93\x88", false, 0,
            [](const GameState& g){ return g.stats.campaignsCompleted >= 10; }},
        {5,  "First $10K Month",   "Earn $10,000 in a single month",         "\xF0\x9F\x92\xB0", false, 0,
            [](const GameState& g){ return g.stats.bestMonthRevenue >= 10000.f; }},
        {6,  "Market Challenger",  "Reach 10% market share",                 "\xF0\x9F\x8F\x86", false, 0,
            [](const GameState& g){ return g.playerMarketShare >= 10.f; }},
        {7,  "Market Leader",      "Reach 25% market share",                 "\xF0\x9F\x91\x91", false, 0,
            [](const GameState& g){ return g.playerMarketShare >= 25.f; }},
        {8,  "Full Team",          "Have 5 staff members",                   "\xF0\x9F\x91\xA5", false, 0,
            [](const GameState& g){ return g.staff.size() >= 5; }},
        {9,  "Reputation Pro",     "Reach 75 reputation",                    "\xE2\xAD\x90", false, 0,
            [](const GameState& g){ return g.stats.reputation >= 75.f; }},
        {10, "Survivor",           "Survive 12 months",                      "\xF0\x9F\x8F\x85", false, 0,
            [](const GameState& g){ return g.stats.monthsPlayed >= 12; }},
        {11, "Crisis Manager",     "Survive a Recession event",              "\xF0\x9F\x9B\xA1\xEF\xB8\x8F", false, 0,
            [](const GameState& g){
                for(auto& e:g.activeEvents)
                    if(e.title=="Recession Warning" && g.budget > 0) return true;
                return false; }},
        {12, "Viral Hit",          "Have an Influencer campaign with >100K reach","\xF0\x9F\x94\xA5", false, 0,
            [](const GameState& g){
                for(auto& c:g.campaigns)
                    if(c.channel==ChannelType::Influencer && c.reach>100000.f) return true;
                return false; }},
        {13, "$100K Revenue",      "Accumulate $100,000 total revenue",      "\xF0\x9F\x92\x8E", false, 0,
            [](const GameState& g){ return g.stats.totalRevenue >= 100000.f; }},
        {14, "Industry Veteran",   "Have clients in 5 different industries", "\xF0\x9F\x8C\x90", false, 0,
            [](const GameState& g){
                std::vector<int> inds;
                for(auto& c:g.clients)
                    if(c.active){
                        int ind=(int)c.industry;
                        bool found=false; for(int i:inds) if(i==ind) found=true;
                        if(!found) inds.push_back(ind);
                    }
                return (int)inds.size() >= 5; }},
        {15, "Empire Builder",     "Win the game (35% market share)",        "\xF0\x9F\x8F\x9B\xEF\xB8\x8F", false, 0,
            [](const GameState& g){ return g.victory; }},
    };
}

void AchievementSystem::Tick(const GameState& gs) {
    for (auto& a : achievements) {
        if (!a.unlocked && a.condition && a.condition(gs)) {
            a.unlocked = true;
            a.unlockedMonth = gs.stats.monthsPlayed;
            recentlyUnlocked.push_back(a.title);
            ToastSystem::Get().Push(
                "\xF0\x9F\x8F\x86 Achievement: " + a.title,
                ToastType::Success, 5.f);
        }
    }
    recentlyUnlocked.clear();
}

int AchievementSystem::CountUnlocked() const {
    int n = 0;
    for (auto& a : achievements) if (a.unlocked) n++;
    return n;
}

void AchievementSystem::Render(GameState& gs) {
    if (!gs.showSettings) return; // reuse settings flag — handled separately
    // Rendered inline in SettingsPanel
}
