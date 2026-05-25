#include "StatsPanel.h"
#include "../systems/StatsTracker.h"
#include "UIStyle.h"
#include "imgui.h"
#include <cstdio>
#include <algorithm>

void StatsPanel::Render(GameState& gs) {
    if (!gs.showStats) return;
    ImGui::SetNextWindowPos(ImVec2(325, 40), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(700, 560), ImGuiCond_Always);
    ImGui::Begin("Analytics", &gs.showStats,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    ImGui::Text("Stats panel under construction - API mismatch");
    ImGui::Text("Total Revenue: $%.0f", gs.stats.totalRevenue);
    ImGui::Text("Months Played: %d", gs.stats.monthsPlayed);
    ImGui::Text("Reputation: %.1f", gs.stats.reputation);
    ImGui::Text("Clients Acquired: %d", gs.stats.clientsAcquired);
    ImGui::Text("Campaigns Completed: %d", gs.stats.campaignsCompleted);

    ImGui::End();
}
