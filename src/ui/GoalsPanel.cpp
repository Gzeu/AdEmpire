#include "GoalsPanel.h"
#include "imgui.h"
#include <cstdio>

void GoalsPanel::Render(GameState& gs) {
    if (!gs.showGoals) return;
    ImGui::SetNextWindowPos(ImVec2(325, 40), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(700, 560), ImGuiCond_Always);
    ImGui::Begin("Quarterly Goals", &gs.showGoals,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    int q = ((gs.month - 1) / 3) + 1;
    ImGui::TextColored(ImVec4(0.3f,0.8f,1.f,1.f),
        "Q%d Goals  |  Month %d  |  Year %d", q, gs.month, gs.year);
    ImGui::Text("Goals Met Total: %d", gs.stats.quarterlyGoalsMet);
    ImGui::Separator();
    ImGui::Spacing();

    for (auto& g : gs.quarterlyGoals) {
        ImVec4 col = g.completed ? ImVec4(0.3f,1.f,0.4f,1.f)
                   : g.failed   ? ImVec4(1.f,0.3f,0.3f,1.f)
                   : ImVec4(1.f,1.f,1.f,1.f);
        ImGui::PushID(&g);

        ImGui::TextColored(col, "%s", g.description.c_str());
        ImGui::SameLine(500);
        if (g.completed)      ImGui::TextColored(ImVec4(0.3f,1.f,0.4f,1.f), "DONE");
        else if (g.failed)    ImGui::TextColored(ImVec4(1.f,0.3f,0.3f,1.f), "FAILED");
        else                  ImGui::TextDisabled("In Progress");

        char progLabel[48];
        snprintf(progLabel, 48, "%.1f / %.1f", g.current, g.target);
        float pct = g.target > 0.f ? std::min(g.current / g.target, 1.f) : 0.f;
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram,
            g.completed ? ImVec4(0.3f,1.f,0.4f,1.f) :
            g.failed    ? ImVec4(1.f,0.3f,0.3f,1.f) :
                          ImVec4(0.3f,0.6f,1.f,1.f));
        ImGui::ProgressBar(pct, ImVec2(-1, 16), progLabel);
        ImGui::PopStyleColor();

        ImGui::TextColored(ImVec4(0.6f,1.f,0.6f,0.9f),
            "  Reward: $%.0f + %.0f reputation",
            g.rewardBudget, g.rewardReputation);
        ImGui::Separator();
        ImGui::PopID();
    }
    ImGui::End();
}
