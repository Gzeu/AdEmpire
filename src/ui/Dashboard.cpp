#include "Dashboard.h"
#include "imgui.h"
#include <cstdio>

float Dashboard::s_revenueHistory[24] = {};
int   Dashboard::s_histIdx = 0;

void Dashboard::Render(GameState& gs) {
    ImGui::SetNextWindowPos(ImVec2(0, 40), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(320, 560), ImGuiCond_Always);
    ImGui::Begin("Dashboard", nullptr,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove);

    // Agency name + period
    ImGui::TextColored(ImVec4(0.3f,0.8f,1.f,1.f), "%s", gs.agencyName.c_str());
    ImGui::Text("Month %d / Year %d", gs.month, gs.year);
    ImGui::Separator();

    // Budget
    ImVec4 budgetColor = gs.budget > 0
        ? ImVec4(0.3f,1.f,0.4f,1.f)
        : ImVec4(1.f,0.3f,0.3f,1.f);
    ImGui::TextColored(budgetColor, "Budget: $%.0f", gs.budget);
    ImGui::Text("Revenue: $%.0f/mo",  gs.monthlyRevenue);
    ImGui::Text("Expenses: $%.0f/mo", gs.monthlyExpenses);

    float profit = gs.monthlyRevenue - gs.monthlyExpenses;
    ImVec4 profCol = profit >= 0 ? ImVec4(0.3f,1.f,0.4f,1.f) : ImVec4(1.f,0.3f,0.3f,1.f);
    ImGui::TextColored(profCol, "Profit: $%.0f/mo", profit);
    ImGui::Separator();

    // KPIs
    int activeClients = 0;
    for (auto& c : gs.clients) if (c.active) activeClients++;
    int activeCamps = 0;
    for (auto& c : gs.campaigns) if (c.active) activeCamps++;

    ImGui::Text("Active Clients:   %d", activeClients);
    ImGui::Text("Active Campaigns: %d", activeCamps);
    ImGui::Text("Staff:            %d", (int)gs.staff.size());
    ImGui::Text("Market Share:     %.1f%%", gs.playerMarketShare);
    ImGui::Separator();

    // Reputation bar
    char repLabel[32];
    snprintf(repLabel, 32, "Rep: %.0f%%", gs.stats.reputation);
    ImGui::ProgressBar(gs.stats.reputation / 100.f, ImVec2(-1, 16), repLabel);
    ImGui::Spacing();

    // Revenue history chart
    s_revenueHistory[s_histIdx % 24] = gs.monthlyRevenue;
    s_histIdx++;
    ImGui::Text("Revenue History (24mo)");
    ImGui::PlotLines("##rev", s_revenueHistory, 24, s_histIdx % 24,
        nullptr, 0.f, gs.stats.bestMonthRevenue * 1.2f + 1.f, ImVec2(-1, 80));
    ImGui::Separator();

    // Active events
    if (!gs.activeEvents.empty()) {
        ImGui::TextColored(ImVec4(1.f,0.8f,0.2f,1.f), "Active Events:");
        for (auto& ev : gs.activeEvents)
            ImGui::BulletText("%s (%dmo)", ev.title.c_str(), ev.monthsLeft);
    }

    ImGui::End();
}
