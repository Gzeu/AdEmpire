#include "GoalsPanel.h"
#include "imgui.h"
#include <cstdio>

static const char* GOAL_TYPE_NAMES[] = {
    "Monthly Revenue", "Active Clients", "Market Share", "Campaigns Launched"
};

void GoalsPanel::Render(GameState& gs) {
    if (!gs.showGoals) return;
    ImGui::SetNextWindowPos(ImVec2(325, 40), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(700, 560), ImGuiCond_Always);
    ImGui::Begin("Quarterly Goals", &gs.showGoals,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    int q = gs.month / 3 + 1;
    ImGui::TextColored(ImVec4(0.8f,0.8f,0.3f,1.f),
        "Quarter %d  (Month %d)", q, gs.month);
    ImGui::Text("Complete goals before the quarter ends to earn rewards.");
    ImGui::Separator();
    ImGui::Spacing();

    for (auto& goal : gs.quarterlyGoals) {
        ImGui::PushID(&goal);

        ImVec4 titleColor = goal.completed ? ImVec4(0.3f,1.f,0.3f,1.f)
                          : goal.failed    ? ImVec4(1.f,0.3f,0.3f,1.f)
                                           : ImVec4(1.f,1.f,1.f,1.f);

        const char* status = goal.completed ? " [COMPLETED]" : goal.failed ? " [FAILED]" : "";
        ImGui::TextColored(titleColor, "%s%s",
            GOAL_TYPE_NAMES[(int)goal.type], status);

        // Progress bar
        float progress = (goal.target > 0) ? goal.current / goal.target : 0.f;
        progress = std::min(progress, 1.f);
        char label[48];
        if (goal.type == GoalType::Revenue || goal.type == GoalType::Reward)
            snprintf(label, 48, "$%.0f / $%.0f", goal.current, goal.target);
        else
            snprintf(label, 48, "%.1f / %.0f", goal.current, goal.target);

        ImVec4 barColor = goal.completed ? ImVec4(0.2f,0.8f,0.2f,1.f)
                        : goal.failed    ? ImVec4(0.6f,0.2f,0.2f,1.f)
                                         : ImVec4(0.2f,0.5f,0.9f,1.f);
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, barColor);
        ImGui::ProgressBar(progress, ImVec2(-1, 22), label);
        ImGui::PopStyleColor();

        ImGui::TextColored(ImVec4(0.5f,1.f,0.5f,1.f),
            "  Reward: +$%.0f", goal.reward);
        ImGui::Spacing();
        ImGui::PopID();
    }

    ImGui::Separator();
    ImGui::TextDisabled("New goals are generated automatically every 3 months.");
    ImGui::TextDisabled("Difficulty scales x1.3 per quarter.");
    ImGui::End();
}
