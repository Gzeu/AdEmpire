#include "AchievementsPanel.h"
#include "../systems/AchievementSystem.h"
#include "imgui.h"

void AchievementsPanel::Render(GameState& gs) {
    if (!gs.showAchievements) return;
    ImGui::SetNextWindowPos(ImVec2(325, 40), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(700, 560), ImGuiCond_Always);
    ImGui::Begin("Achievements", &gs.showAchievements,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    int unlocked = AchievementSystem::UnlockedCount();
    int total    = (int)AchievementSystem::All().size();
    char prog[32]; snprintf(prog, 32, "%d / %d", unlocked, total);
    ImGui::Text("Progress:");
    ImGui::SameLine();
    ImGui::ProgressBar((float)unlocked / total, ImVec2(300, 18), prog);
    ImGui::Separator();
    ImGui::Spacing();

    auto& all = AchievementSystem::All();
    int cols = 2;
    if (ImGui::BeginTable("##ach", cols, ImGuiTableFlags_None)) {
        for (int i = 0; i < (int)all.size(); i++) {
            ImGui::TableNextColumn();
            auto& a = all[i];
            ImGui::PushID(i);
            ImGui::BeginGroup();

            // Icon + title
            if (a.unlocked)
                ImGui::TextColored(ImVec4(1.f,0.85f,0.1f,1.f), "%s %s", a.icon.c_str(), a.title.c_str());
            else
                ImGui::TextDisabled("🔒 %s", a.title.c_str());

            // Description
            if (a.unlocked) {
                ImGui::TextWrapped("%s", a.description.c_str());
                if (a.monthUnlocked >= 0)
                    ImGui::TextColored(ImVec4(0.5f,0.9f,0.5f,1.f),
                        "Unlocked month %d", a.monthUnlocked);
            } else {
                ImGui::TextDisabled("%s", a.description.c_str());
            }

            ImGui::EndGroup();
            ImGui::PopID();
            ImGui::Spacing();
        }
        ImGui::EndTable();
    }
    ImGui::End();
}
