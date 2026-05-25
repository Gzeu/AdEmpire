#include "MainMenu.h"
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
    ImGui::SetNextWindowSize(ImVec2(480, 380));
    ImGui::SetNextWindowBgAlpha(0.97f);
    ImGui::Begin("##MainMenu", nullptr,
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);

    ImGui::SetCursorPosX((480 - ImGui::CalcTextSize("AdEmpire").x * 2.2f) * 0.5f);
    ImGui::SetWindowFontScale(2.2f);
    ImGui::TextColored(ImVec4(0.3f,0.7f,1.f,1.f), "AdEmpire");
    ImGui::SetWindowFontScale(1.0f);
    ImGui::TextColored(ImVec4(0.6f,0.8f,1.f,0.8f),
        "          Marketing Tycoon Simulator");
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Agency Name:");
    ImGui::SetNextItemWidth(-1);
    ImGui::InputText("##agname", s_agencyNameBuf, 64);
    ImGui::Spacing();
    ImGui::TextColored(ImVec4(0.5f,0.9f,0.5f,1.f),
        "Starting budget: $10,000");
    ImGui::TextColored(ImVec4(0.8f,0.6f,0.2f,1.f),
        "Goal: Reach 35%% market share");
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    float bw = 440;
    if (ImGui::Button("  New Game  ", ImVec2(bw, 44))) {
        gs.agencyName = std::string(s_agencyNameBuf);
        AICompetitor::Init(gs);
        EventSystem::Init(gs);
        // Add initial available clients
        gs.clients = {
            {1,"TastyBite Foods",   ClientIndustry::Food,    3000, 70, 6,  false, true, {}, 0},
            {2,"UrbanThread Co",    ClientIndustry::Fashion, 4500, 70, 6,  false, true, {}, 0},
            {3,"SparkTech Ltd",     ClientIndustry::Tech,    8000, 70, 12, false, true, {}, 0},
            {4,"WealthWise",        ClientIndustry::Finance, 12000,70, 12, false, true, {}, 0},
            {5,"VitalHealth Clinic",ClientIndustry::Health,  5000, 70, 6,  false, true, {}, 0},
            {6,"EduPath Academy",   ClientIndustry::Education,3500,70, 6,  false, true, {}, 0},
        };
        gs.nextClientId = 7;
        s_showMenu = false;
        return true;
    }
    ImGui::Spacing();
    if (ImGui::Button("     Quit     ", ImVec2(bw, 36)))
        exit(0);
    ImGui::End();
    return false;
}
