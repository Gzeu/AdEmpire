#include "Dashboard.h"
#include "UIStyle.h"
#include "imgui.h"
#include <cstdio>

float Dashboard::s_revenueHistory[24] = {};
int   Dashboard::s_histIdx = 0;

void Dashboard::Render(GameState& gs) {
    ImGui::SetNextWindowPos(ImVec2(0, 40), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(330, 660), ImGuiCond_Always);
    ImGui::Begin("Dashboard", nullptr,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    // Agency name + period
    ImGui::SetWindowFontScale(1.1f);
    ImGui::TextColored(UIStyle::Accent, "%s", gs.agencyName.c_str());
    ImGui::SetWindowFontScale(1.0f);
    ImGui::TextColored(UIStyle::Muted, "Month %d  |  Year %d", gs.month, gs.year);
    ImGui::Spacing();

    // ---- KPI Cards row 1 ----
    int activeClients = 0;
    for (auto& c : gs.clients) if (c.active) activeClients++;
    int activeCamps = 0;
    for (auto& c : gs.campaigns) if (c.active) activeCamps++;

    char budgetStr[24], shareStr[24], clientStr[16], campStr[16];
    snprintf(budgetStr, 24, gs.budget >= 0 ? "$%.0f" : "-$%.0f",
             gs.budget >= 0 ? gs.budget : -gs.budget);
    snprintf(shareStr,  24, "%.1f%%", gs.playerMarketShare);
    snprintf(clientStr, 16, "%d", activeClients);
    snprintf(campStr,   16, "%d", activeCamps);

    float revDelta = gs.monthlyRevenue - gs.monthlyExpenses;
    char revSub[32];
    snprintf(revSub, 32, revDelta >= 0 ? "+$%.0f/mo" : "-$%.0f/mo",
             revDelta >= 0 ? revDelta : -revDelta);

    UIStyle::KpiCard("CASH", budgetStr, revSub,
        gs.budget >= 5000.f ? UIStyle::Positive
      : gs.budget >= 0     ? UIStyle::Warning
                            : UIStyle::Negative);
    ImGui::SameLine(0, 6);
    UIStyle::KpiCard("MARKET SHARE", shareStr, "Goal: 35%", UIStyle::Gold);

    ImGui::Spacing();
    UIStyle::KpiCard("CLIENTS", clientStr, "active", UIStyle::Accent);
    ImGui::SameLine(0, 6);
    UIStyle::KpiCard("CAMPAIGNS", campStr, "running", UIStyle::Accent);
    ImGui::Spacing();

    ImGui::Separator();

    // Revenue / Expenses
    ImGui::TextColored(UIStyle::Positive, "Revenue: $%.0f/mo", gs.monthlyRevenue);
    ImGui::TextColored(UIStyle::Negative, "Expenses: $%.0f/mo", gs.monthlyExpenses);
    ImGui::TextColored(revDelta >= 0 ? UIStyle::Positive : UIStyle::Negative,
        "Net: %+.0f/mo", revDelta);
    ImGui::Separator();

    // Staff count
    ImGui::Text("Staff: %d  |  All-time revenue: $%.0f",
        (int)gs.staff.size(), gs.stats.totalRevenue);

    // Reputation bar
    ImGui::Spacing();
    ImGui::TextColored(UIStyle::Muted, "Agency Reputation");
    char repLabel[24]; snprintf(repLabel, 24, "%.0f / 100", gs.stats.reputation);
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, UIStyle::Accent);
    ImGui::ProgressBar(gs.stats.reputation / 100.f, ImVec2(-1, 16), repLabel);
    ImGui::PopStyleColor();

    // Market share toward goal
    ImGui::Spacing();
    ImGui::TextColored(UIStyle::Muted, "Market Share Progress");
    char msLabel[24]; snprintf(msLabel, 24, "%.1f%% / 35%%", gs.playerMarketShare);
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, UIStyle::Gold);
    ImGui::ProgressBar(gs.playerMarketShare / 35.f, ImVec2(-1, 16), msLabel);
    ImGui::PopStyleColor();
    ImGui::Separator();

    // Revenue history chart
    s_revenueHistory[s_histIdx % 24] = gs.monthlyRevenue;
    s_histIdx++;
    ImGui::TextColored(UIStyle::Muted, "Revenue History (24mo)");
    ImGui::PushStyleColor(ImGuiCol_PlotLines, UIStyle::Positive);
    ImGui::PlotLines("##rev", s_revenueHistory, 24, s_histIdx % 24,
        nullptr, 0.f,
        gs.stats.bestMonthRevenue * 1.3f + 1.f,
        ImVec2(-1, 90));
    ImGui::PopStyleColor();
    ImGui::Separator();

    // Active events
    if (!gs.activeEvents.empty()) {
        ImGui::TextColored(UIStyle::Warning, "Active Events:");
        for (auto& ev : gs.activeEvents) {
            ImGui::BulletText("%s (%dmo)", ev.title.c_str(), ev.monthsLeft);
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("%s\nImpact: %s",
                    ev.description.c_str(), ev.impact.c_str());
        }
    }

    ImGui::End();
}
