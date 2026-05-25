#include "AchievementsPanel.h"
#include "../systems/AchievementsSystem.h"
#include "imgui.h"

void AchievementsPanel::Render(GameState& gs) {
    if (!gs.showAchievements) return;
    ImGui::SetNextWindowPos(ImVec2(325, 40), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(700, 560), ImGuiCond_Always);
    ImGui::Begin("Achievements", &gs.showAchievements,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    int total   = (int)AchievementsSystem::s_achievements.size();
    int unlocked = 0;
    for (auto& a : AchievementsSystem::s_achievements) if (a.unlocked) unlocked++;

    char progLabel[32];
    snprintf(progLabel, 32, "%d / %d", unlocked, total);
    ImGui::Text("Progress:"); ImGui::SameLine();
    ImGui::ProgressBar((float)unlocked / total, ImVec2(300, 18), progLabel);
    ImGui::Separator();

    static const char* categories[] = {
        "All","Revenue","Clients","Campaigns","Staff","Market","Survival","Reputation"
    };
    static int cat = 0;
    ImGui::Text("Filter:"); ImGui::SameLine();
    for (int i = 0; i < 8; i++) {
        if (i > 0) ImGui::SameLine();
        bool sel = (cat == i);
        if (sel) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f,0.6f,1.f,1.f));
        if (ImGui::Button(categories[i])) cat = i;
        if (sel) ImGui::PopStyleColor();
    }
    ImGui::Separator();

    ImGui::BeginChild("##achlist", ImVec2(-1,-1), false);
    for (auto& a : AchievementsSystem::s_achievements) {
        bool show = (cat == 0 || a.category == categories[cat]);
        if (!show) continue;
        ImGui::PushID(a.id);
        ImVec4 col = a.unlocked
            ? ImVec4(0.3f, 1.f, 0.4f, 1.f)
            : ImVec4(0.5f, 0.5f, 0.5f, 0.6f);
        ImGui::TextColored(col, a.unlocked ? "[X]" : "[ ]");
        ImGui::SameLine();
        ImGui::TextColored(col, "%s", a.title.c_str());
        ImGui::SameLine(320);
        ImGui::TextColored(ImVec4(0.6f,0.6f,0.6f,1.f), "[%s]", a.category.c_str());
        ImGui::TextDisabled("    %s", a.description.c_str());
        ImGui::Separator();
        ImGui::PopID();
    }
    ImGui::EndChild();
    ImGui::End();
}
