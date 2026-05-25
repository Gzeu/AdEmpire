#include "AchievementsPanel.h"
#include "../systems/AchievementSystem.h"
#include "imgui.h"

void AchievementsPanel::Render(GameState& gs, bool& show) {
    if (!show) return;
    ImGui::SetNextWindowPos(ImVec2(325, 40), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(700, 560), ImGuiCond_Always);
    ImGui::Begin("Achievements", &show,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    auto& sys = AchievementSystem::Get();
    int unlocked = sys.CountUnlocked();
    int total    = (int)sys.achievements.size();
    char prog[32]; snprintf(prog, 32, "%d / %d", unlocked, total);
    ImGui::Text("Progress:"); ImGui::SameLine();
    ImGui::ProgressBar((float)unlocked / total, ImVec2(300, 18), prog);
    ImGui::Separator();
    ImGui::Spacing();

    int cols = 2;
    if (ImGui::BeginTable("##ach", cols, ImGuiTableFlags_BordersInner | ImGuiTableFlags_PadOuterX)) {
        for (auto& a : sys.achievements) {
            ImGui::TableNextColumn();
            ImGui::PushID(a.id);
            ImVec4 bg = a.unlocked
                ? ImVec4(0.1f, 0.25f, 0.1f, 0.9f)
                : ImVec4(0.12f, 0.12f, 0.18f, 0.9f);
            ImGui::PushStyleColor(ImGuiCol_ChildBg, bg);
            ImGui::BeginChild("##achcard", ImVec2(-1, 70), true);

            if (a.unlocked)
                ImGui::TextColored(ImVec4(0.3f,1.f,0.4f,1.f), "%s  %s",
                    a.icon.c_str(), a.title.c_str());
            else
                ImGui::TextColored(ImVec4(0.5f,0.5f,0.5f,1.f), "???  %s",
                    a.title.c_str());

            if (a.unlocked) {
                ImGui::TextWrapped("%s", a.description.c_str());
                ImGui::TextColored(ImVec4(0.4f,0.8f,0.4f,1.f),
                    "Unlocked month %d", a.unlockedMonth);
            } else {
                ImGui::TextColored(ImVec4(0.4f,0.4f,0.4f,1.f),
                    "%s", a.description.c_str());
            }
            ImGui::EndChild();
            ImGui::PopStyleColor();
            ImGui::PopID();
        }
        ImGui::EndTable();
    }
    ImGui::End();
}
