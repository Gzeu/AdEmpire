#include "MainMenu.h"
#include "Newsfeed.h"
#include "imgui.h"
#include "../systems/AICompetitor.h"
#include "../systems/EventSystem.h"
#include <cstring>

char MainMenu::s_agencyNameBuf[64] = "My Ad Agency";
bool MainMenu::s_showMenu = true;

bool MainMenu::Render(GameState& gs) {
    if (!s_showMenu) return false;
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(500, 420));
    ImGui::SetNextWindowBgAlpha(0.97f);
    ImGui::Begin("##MainMenu", nullptr,
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar);

    // Title
    float titleW = ImGui::CalcTextSize("AdEmpire").x * 2.4f;
    ImGui::SetCursorPosX((500.f - titleW) * 0.5f);
    ImGui::SetWindowFontScale(2.4f);
    ImGui::TextColored(ImVec4(0.3f,0.75f,1.f,1.f), "AdEmpire");
    ImGui::SetWindowFontScale(1.0f);
    ImGui::SetCursorPosX(80.f);
    ImGui::TextColored(ImVec4(0.6f,0.85f,1.f,0.85f),
        "Marketing Tycoon Simulator");
    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    ImGui::Text("Agency Name:");
    ImGui::SetNextItemWidth(460);
    ImGui::InputText("##agname", s_agencyNameBuf, 64);
    ImGui::Spacing();

    ImGui::TextColored(ImVec4(0.5f,0.95f,0.5f,1.f), "  Starting Budget: $10,000");
    ImGui::TextColored(ImVec4(1.f,0.8f,0.2f,1.f),   "  Victory Goal: 35%% Market Share");
    ImGui::TextColored(ImVec4(1.f,0.4f,0.4f,1.f),   "  Bankruptcy: budget < -$50,000");
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Button("   New Game   ", ImVec2(460, 48))) {
        gs = GameState();
        gs.agencyName = std::string(s_agencyNameBuf);
        gs.budget     = 10000.f;
        AICompetitor::Init(gs);
        EventSystem::Init(gs);
        // Initial available clients
        gs.clients = {
            {1,"TastyBite Foods",    ClientIndustry::Food,      3000,  70, 6,  false, true, {}, 0},
            {2,"UrbanThread Co",     ClientIndustry::Fashion,   4500,  70, 6,  false, true, {}, 0},
            {3,"SparkTech Ltd",      ClientIndustry::Tech,      8000,  70, 12, false, true, {}, 0},
            {4,"WealthWise",         ClientIndustry::Finance,   12000, 70, 12, false, true, {}, 0},
            {5,"VitalHealth Clinic", ClientIndustry::Health,    5000,  70, 6,  false, true, {}, 0},
            {6,"EduPath Academy",    ClientIndustry::Education, 3500,  70, 6,  false, true, {}, 0},
            {7,"GreenGrove Organic", ClientIndustry::Food,      2500,  70, 6,  false, true, {}, 0},
            {8,"LuxeLook Cosmetics", ClientIndustry::Fashion,   7000,  70, 9,  false, true, {}, 0},
            {9,"PixelForge Games",   ClientIndustry::Gaming,    9000,  70, 12, false, true, {}, 0},
        };
        gs.nextClientId = 10;
        Newsfeed::s_entries.clear();
        Newsfeed::PushNews("Welcome to " + gs.agencyName + "! Start by pitching clients.",
            0.4f, 1.f, 0.6f);
        s_showMenu = false;
        return true;
    }
    ImGui::Spacing();
    if (ImGui::Button("      Quit      ", ImVec2(460, 38))) exit(0);
    ImGui::End();
    return false;
}
