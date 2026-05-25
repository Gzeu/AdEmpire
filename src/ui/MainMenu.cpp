#include "MainMenu.h"
#include "imgui.h"
#include "../systems/AICompetitor.h"
#include "../systems/EventSystem.h"
#include <cstring>

char MainMenu::s_agencyNameBuf[64] = "My Ad Agency";
bool MainMenu::s_showMenu          = true;

// All 30 clients from assets/data/clients.json
static std::vector<Client> BuildAllClients() {
    return {
        {1,  "TastyBite Foods",      ClientIndustry::Food,      3000,  70, 6,  false, true, {}, 0},
        {2,  "UrbanThread Co",       ClientIndustry::Fashion,   4500,  70, 6,  false, true, {}, 0},
        {3,  "SparkTech Ltd",        ClientIndustry::Tech,      8000,  70, 12, false, true, {}, 0},
        {4,  "WealthWise",           ClientIndustry::Finance,   12000, 70, 12, false, true, {}, 0},
        {5,  "VitalHealth Clinic",   ClientIndustry::Health,    5000,  70, 6,  false, true, {}, 0},
        {6,  "EduPath Academy",      ClientIndustry::Education, 3500,  70, 6,  false, true, {}, 0},
        {7,  "GreenGrove Organic",   ClientIndustry::Food,      2500,  70, 6,  false, true, {}, 0},
        {8,  "LuxeLook Cosmetics",   ClientIndustry::Fashion,   7000,  70, 6,  false, true, {}, 0},
        {9,  "CloudStack Inc",       ClientIndustry::Tech,      15000, 70, 12, false, true, {}, 0},
        {10, "AutoDrive Motors",     ClientIndustry::Retail,    20000, 70, 12, false, true, {}, 0},
        {11, "FreshPress Juices",    ClientIndustry::Food,      1800,  70, 6,  false, true, {}, 0},
        {12, "PixelForge Games",     ClientIndustry::Gaming,    9000,  70, 6,  false, true, {}, 0},
        {13, "BlueSky Airlines",     ClientIndustry::Retail,    25000, 70, 12, false, true, {}, 0},
        {14, "NovaCure Pharma",      ClientIndustry::Health,    18000, 70, 12, false, true, {}, 0},
        {15, "ZenMind Wellness",     ClientIndustry::Health,    4000,  70, 6,  false, true, {}, 0},
        {16, "CryptoVault Exchange", ClientIndustry::Finance,   30000, 70, 12, false, true, {}, 0},
        {17, "TrendBurst Apparel",   ClientIndustry::Fashion,   6000,  70, 6,  false, true, {}, 0},
        {18, "EcoNest Homes",        ClientIndustry::Retail,    8500,  70, 6,  false, true, {}, 0},
        {19, "BrainWave EdTech",     ClientIndustry::Education, 5500,  70, 6,  false, true, {}, 0},
        {20, "RoboArm Industrial",   ClientIndustry::Tech,      22000, 70, 12, false, true, {}, 0},
        {21, "SunRise Coffee",       ClientIndustry::Food,      3200,  70, 6,  false, true, {}, 0},
        {22, "DataMind Analytics",   ClientIndustry::Tech,      11000, 70, 12, false, true, {}, 0},
        {23, "ActiveLife Sports",    ClientIndustry::Retail,    7500,  70, 6,  false, true, {}, 0},
        {24, "NatureGlow Skincare",  ClientIndustry::Health,    5800,  70, 6,  false, true, {}, 0},
        {25, "MetaVerse Studio",     ClientIndustry::Gaming,    14000, 70, 12, false, true, {}, 0},
        {26, "PocketBank",           ClientIndustry::Finance,   16000, 70, 12, false, true, {}, 0},
        {27, "UrbanEats Delivery",   ClientIndustry::Food,      6500,  70, 6,  false, true, {}, 0},
        {28, "SmartHome Devices",    ClientIndustry::Tech,      9500,  70, 6,  false, true, {}, 0},
        {29, "LegalEase Platform",   ClientIndustry::Tech,      7200,  70, 6,  false, true, {}, 0},
        {30, "GlobeTravel Agency",   ClientIndustry::Retail,    13000, 70, 12, false, true, {}, 0},
    };
}

bool MainMenu::Render(GameState& gs) {
    if (!s_showMenu) return false;
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(480, 380));
    ImGui::SetNextWindowBgAlpha(0.97f);
    ImGui::Begin("##MainMenu", nullptr,
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);

    ImGui::SetCursorPosX(100);
    ImGui::SetWindowFontScale(2.2f);
    ImGui::TextColored(ImVec4(0.3f,0.7f,1.f,1.f), "AdEmpire");
    ImGui::SetWindowFontScale(1.0f);
    ImGui::TextColored(ImVec4(0.6f,0.8f,1.f,0.8f), "          Marketing Tycoon Simulator");
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Agency Name:");
    ImGui::SetNextItemWidth(-1);
    ImGui::InputText("##agname", s_agencyNameBuf, 64);
    ImGui::Spacing();
    ImGui::TextColored(ImVec4(0.5f,0.9f,0.5f,1.f), "Starting budget: $10,000");
    ImGui::TextColored(ImVec4(0.8f,0.6f,0.2f,1.f), "Goal: Reach 35%% market share");
    ImGui::TextColored(ImVec4(0.7f,0.7f,0.7f,1.f), "30 clients available to pitch");
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    float bw = 440;
    if (ImGui::Button("  New Game  ", ImVec2(bw, 44))) {
        gs             = GameState(); // reset
        gs.agencyName  = std::string(s_agencyNameBuf);
        gs.clients     = BuildAllClients();
        gs.nextClientId = 31;
        AICompetitor::Init(gs);
        EventSystem::Init(gs);
        s_showMenu = false;
        return true;
    }
    ImGui::Spacing();
    if (ImGui::Button("     Quit     ", ImVec2(bw, 36)))
        exit(0);
    ImGui::End();
    return false;
}
