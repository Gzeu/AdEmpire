#include "Dashboard.h"
#include "imgui.h"
#include "ChartRenderer.h"
#include "LiveMarketPanel.h"
#include "AgentSuggestionsPanel.h"
#include "SidebarNav.h"
#include "RevenueMultiplierWidget.h"
#include "ContractsPanel.h"
#include "../core/GameState.h"
#include "../systems/StatsTracker.h"
#include "../systems/MarketEventBridge.h"
#include "ToastSystem.h"
#include "../network/MarketFeed.h"
#include <cstring>

// ── Static instances (persist across frames) ────────────────────────────────
static AgentSuggestionsPanel s_advisor;
static SidebarNav             s_nav;

void Dashboard::Render(GameState& gs) {
    ImGuiIO& io = ImGui::GetIO();

    // ── Flush pendingToasts from Simulation.cpp into ToastSystem ─────────────
    for (auto& t : gs.pendingToasts)
        ToastSystem::Get().Push(t);
    gs.pendingToasts.clear();

    // ── Sidebar (renders its own window, fixed left) ─────────────────────────
    s_nav.Render(gs);

    // ── Main dashboard window — offset by sidebar width ──────────────────────
    ImGui::SetNextWindowPos(ImVec2(SidebarNav::kWidth, 0));
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x - SidebarNav::kWidth, io.DisplaySize.y));
    ImGui::SetNextWindowBgAlpha(1.f);
    ImGui::Begin("Dashboard",
                 nullptr,
                 ImGuiWindowFlags_NoTitleBar |
                 ImGuiWindowFlags_NoResize   |
                 ImGuiWindowFlags_NoMove     |
                 ImGuiWindowFlags_NoBringToFrontOnFocus);

    // ── Header row: title + revenue multiplier chip ──────────────────────────
    ImGui::SetWindowFontScale(1.1f);
    ImGui::TextUnformatted("AdEmpire  Dashboard");
    ImGui::SetWindowFontScale(1.f);
    ImGui::SameLine();
    RevenueMultiplierWidget::Render(gs);
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // ── Tab bar ───────────────────────────────────────────────────────────────
    if (ImGui::BeginTabBar("##dash_tabs")) {

        // ════════════════════════════════════
        //  TAB 1 — Overview
        // ════════════════════════════════════
        if (ImGui::BeginTabItem(" Overview ")) {

            // KPI row
            float avail = ImGui::GetContentRegionAvail().x;
            float kpiW  = (avail - 24.f) / 4.f;

            auto KPI = [&](const char* label, const char* value, ImVec4 col) {
                ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.13f,0.13f,0.16f,1.f));
                ImGui::BeginChild(label, ImVec2(kpiW, 68), true);
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4.f);
                ImGui::PushStyleColor(ImGuiCol_Text, col);
                ImGui::SetWindowFontScale(1.4f);
                ImGui::TextUnformatted(value);
                ImGui::SetWindowFontScale(1.0f);
                ImGui::PopStyleColor();
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f,0.6f,0.6f,1.f));
                ImGui::TextUnformatted(label);
                ImGui::PopStyleColor();
                ImGui::EndChild();
                ImGui::PopStyleColor();
            };

            char bufBudget[32];  snprintf(bufBudget,  sizeof(bufBudget),  "$%.0f", gs.budget);
            char bufShare[32];   snprintf(bufShare,   sizeof(bufShare),   "%.1f%%", gs.playerMarketShare);
            char bufClients[16]; snprintf(bufClients, sizeof(bufClients), "%d", (int)gs.clients.size());
            char bufMonth[16];   snprintf(bufMonth,   sizeof(bufMonth),   "Month %d", gs.month);

            KPI("Budget",       bufBudget,  ImVec4(0.30f,0.85f,0.50f,1.f));
            ImGui::SameLine(0.f, 8.f);
            KPI("Market Share", bufShare,   ImVec4(0.40f,0.70f,1.00f,1.f));
            ImGui::SameLine(0.f, 8.f);
            KPI("Clients",      bufClients, ImVec4(1.00f,0.80f,0.20f,1.f));
            ImGui::SameLine(0.f, 8.f);
            KPI("Time",         bufMonth,   ImVec4(0.80f,0.55f,1.00f,1.f));

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            // Revenue history chart
            ImGui::TextUnformatted("Revenue History");
            ImGui::Spacing();
            // TODO: Implement revenue history tracking in v1.2
            ImGui::TextDisabled("Revenue history tracking coming in v1.2.");

            ImGui::Spacing();

            // Market share trend
            ImGui::TextUnformatted("Market Share Trend");
            ImGui::Spacing();
            // TODO: Implement market share history tracking in v1.2
            ImGui::TextDisabled("Market share history tracking coming in v1.2.");

            ImGui::EndTabItem();
        }

        // ════════════════════════════════════
        //  TAB 2 — Channels
        // ════════════════════════════════════
        if (ImGui::BeginTabItem(" Channels ")) {
            ImGui::Spacing();
            ImGui::TextUnformatted("Active campaign channels and their performance:");
            ImGui::Spacing();
            ImGui::Separator();

            struct ChanRow { const char* name; float share; float roi; ImVec4 col; };
            ChanRow channels[] = {
                { "Social Media",  0.34f, 2.1f, ImVec4(0.4f,0.7f,1.f,1.f) },
                { "Search Ads",    0.28f, 3.4f, ImVec4(0.3f,0.85f,0.5f,1.f) },
                { "Display",       0.18f, 1.2f, ImVec4(1.f,0.8f,0.2f,1.f) },
                { "Influencer",    0.12f, 4.0f, ImVec4(0.8f,0.5f,1.f,1.f) },
                { "Email",         0.08f, 5.1f, ImVec4(1.f,0.6f,0.3f,1.f) },
            };

            const MarketState& ms = MarketFeed::Get().GetState();
            float mult = MarketEventBridge::Get().GetRevenueMultiplier(ms);

            if (ImGui::BeginTable("##chantbl", 4,
                ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchSame)) {
                ImGui::TableSetupColumn("Channel",     ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Budget Share",ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("ROI (live)",  ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Trend",       ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableHeadersRow();

                for (auto& c : channels) {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::PushStyleColor(ImGuiCol_Text, c.col);
                    ImGui::TextUnformatted(c.name);
                    ImGui::PopStyleColor();

                    ImGui::TableNextColumn();
                    ImGui::ProgressBar(c.share, ImVec2(-1.f, 12.f));

                    ImGui::TableNextColumn();
                    float effectiveRoi = c.roi * mult;
                    ImGui::PushStyleColor(ImGuiCol_Text,
                        effectiveRoi >= c.roi ? ImVec4(0.3f,0.9f,0.5f,1.f)
                                              : ImVec4(0.9f,0.4f,0.3f,1.f));
                    ImGui::Text("x%.2f", effectiveRoi);
                    ImGui::PopStyleColor();

                    ImGui::TableNextColumn();
                    ImGui::TextDisabled(mult >= 1.f ? "+" : "-");
                }
                ImGui::EndTable();
            }

            ImGui::Spacing();
            ImGui::PushStyleColor(ImGuiCol_Text,
                mult >= 1.f ? ImVec4(0.3f,0.9f,0.5f,1.f) : ImVec4(0.9f,0.4f,0.3f,1.f));
            ImGui::Text("Market modifier active: x%.2f  (source: live data)", mult);
            ImGui::PopStyleColor();

            ImGui::EndTabItem();
        }

        // ════════════════════════════════════
        //  TAB 3 — Live Market
        // ════════════════════════════════════
        if (ImGui::BeginTabItem(" Live Market ")) {
            LiveMarketPanel::RenderLiveMarket();
            ImGui::EndTabItem();
        }

        // ════════════════════════════════════
        //  TAB 4 — AI Advisor
        // ════════════════════════════════════
        if (ImGui::BeginTabItem(" AI Advisor ")) {
            s_advisor.Render(gs, MarketFeed::Get().GetState());
            ImGui::EndTabItem();
        }

        // ════════════════════════════════════
        //  TAB 5 — Contracts
        // ════════════════════════════════════
        if (ImGui::BeginTabItem(" Contracts ")) {
            ContractsPanel::Draw(gs);
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
}
