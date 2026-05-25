#include "AchievementsPanel.h"
#include "imgui.h"
#include <cstdio>

void AchievementsPanel::Render(GameState& gs) {
    if (!gs.showAchievements) return;
    ImGui::SetNextWindowPos(ImVec2(325, 40), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(700, 560), ImGuiCond_Always);
    ImGui::Begin("Achievements", &gs.showAchievements,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    int total    = (int)gs.achievements.size();
    int unlocked = 0;
    for (auto& a : gs.achievements) if (a.unlocked) unlocked++;

    ImGui::Text("Unlocked: %d / %d", unlocked, total);
    char prog[32]; snprintf(prog, 32, "%d/%d", unlocked, total);
    ImGui::ProgressBar((float)unlocked / (float)(total ? total : 1),
        ImVec2(-1, 14), prog);
    ImGui::Separator();
    ImGui::Spacing();

    // Two-column grid
    if (ImGui::BeginTable("##ach", 2, ImGuiTableFlags_None)) {
        for (auto& a : gs.achievements) {
            ImGui::TableNextColumn();
            ImVec4 col = a.unlocked
                ? ImVec4(0.3f,1.f,0.4f,1.f)
                : ImVec4(0.4f,0.4f,0.4f,1.f);
            ImGui::PushStyleColor(ImGuiCol_ChildBg,
                a.unlocked ? ImVec4(0.08f,0.18f,0.10f,0.9f)
                           : ImVec4(0.08f,0.08f,0.12f,0.9f));
            ImGui::BeginChild(a.id.c_str(), ImVec2(-1, 64), true);
            ImGui::TextColored(col, "%s  %s",
                a.unlocked ? "\u2705" : "\ud83d\udd12", a.name.c_str());
            ImGui::TextWrapped("%s", a.description.c_str());
            ImGui::EndChild();
            ImGui::PopStyleColor();
        }
        ImGui::EndTable();
    }
    ImGui::End();
}
