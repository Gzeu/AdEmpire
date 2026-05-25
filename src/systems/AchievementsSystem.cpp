#include "AchievementsSystem.h"
#include "imgui.h"
#include <algorithm>

std::vector<Achievement>      AchievementsSystem::s_achievements;
std::vector<AchievementPopup> AchievementsSystem::s_popups;

void AchievementsSystem::Init() {
    s_achievements = {
        // ─ REVENUE (1-5)
        {1,  "First Dollar",        "Earn your first $1 in agency fees",         "Revenue",    false, [](const GameState& g){ return g.stats.totalRevenue >= 1.f; }},
        {2,  "Small Agency",        "Earn $10,000 total revenue",                "Revenue",    false, [](const GameState& g){ return g.stats.totalRevenue >= 10000.f; }},
        {3,  "Six Figures",         "Earn $100,000 total revenue",               "Revenue",    false, [](const GameState& g){ return g.stats.totalRevenue >= 100000.f; }},
        {4,  "Half Million Club",   "Earn $500,000 total revenue",               "Revenue",    false, [](const GameState& g){ return g.stats.totalRevenue >= 500000.f; }},
        {5,  "Million Dollar Agency","Earn $1,000,000 total revenue",            "Revenue",    false, [](const GameState& g){ return g.stats.totalRevenue >= 1000000.f; }},
        // ─ CLIENTS (6-9)
        {6,  "First Client",        "Sign your first client",                    "Clients",    false, [](const GameState& g){ return g.stats.clientsAcquired >= 1; }},
        {7,  "Agency of 5",         "Have 5 active clients simultaneously",      "Clients",    false, [](const GameState& g){
            int n=0; for(auto& c:g.clients) if(c.active) n++; return n>=5; }},
        {8,  "Portfolio Builder",   "Acquire 10 clients total",                  "Clients",    false, [](const GameState& g){ return g.stats.clientsAcquired >= 10; }},
        {9,  "Client Whisperer",    "Acquire 20 clients total",                  "Clients",    false, [](const GameState& g){ return g.stats.clientsAcquired >= 20; }},
        // ─ CAMPAIGNS (10-13)
        {10, "First Campaign",      "Launch your first campaign",                "Campaigns",  false, [](const GameState& g){ return g.stats.campaignsCompleted >= 1; }},
        {11, "Campaign Machine",    "Complete 10 campaigns",                     "Campaigns",  false, [](const GameState& g){ return g.stats.campaignsCompleted >= 10; }},
        {12, "Multi-Channel",       "Run campaigns on 3+ different channels",    "Campaigns",  false, [](const GameState& g){
            bool ch[6]={}; for(auto& c:g.campaigns) if(c.completed) ch[(int)c.channel]=true;
            int n=0; for(int i=0;i<6;i++) if(ch[i]) n++; return n>=3; }},
        {13, "Campaign Veteran",    "Complete 50 campaigns",                     "Campaigns",  false, [](const GameState& g){ return g.stats.campaignsCompleted >= 50; }},
        // ─ STAFF (14-16)
        {14, "First Hire",          "Hire your first staff member",              "Staff",      false, [](const GameState& g){ return g.staff.size() >= 1; }},
        {15, "Dream Team",          "Have 5 staff members",                      "Staff",      false, [](const GameState& g){ return g.staff.size() >= 5; }},
        {16, "Full Roster",         "Have one of each staff role",               "Staff",      false, [](const GameState& g){
            bool r[6]={}; for(auto& s:g.staff) r[(int)s.role]=true;
            for(int i=0;i<6;i++) if(!r[i]) return false; return true; }},
        // ─ MARKET (17-19)
        {17, "Market Entry",        "Reach 5% market share",                     "Market",     false, [](const GameState& g){ return g.playerMarketShare >= 5.f; }},
        {18, "Market Challenger",   "Reach 15% market share",                    "Market",     false, [](const GameState& g){ return g.playerMarketShare >= 15.f; }},
        {19, "Market Leader",       "Reach 35% market share — VICTORY!",         "Market",     false, [](const GameState& g){ return g.playerMarketShare >= 35.f; }},
        // ─ SURVIVAL (20-21)
        {20, "Year One",            "Survive 12 months",                         "Survival",   false, [](const GameState& g){ return g.stats.monthsPlayed >= 12; }},
        {21, "Veteran Agency",      "Survive 36 months",                         "Survival",   false, [](const GameState& g){ return g.stats.monthsPlayed >= 36; }},
        // ─ REPUTATION (22)
        {22, "Legendary",           "Reach 90+ reputation",                      "Reputation", false, [](const GameState& g){ return g.stats.reputation >= 90.f; }},
    };
}

void AchievementsSystem::CheckAll(const GameState& gs) {
    for (auto& a : s_achievements) {
        if (!a.unlocked && a.condition(gs)) {
            a.unlocked = true;
            s_popups.push_back({a.title, 3.5f});
        }
    }
}

void AchievementsSystem::RenderPopups() {
    float dt = ImGui::GetIO().DeltaTime;
    ImVec2 vp = ImGui::GetMainViewport()->Size;
    float y = vp.y - 80.f;
    for (auto& p : s_popups) {
        p.timer -= dt;
        float alpha = std::min(p.timer / 0.5f, 1.f);
        ImGui::SetNextWindowPos(ImVec2(vp.x - 320.f, y), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(300, 54));
        ImGui::SetNextWindowBgAlpha(alpha * 0.92f);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.5f, 0.15f, 1.f));
        char wid[32]; snprintf(wid, 32, "##popup%d", (int)(y));
        ImGui::Begin(wid, nullptr,
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs |
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);
        ImGui::Text("\xF0\x9F\x8F\x86 Achievement Unlocked!");
        ImGui::TextColored(ImVec4(1.f,1.f,0.4f,alpha), "%s", p.title.c_str());
        ImGui::End();
        ImGui::PopStyleColor();
        y -= 60.f;
    }
    s_popups.erase(
        std::remove_if(s_popups.begin(), s_popups.end(),
            [](const AchievementPopup& p){ return p.timer <= 0.f; }),
        s_popups.end());
}

void AchievementsSystem::Render(GameState& gs) {
    if (!gs.showSettings) return; // reuse flag — handled in main
}
