#include "ReportPanel.h"
#include "../systems/StatsTracker.h"
#include "imgui.h"
#include <cstdio>
#include <algorithm>

ReportPanel::ReportData ReportPanel::s_last = {};
bool ReportPanel::s_hasReport = false;

void ReportPanel::GenerateMonthlyReport(const GameState& gs) {
    s_last.month      = gs.month;
    s_last.year       = gs.year;
    s_last.revenue    = gs.monthlyRevenue;
    s_last.expenses   = gs.monthlyExpenses;
    s_last.profit     = gs.monthlyRevenue - gs.monthlyExpenses;
    s_last.marketShare = gs.playerMarketShare;
    s_last.eventsCount = (int)gs.activeEvents.size();

    int clients = 0, camps = 0;
    for (auto& c : gs.clients)   if (c.active) clients++;
    for (auto& c : gs.campaigns) if (c.active) camps++;
    s_last.activeClients   = clients;
    s_last.activeCampaigns = camps;

    s_last.bestCampaignFee  = 0.f;
    s_last.bestCampaignName = "—";
    for (auto& c : gs.campaigns) {
        if (c.agencyFee > s_last.bestCampaignFee) {
            s_last.bestCampaignFee  = c.agencyFee;
            s_last.bestCampaignName = c.name;
        }
    }
    s_hasReport = true;
}

void ReportPanel::Render(GameState& gs) {
    if (!gs.showReport) return;
    ImGui::SetNextWindowPos(ImVec2(325, 40), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(700, 560), ImGuiCond_Always);
    ImGui::Begin("Monthly Report", &gs.showReport,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    if (!s_hasReport) {
        ImGui::TextColored(ImVec4(0.6f,0.6f,0.6f,1.f),
            "No report yet. Complete at least one month to see results.");
        ImGui::End(); return;
    }

    ImGui::TextColored(ImVec4(0.3f,0.8f,1.f,1.f),
        "Report — Month %d / Year %d", s_last.month, s_last.year);
    ImGui::Separator();

    // ── Financial summary
    if (ImGui::BeginTable("##rep", 2, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_RowBg)) {
        auto Row = [&](const char* label, const char* val, ImVec4 col = ImVec4(1,1,1,1)) {
            ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0);
            ImGui::Text("%s", label);
            ImGui::TableSetColumnIndex(1);
            ImGui::TextColored(col, "%s", val);
        };
        char buf[64];

        snprintf(buf, 64, "$%.0f", s_last.revenue);
        Row("Monthly Revenue", buf, ImVec4(0.3f,1.f,0.4f,1.f));

        snprintf(buf, 64, "$%.0f", s_last.expenses);
        Row("Monthly Expenses", buf, ImVec4(1.f,0.5f,0.5f,1.f));

        snprintf(buf, 64, "$%.0f", s_last.profit);
        Row("Net Profit", buf, s_last.profit >= 0
            ? ImVec4(0.3f,1.f,0.4f,1.f) : ImVec4(1.f,0.3f,0.3f,1.f));

        snprintf(buf, 64, "%.1f%%", s_last.marketShare);
        Row("Market Share", buf, ImVec4(0.5f,0.8f,1.f,1.f));

        snprintf(buf, 64, "%d", s_last.activeClients);
        Row("Active Clients", buf);

        snprintf(buf, 64, "%d", s_last.activeCampaigns);
        Row("Active Campaigns", buf);

        snprintf(buf, 64, "%s  ($%.0f/mo)",
            s_last.bestCampaignName.c_str(), s_last.bestCampaignFee);
        Row("Best Campaign", buf, ImVec4(1.f,0.8f,0.2f,1.f));

        snprintf(buf, 64, "%d active", s_last.eventsCount);
        Row("Market Events", buf);

        ImGui::EndTable();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Revenue by Channel (lifetime)");
    float chRev[6] = {};
    // TODO: Implement channel revenue tracking in v1.2
    // StatsTracker::Get().FillChannelRevenue(chRev);
    for (int i = 0; i < 6; i++) {
        ImGui::Text("  %-16s", ChannelNames[i]);
        ImGui::SameLine();
        float maxRev = *std::max_element(chRev, chRev + 6);
        char lbl[32]; snprintf(lbl, 32, "$%.0f", chRev[i]);
        ImGui::ProgressBar(maxRev > 0 ? chRev[i] / maxRev : 0.f, ImVec2(300, 16), lbl);
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Industry Win Rate (pitches)");
    float winRates[8] = {};
    // TODO: Implement industry win rate tracking in v1.2
    // StatsTracker::Get().FillIndustryWinRate(winRates);
    for (int i = 0; i < 8; i++) {
        ImGui::Text("  %-18s", IndustryNames[i]);
        ImGui::SameLine();
        char lbl[32]; snprintf(lbl, 32, "%.0f%%", winRates[i] * 100.f);
        ImVec4 col = winRates[i] > 0.6f ? ImVec4(0.3f,1.f,0.4f,1.f)
                   : winRates[i] > 0.3f ? ImVec4(1.f,0.8f,0.2f,1.f)
                   : ImVec4(0.6f,0.6f,0.6f,1.f);
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, col);
        ImGui::ProgressBar(winRates[i], ImVec2(260, 16), lbl);
        ImGui::PopStyleColor();
    }
    ImGui::End();
}
