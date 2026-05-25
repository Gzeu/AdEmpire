#include "AchievementsPanel.h"
#include "../systems/AchievementsSystem.h"
#include "imgui.h"

void AchievementsPanel::Render(GameState& gs) {
    if (!gs.showAchievements) return;
    ImGui::SetNextWindowPos(ImVec2(325, 40), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(700, 560), ImGuiCond_Always);
    ImGui::Begin("Achievements", &gs.showAchievements,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    int total    = (int)AchievementsSystem::GetAll().size();
    int unlocked = AchievementsSystem::GetUnlockedCount();
    ImGui::Text("Unlocked: %d / %d", unlocked, total);
    ImGui::ProgressBar((float)unlocked / (float)total, ImVec2(-1, 12));
    ImGui::Separator();

    if (ImGui::BeginTable("##ach", 4,
        ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
        ImGui::TableSetupColumn("Icon",  ImGuiTableColumnFlags_WidthFixed, 40);
        ImGui::TableSetupColumn("Title", ImGuiTableColumnFlags_WidthFixed, 160);
        ImGui::TableSetupColumn("Description");
        ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed, 80);
        ImGui::TableHeadersRow();

        for (auto& a : AchievementsSystem::GetAll()) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%s", a.icon.c_str());
            ImGui::TableSetColumnIndex(1);
            if (a.unlocked)
                ImGui::TextColored(ImVec4(0.3f,1.f,0.5f,1.f), "%s", a.title.c_str());
            else
                ImGui::TextDisabled("%s", a.title.c_str());
            ImGui::TableSetColumnIndex(2);
            ImGui::TextWrapped("%s", a.description.c_str());
            ImGui::TableSetColumnIndex(3);
            if (a.unlocked)
                ImGui::TextColored(ImVec4(0.3f,1.f,0.4f,1.f), "✓ Mo %d", a.monthUnlocked);
            else
                ImGui::TextDisabled("Locked");
        }
        ImGui::EndTable();
    }
    ImGui::End();
}

void AchievementsPanel::RenderPopupOverlay() {
    if (!AchievementsSystem::s_showPopup) return;
    AchievementsSystem::s_popupTimer -= ImGui::GetIO().DeltaTime;
    if (AchievementsSystem::s_popupTimer <= 0.f) {
        AchievementsSystem::s_showPopup = false;
        return;
    }
    float alpha = std::min(AchievementsSystem::s_popupTimer, 1.0f);
    ImGui::SetNextWindowBgAlpha(alpha * 0.9f);
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 320, 50));
    ImGui::SetNextWindowSize(ImVec2(300, 60));
    ImGui::Begin("##achpopup", nullptr,
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs |
        ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoBringToFrontOnFocus);
    ImGui::SetWindowFontScale(1.1f);
    ImGui::TextColored(ImVec4(1.f,0.85f,0.1f,alpha),
        "%s", AchievementsSystem::s_popupText.c_str());
    ImGui::End();
}
