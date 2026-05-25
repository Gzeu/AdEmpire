#include "MainMenu.h"
#include "UIStyle.h"
#include "../core/Difficulty.h"
#include "../systems/AICompetitor.h"
#include "../systems/EventSystem.h"
#include "imgui.h"
#include <cstring>

char MainMenu::s_agencyNameBuf[64] = "My Ad Agency";
bool MainMenu::s_showMenu = true;
static int s_diffIndex = 1; // 0=Easy 1=Normal 2=Hard

bool MainMenu::Render(GameState& gs) {
    if (!s_showMenu) return false;
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(500, 440));
    ImGui::SetNextWindowBgAlpha(0.97f);
    ImGui::Begin("##MainMenu", nullptr,
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);

    // Title
    ImGui::SetCursorPosX(90.f);
    ImGui::SetWindowFontScale(2.6f);
    ImGui::TextColored(UIStyle::Accent, "AdEmpire");
    ImGui::SetWindowFontScale(1.0f);
    ImGui::TextColored(UIStyle::Muted, "       Marketing Tycoon Simulator  v0.3");
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Agency name
    ImGui::TextColored(UIStyle::Muted, "Agency Name:");
    ImGui::SetNextItemWidth(-1);
    ImGui::InputText("##agname", s_agencyNameBuf, 64);
    ImGui::Spacing();

    // Difficulty selector
    ImGui::TextColored(UIStyle::Muted, "Difficulty:");
    ImGui::Spacing();
    const char* labels[] = {"  Easy  ", " Normal ", "  Hard  "};
    for (int i = 0; i < 3; i++) {
        if (i > 0) ImGui::SameLine(0, 6);
        bool sel = (s_diffIndex == i);
        ImVec4 col = sel
            ? (i == 0 ? UIStyle::Positive : i == 1 ? UIStyle::Accent : UIStyle::Negative)
            : ImVec4(0.25f,0.28f,0.40f,1.f);
        ImGui::PushStyleColor(ImGuiCol_Button, col);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, col);
        if (ImGui::Button(labels[i], ImVec2(148, 36))) s_diffIndex = i;
        ImGui::PopStyleColor(2);
    }
    ImGui::Spacing();

    // Show selected difficulty description
    auto dcfg = Difficulty::Get((DifficultyLevel)s_diffIndex);
    ImGui::TextColored(UIStyle::Muted, "%s", dcfg.description);
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Win/lose info
    ImGui::TextColored(UIStyle::Positive, "Win:  Reach 35%% market share");
    ImGui::TextColored(UIStyle::Negative, "Lose: Budget drops below -$50,000");
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (UIStyle::GreenButton("  Start Game  ", ImVec2(-1, 46))) {
        gs.agencyName = std::string(s_agencyNameBuf);
        gs.budget     = dcfg.startingBudget;
        AICompetitor::Init(gs);
        // Apply difficulty aggressiveness
        for (auto& ai : gs.competitors)
            ai.aggressiveness *= dcfg.aiAggressiveness;
        EventSystem::Init(gs);
        gs.clients = {
            {1,"TastyBite Foods",   ClientIndustry::Food,    3000, 70, 6,  false, true, false, ContractType::Monthly, {}, 0},
            {2,"UrbanThread Co",    ClientIndustry::Fashion, 4500, 70, 6,  false, true, false, ContractType::Monthly, {}, 0},
            {3,"SparkTech Ltd",     ClientIndustry::Tech,    8000, 70, 12, false, true, false, ContractType::Monthly, {}, 0},
            {4,"WealthWise",        ClientIndustry::Finance, 12000,70, 12, false, true, false, ContractType::Monthly, {}, 0},
            {5,"VitalHealth Clinic",ClientIndustry::Health,  5000, 70, 6,  false, true, false, ContractType::Monthly, {}, 0},
            {6,"EduPath Academy",   ClientIndustry::Education,3500,70, 6,  false, true, false, ContractType::Monthly, {}, 0},
            {7,"GreenGrove Organic",ClientIndustry::Food,    2500, 70, 6,  false, true, false, ContractType::Monthly, {}, 0},
            {8,"PixelForge Games",  ClientIndustry::Gaming,  9000, 70, 12, false, true, false, ContractType::Monthly, {}, 0},
        };
        gs.nextClientId = 9;
        s_showMenu = false;
        return true;
    }
    ImGui::Spacing();
    if (ImGui::Button("     Quit     ", ImVec2(-1, 32))) exit(0);
    ImGui::End();
    return false;
}
