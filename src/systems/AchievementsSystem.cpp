#include "AchievementsSystem.h"

bool        AchievementsSystem::s_showPopup  = false;
std::string AchievementsSystem::s_popupText  = "";
float       AchievementsSystem::s_popupTimer = 0.f;

static std::vector<Achievement> s_achievements;

void AchievementsSystem::Init() {
    s_achievements = {
        // Revenue
        {"first_dollar",   "First Dollar",      "Earn your first agency fee",              "💰", false, 0},
        {"rev_10k",        "$10K Agency",        "Accumulate $10,000 total revenue",        "📈", false, 0},
        {"rev_100k",       "$100K Club",         "Accumulate $100,000 total revenue",       "🏆", false, 0},
        {"rev_1m",         "Millionaire Agency", "Accumulate $1,000,000 total revenue",     "🚀", false, 0},
        {"best_month_5k",  "Hot Month",          "Earn $5,000 in a single month",           "🔥", false, 0},
        {"best_month_20k", "On Fire",            "Earn $20,000 in a single month",          "⚡", false, 0},
        // Clients
        {"first_client",   "First Client",       "Sign your first client",                  "🤝", false, 0},
        {"5_clients",      "Growing Agency",     "Have 5 active clients simultaneously",    "👥", false, 0},
        {"10_clients",     "Powerhouse",         "Have 10 active clients simultaneously",   "🌟", false, 0},
        {"pitch_master",   "Pitch Master",       "Successfully pitch 15 clients total",     "🎯", false, 0},
        // Campaigns
        {"5_camps",        "Campaign Veteran",   "Complete 5 campaigns",                    "📋", false, 0},
        {"20_camps",       "Campaign Machine",   "Complete 20 campaigns",                   "⚙️",  false, 0},
        {"all_channels",   "Omnichannel",        "Run a campaign on every channel type",    "📡", false, 0},
        // Staff
        {"first_hire",     "First Employee",     "Hire your first staff member",            "👔", false, 0},
        {"full_team",      "Full Team",          "Have at least one of every staff role",   "🏢", false, 0},
        // Market
        {"share_10",       "Market Contender",   "Reach 10%% market share",                 "📊", false, 0},
        {"share_25",       "Market Leader",      "Reach 25%% market share",                 "👑", false, 0},
        {"share_35",       "Industry Titan",     "Reach 35%% market share — YOU WIN!",      "🏆", false, 0},
        // Survival
        {"survive_12",     "Year One",           "Survive 12 months without going bankrupt","🎂", false, 0},
        {"survive_24",     "Veteran Agency",     "Survive 24 months",                       "🏅", false, 0},
        // Rep
        {"rep_50",         "Reputable",          "Reach 50 reputation",                     "⭐", false, 0},
        {"rep_90",         "Legendary",          "Reach 90 reputation",                     "🌠", false, 0},
    };
}

static void Unlock(const std::string& id, const GameState& gs) {
    for (auto& a : s_achievements) {
        if (a.id == id && !a.unlocked) {
            a.unlocked      = true;
            a.monthUnlocked = gs.stats.monthsPlayed;
            AchievementsSystem::s_showPopup  = true;
            AchievementsSystem::s_popupText  = a.icon + " Achievement: " + a.title;
            AchievementsSystem::s_popupTimer = 3.0f;
        }
    }
}

void AchievementsSystem::Check(const GameState& gs) {
    int activeClients = 0;
    for (auto& c : gs.clients) if (c.active) activeClients++;

    // Revenue
    if (gs.stats.totalRevenue > 0)         Unlock("first_dollar",   gs);
    if (gs.stats.totalRevenue >= 10000)     Unlock("rev_10k",        gs);
    if (gs.stats.totalRevenue >= 100000)    Unlock("rev_100k",       gs);
    if (gs.stats.totalRevenue >= 1000000)   Unlock("rev_1m",         gs);
    if (gs.stats.bestMonthRevenue >= 5000)  Unlock("best_month_5k",  gs);
    if (gs.stats.bestMonthRevenue >= 20000) Unlock("best_month_20k", gs);
    // Clients
    if (gs.stats.clientsAcquired >= 1)      Unlock("first_client",   gs);
    if (activeClients >= 5)                 Unlock("5_clients",      gs);
    if (activeClients >= 10)                Unlock("10_clients",     gs);
    if (gs.stats.clientsAcquired >= 15)     Unlock("pitch_master",   gs);
    // Campaigns
    if (gs.stats.campaignsCompleted >= 5)   Unlock("5_camps",        gs);
    if (gs.stats.campaignsCompleted >= 20)  Unlock("20_camps",       gs);
    // Staff
    if (!gs.staff.empty())                  Unlock("first_hire",     gs);
    // Check full team (one of each role)
    bool roles[6] = {};
    for (auto& s : gs.staff) roles[(int)s.role] = true;
    bool fullTeam = true;
    for (int i = 0; i < 6; i++) if (!roles[i]) { fullTeam = false; break; }
    if (fullTeam)                           Unlock("full_team",      gs);
    // Market share
    if (gs.playerMarketShare >= 10.f)       Unlock("share_10",       gs);
    if (gs.playerMarketShare >= 25.f)       Unlock("share_25",       gs);
    if (gs.playerMarketShare >= 35.f)       Unlock("share_35",       gs);
    // Survival
    if (gs.stats.monthsPlayed >= 12)        Unlock("survive_12",     gs);
    if (gs.stats.monthsPlayed >= 24)        Unlock("survive_24",     gs);
    // Reputation
    if (gs.stats.reputation >= 50.f)        Unlock("rep_50",         gs);
    if (gs.stats.reputation >= 90.f)        Unlock("rep_90",         gs);
    // All channels used
    bool usedChannels[6] = {};
    for (auto& c : gs.campaigns) usedChannels[(int)c.channel] = true;
    bool allCh = true;
    for (int i = 0; i < 6; i++) if (!usedChannels[i]) { allCh = false; break; }
    if (allCh)                              Unlock("all_channels",   gs);
}

const std::vector<Achievement>& AchievementsSystem::GetAll() { return s_achievements; }
int AchievementsSystem::GetUnlockedCount() {
    int n = 0;
    for (auto& a : s_achievements) if (a.unlocked) n++;
    return n;
}
