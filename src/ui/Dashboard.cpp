#include "Dashboard.h"
#include "Newsfeed.h"
#include "imgui.h"
#include <cstdio>

float Dashboard::s_revenueHistory[24] = {};
int   Dashboard::s_histIdx = 0;

void Dashboard::Render(GameState& gs) {
    ImGui::SetNextWindowPos(ImVec2(0, 40), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(320, 640), ImGuiCond_Always);
    ImGui::Begin("Dashboard", nullptr,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    ImGui::TextColored(ImVec4(0.3f,0.8f,1.f,1.f), "%s", gs.agencyName.c_str());
    ImGui::Text("Month %d / Year %d", gs.month, gs.year);
    ImGui::Text("Months played: %d", gs.stats.monthsPlayed);
    ImGui::Separator();

    // Budget
    ImVec4 budgetColor = gs.budget >= 0 ? ImVec4(0.3f,1.f,0.4f,1.f) : ImVec4(1.f,0.3f,0.3f,1.f);
    ImGui::TextColored(budgetColor, "Budget: $%.0f", gs.budget);
    float profit = gs.monthlyRevenue - gs.monthlyExpenses;
    ImGui::Text("Revenue:  $%.0f/mo",  gs.monthlyRevenue);
    ImGui::Text("Expenses: $%.0f/mo", gs.monthlyExpenses);
    ImVec4 profCol = profit >= 0 ? ImVec4(0.3f,1.f,0.4f,1.f) : ImVec4(1.f,0.3f,0.3f,1.f);
    ImGui::TextColored(profCol, "Profit:   $%.0f/mo", profit);
    ImGui::Text("Total Rev: $%.0f", gs.stats.totalRevenue);
    ImGui::Separator();

    // KPIs
    int activeClients = 0, activeCamps = 0;
    for (auto& c : gs.clients)   if (c.active)  activeClients++;
    for (auto& c : gs.campaigns) if (c.active)  activeCamps++;
    ImGui::Text("Active Clients:   %d", activeClients);
    ImGui::Text("Active Campaigns: %d", activeCamps);
    ImGui::Text("Staff Members:    %d", (int)gs.staff.size());
    ImGui::Text("Market Share:     %.1f%%", gs.playerMarketShare);
    ImGui::Separator();

    // Reputation
    char repLabel[32];
    snprintf(repLabel, 32, "Rep: %.0f/100", gs.stats.reputation);
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.3f,0.6f,1.f,1.f));
    ImGui::ProgressBar(gs.stats.reputation / 100.f, ImVec2(-1, 18), repLabel);
    ImGui::PopStyleColor();
    ImGui::Spacing();

    // Market share progress to victory
    char shareLabel[48];
    snprintf(shareLabel, 48, "Share: %.1f%% / 35%%", gs.playerMarketShare);
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.2f,0.9f,0.4f,1.f));
    ImGui::ProgressBar(gs.playerMarketShare / 35.f, ImVec2(-1, 18), shareLabel);
    ImGui::PopStyleColor();
    ImGui::Separator();

    // Revenue history
    s_revenueHistory[s_histIdx % 24] = gs.monthlyRevenue;
    s_histIdx++;
    ImGui::Text("Revenue (last 24 months):");
    ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.3f,0.9f,0.5f,1.f));
    ImGui::PlotLines("##rev", s_revenueHistory, 24, s_histIdx % 24,
        nullptr, 0.f, gs.stats.bestMonthRevenue * 1.3f + 1.f, ImVec2(-1, 90));
    ImGui::PopStyleColor();
    ImGui::Separator();

    // Active events summary
    if (!gs.activeEvents.empty()) {
        ImGui::TextColored(ImVec4(1.f,0.8f,0.2f,1.f), "Market Events:");
        for (auto& ev : gs.activeEvents)
            ImGui::BulletText("%s (%dmo)", ev.title.c_str(), ev.monthsLeft);
    } else {
        ImGui::TextDisabled("No active market events.");
    }
    ImGui::End();
}
