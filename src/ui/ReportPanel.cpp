#include "ReportPanel.h"
#include "UIStyle.h"
#include "../systems/ToastSystem.h"
#include "imgui.h"
#include <cstdio>
#include <algorithm>

MonthlyReport ReportPanel::s_last = {};
bool          ReportPanel::s_showOnMonthEnd = false;

void ReportPanel::GenerateReport(GameState& gs) {
    MonthlyReport r;
    r.month           = gs.month;
    r.year            = gs.year;
    r.totalRevenue    = gs.monthlyRevenue;
    r.totalExpenses   = gs.monthlyExpenses;
    r.netProfit       = gs.monthlyRevenue - gs.monthlyExpenses;
    r.marketShareEnd  = gs.playerMarketShare;
    r.campaignsCompleted = 0;
    r.activeCampaigns    = 0;
    r.topCampaignFee     = 0.f;

    for (int i = 0; i < 6; i++) r.revenueByChannel[i] = 0.f;

    for (auto& c : gs.campaigns) {
        if (c.active) r.activeCampaigns++;
        if (c.completed) r.campaignsCompleted++;
        r.revenueByChannel[(int)c.channel] += c.agencyFee;
        if (c.agencyFee > r.topCampaignFee) {
            r.topCampaignFee  = c.agencyFee;
            r.topCampaignName = c.name;
        }
    }
    s_last = r;
    s_showOnMonthEnd = true;
}

void ReportPanel::Render(GameState& gs) {
    if (!gs.showReport) return;
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f,0.5f));
    ImGui::SetNextWindowSize(ImVec2(620, 520));
    ImGui::Begin("Monthly Report", &gs.showReport,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    // Header
    char title[64];
    snprintf(title, 64, "End of Month %d / Year %d", s_last.month, s_last.year);
    ImGui::TextColored(UIStyle::Accent, "%s", title);
    ImGui::Separator();
    ImGui::Spacing();

    // Top KPI row
    char cash[32], rev[32], exp[32], profit[32];
    snprintf(cash,   32, "$%.0f", gs.budget);
    snprintf(rev,    32, "$%.0f", s_last.totalRevenue);
    snprintf(exp,    32, "$%.0f", s_last.totalExpenses);
    snprintf(profit, 32, s_last.netProfit >= 0 ? "+$%.0f" : "-$%.0f",
             std::abs(s_last.netProfit));

    UIStyle::KpiCard("Cash Balance", cash, nullptr,
        gs.budget >= 0 ? UIStyle::Positive : UIStyle::Negative);
    ImGui::SameLine(0, 8);
    UIStyle::KpiCard("Revenue", rev, "this month", UIStyle::Positive);
    ImGui::SameLine(0, 8);
    UIStyle::KpiCard("Expenses", exp, "salaries etc.", UIStyle::Negative);
    ImGui::SameLine(0, 8);
    UIStyle::KpiCard("Net Profit", profit, nullptr,
        s_last.netProfit >= 0 ? UIStyle::Positive : UIStyle::Negative);
    ImGui::Spacing();

    // Revenue by channel
    UIStyle::SectionHeader("Revenue Breakdown by Channel");
    for (int i = 0; i < 6; i++) {
        if (s_last.revenueByChannel[i] <= 0.f) continue;
        float pct = s_last.totalRevenue > 0
            ? s_last.revenueByChannel[i] / s_last.totalRevenue : 0.f;
        char lbl[48];
        snprintf(lbl, 48, "%s  $%.0f", ChannelNames[i], s_last.revenueByChannel[i]);
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, UIStyle::Accent);
        ImGui::ProgressBar(pct, ImVec2(-1, 18), lbl);
        ImGui::PopStyleColor();
    }
    ImGui::Spacing();

    // Market share
    UIStyle::SectionHeader("Market Position");
    char ms[48];
    snprintf(ms, 48, "Market Share: %.1f%%", s_last.marketShareEnd);
    ImGui::TextColored(UIStyle::Gold, "%s", ms);
    ImGui::Text("Goal: 35.0%%");
    ImGui::ProgressBar(s_last.marketShareEnd / 35.f, ImVec2(-1, 16), ms);
    ImGui::Spacing();

    // Campaign summary
    UIStyle::SectionHeader("Campaigns");
    ImGui::Text("Active: %d   |   Completed this month: %d",
        s_last.activeCampaigns, s_last.campaignsCompleted);
    if (!s_last.topCampaignName.empty())
        ImGui::TextColored(UIStyle::Gold, "Top Campaign: %s  (+$%.0f fee)",
            s_last.topCampaignName.c_str(), s_last.topCampaignFee);
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (UIStyle::PrimaryButton("Close Report", ImVec2(-1, 36)))
        gs.showReport = false;

    ImGui::End();
}
