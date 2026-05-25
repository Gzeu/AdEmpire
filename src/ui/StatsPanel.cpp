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

    auto& tracker = StatsTracker::Get();

    if (ImGui::BeginTabBar("##statstabs")) {

        // ── TAB 1: Revenue by Channel ─────────────────────────────────────
        if (ImGui::BeginTabItem("Revenue by Channel")) {
            UIStyle::SectionHeader("Channel Performance");
            float maxRev = 0;
            for (int i = 0; i < 6; i++)
                maxRev = std::max(maxRev, tracker.GetChannel((ChannelType)i).totalRevenue + 1.f);

            if (ImGui::BeginTable("##chrev", 5,
                ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                ImGui::TableSetupColumn("Channel",    ImGuiTableColumnFlags_WidthFixed, 140);
                ImGui::TableSetupColumn("Revenue",    ImGuiTableColumnFlags_WidthFixed, 100);
                ImGui::TableSetupColumn("ROI",        ImGuiTableColumnFlags_WidthFixed, 80);
                ImGui::TableSetupColumn("Campaigns",  ImGuiTableColumnFlags_WidthFixed, 90);
                ImGui::TableSetupColumn("Share",      ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableHeadersRow();

                for (int i = 0; i < 6; i++) {
                    auto ch = (ChannelType)i;
                    auto& s = tracker.GetChannel(ch);
                    float share = tracker.GetChannelRevenueShare(ch);
                    float roi   = tracker.GetROI(ch);

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%s", ChannelNames[i]);

                    // Tooltip on channel name
                    if (ImGui::IsItemHovered()) {
                        ImGui::BeginTooltip();
                        ImGui::Text("Total Spent: $%.0f", s.totalSpent);
                        ImGui::Text("Total Reach: %.0f people", s.totalReach);
                        ImGui::Text("Completed:   %d campaigns", s.completed);
                        ImGui::EndTooltip();
                    }

                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextColored(UIStyle::Positive, "$%.0f", s.totalRevenue);

                    ImGui::TableSetColumnIndex(2);
                    ImVec4 roiColor = roi >= 0 ? UIStyle::Positive : UIStyle::Negative;
                    ImGui::TextColored(roiColor, "%.1f%%", roi);
                    if (ImGui::IsItemHovered()) {
                        ImGui::BeginTooltip();
                        ImGui::Text("ROI = (Revenue / Spend - 1) x 100");
                        ImGui::Text("Agency keeps 18%% of client revenue");
                        ImGui::EndTooltip();
                    }

                    ImGui::TableSetColumnIndex(3);
                    ImGui::Text("%d", s.campaigns);

                    ImGui::TableSetColumnIndex(4);
                    char lbl[16]; snprintf(lbl, 16, "%.1f%%", share);
                    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, UIStyle::Accent);
                    ImGui::ProgressBar(share / 100.f, ImVec2(-1, 16), lbl);
                    ImGui::PopStyleColor();
                }
                ImGui::EndTable();
            }

            // Sparkline: revenue history
            UIStyle::SectionHeader("Monthly Revenue (last 36 months)");
            auto& hist = tracker.GetHistory();
            static float revBuf[36] = {};
            float maxVal = 1.f;
            for (int i = 0; i < (int)hist.size() && i < 36; i++) {
                revBuf[i] = hist[i].revenue;
                maxVal = std::max(maxVal, revBuf[i]);
            }
            ImGui::PushStyleColor(ImGuiCol_PlotLines, UIStyle::Positive);
            ImGui::PlotLines("##revhist", revBuf, (int)std::min((int)hist.size(), 36),
                0, nullptr, 0.f, maxVal * 1.1f, ImVec2(-1, 90));
            ImGui::PopStyleColor();

            // Tooltip on sparkline
            if (ImGui::IsItemHovered() && !hist.empty()) {
                int idx = (int)((ImGui::GetMousePos().x - ImGui::GetItemRectMin().x)
                    / ImGui::GetItemRectSize().x * hist.size());
                idx = std::clamp(idx, 0, (int)hist.size() - 1);
                ImGui::BeginTooltip();
                ImGui::Text("Month %d / %d", hist[idx].month, hist[idx].year);
                ImGui::Text("Revenue:  $%.0f", hist[idx].revenue);
                ImGui::Text("Expenses: $%.0f", hist[idx].expenses);
                ImGui::Text("Budget:   $%.0f", hist[idx].budget);
                ImGui::EndTooltip();
            }
            ImGui::EndTabItem();
        }

        // ── TAB 2: Industry Win Rate ──────────────────────────────────────
        if (ImGui::BeginTabItem("Industry Win Rate")) {
            UIStyle::SectionHeader("Pitch Performance by Industry");
            if (ImGui::BeginTable("##indwin", 5,
                ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                ImGui::TableSetupColumn("Industry", ImGuiTableColumnFlags_WidthFixed, 140);
                ImGui::TableSetupColumn("Pitches",  ImGuiTableColumnFlags_WidthFixed, 70);
                ImGui::TableSetupColumn("Won",      ImGuiTableColumnFlags_WidthFixed, 60);
                ImGui::TableSetupColumn("Win Rate", ImGuiTableColumnFlags_WidthFixed, 90);
                ImGui::TableSetupColumn("Avg Sat",  ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableHeadersRow();

                for (int i = 0; i < 8; i++) {
                    auto ind = (ClientIndustry)i;
                    auto& s  = tracker.GetIndustry(ind);
                    float wr = tracker.GetPitchWinRate(ind);

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("%s", IndustryNames[i]);
                    ImGui::TableSetColumnIndex(1); ImGui::Text("%d", s.pitches);
                    ImGui::TableSetColumnIndex(2);
                    ImGui::TextColored(UIStyle::Positive, "%d", s.wins);
                    ImGui::TableSetColumnIndex(3);
                    ImVec4 wrColor = wr >= 60 ? UIStyle::Positive
                                   : wr >= 35 ? UIStyle::Warning
                                   : UIStyle::Negative;
                    ImGui::TextColored(wrColor, "%.1f%%", wr);
                    if (ImGui::IsItemHovered()) {
                        ImGui::BeginTooltip();
                        ImGui::Text("Lost: %d pitches", s.losses);
                        ImGui::Text("Win rate improves with reputation and industry specialization.");
                        ImGui::EndTooltip();
                    }
                    ImGui::TableSetColumnIndex(4);
                    char sl[20];
                    snprintf(sl, 20, "%.0f%%", s.avgSatisfaction);
                    ImGui::PushStyleColor(ImGuiCol_PlotHistogram,
                        s.avgSatisfaction >= 70 ? UIStyle::Positive
                        : s.avgSatisfaction >= 45 ? UIStyle::Warning
                        : UIStyle::Negative);
                    ImGui::ProgressBar(s.avgSatisfaction / 100.f, ImVec2(-1, 16), sl);
                    ImGui::PopStyleColor();
                }
                ImGui::EndTable();
            }
            ImGui::EndTabItem();
        }

        // ── TAB 3: Market Share Trend ─────────────────────────────────────
        if (ImGui::BeginTabItem("Market Trend")) {
            UIStyle::SectionHeader("Market Share History");
            auto& hist = tracker.GetHistory();
            static float msBuf[36] = {};
            for (int i = 0; i < (int)hist.size() && i < 36; i++)
                msBuf[i] = hist[i].marketShare;
            ImGui::PushStyleColor(ImGuiCol_PlotLines, UIStyle::Gold);
            ImGui::PlotLines("##ms", msBuf, (int)std::min((int)hist.size(), 36),
                0, nullptr, 0.f, 40.f, ImVec2(-1, 90));
            ImGui::PopStyleColor();
            // Goal line indicator
            ImGui::TextColored(UIStyle::Gold, "  Target: 35%% market share to win");

            // Monthly snapshot table
            UIStyle::SectionHeader("Monthly Snapshots");
            if (ImGui::BeginTable("##snaps", 6,
                ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY,
                ImVec2(0, 280))) {
                ImGui::TableSetupScrollFreeze(0, 1);
                ImGui::TableSetupColumn("Period");
                ImGui::TableSetupColumn("Revenue");
                ImGui::TableSetupColumn("Expenses");
                ImGui::TableSetupColumn("Budget");
                ImGui::TableSetupColumn("Share");
                ImGui::TableSetupColumn("Clients");
                ImGui::TableHeadersRow();

                for (int i = (int)hist.size() - 1; i >= 0; i--) {
                    auto& h = hist[i];
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    char period[12]; snprintf(period, 12, "Mo%d/%d", h.month, h.year);
                    ImGui::Text("%s", period);
                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextColored(UIStyle::Positive, "$%.0f", h.revenue);
                    ImGui::TableSetColumnIndex(2);
                    ImGui::TextColored(UIStyle::Negative, "$%.0f", h.expenses);
                    ImGui::TableSetColumnIndex(3);
                    ImVec4 bc = h.budget >= 0 ? UIStyle::Positive : UIStyle::Negative;
                    ImGui::TextColored(bc, "$%.0f", h.budget);
                    ImGui::TableSetColumnIndex(4);
                    ImGui::Text("%.1f%%", h.marketShare);
                    ImGui::TableSetColumnIndex(5);
                    ImGui::Text("%d", h.activeClients);
                }
                ImGui::EndTable();
            }
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
    ImGui::End();
}
