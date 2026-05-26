#pragma once
// ── LeaderboardPanel v0.8 ────────────────────────────────────────────────────
// Two tabs: Local (existing v0.7) + Global (REST API, live fetch)
// ─────────────────────────────────────────────────────────────────────────────
#include "imgui.h"
#include "UIStyle.h"
#include "../core/GameState.h"
#include "../network/LeaderboardClient.h"
#include <vector>
#include <string>
#include <algorithm>

namespace LeaderboardPanel {

static std::string FormatNum(int n) {
    std::string s = std::to_string(n);
    int ins = (int)s.size() - 3;
    while (ins > 0) { s.insert(ins, ","); ins -= 3; }
    return s;
}

inline void Render(GameState& gs) {
    if (!gs.showLeaderboard) return;

    ImGui::SetNextWindowPos(ImVec2(200, 50), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(700, 500), ImGuiCond_FirstUseEver);
    ImGui::Begin("🏆 Leaderboard — v0.8", &gs.showLeaderboard,
                 ImGuiWindowFlags_NoCollapse);

    auto& lc = LeaderboardClient::Get();

    if (ImGui::BeginTabBar("LBTabs")) {

        // ── Tab 1: Local ───────────────────────────────────────────────────
        if (ImGui::BeginTabItem("🖥  Local Top-10")) {
            ImGui::Spacing();
            ImGui::TextColored(UIStyle::Gold, "Your best runs on this machine");
            ImGui::Separator();

            if (gs.leaderboard.empty()) {
                ImGui::Spacing();
                ImGui::TextColored(UIStyle::Muted, "No runs recorded yet. Win a game to appear here!");
            } else {
                // Sort by score desc
                auto sorted = gs.leaderboard;
                std::sort(sorted.begin(), sorted.end(),
                          [](const auto& a, const auto& b){ return a.score > b.score; });

                ImGui::Columns(4, "lb_local", true);
                ImGui::SetColumnWidth(0, 60);  ImGui::TextColored(UIStyle::Muted, "Rank");   ImGui::NextColumn();
                ImGui::SetColumnWidth(1, 200); ImGui::TextColored(UIStyle::Muted, "Name");   ImGui::NextColumn();
                ImGui::SetColumnWidth(2, 160); ImGui::TextColored(UIStyle::Muted, "Revenue");ImGui::NextColumn();
                ImGui::TextColored(UIStyle::Muted, "Months"); ImGui::NextColumn();
                ImGui::Separator();

                int rank = 1;
                for (const auto& e : sorted) {
                    ImVec4 col = (rank == 1) ? UIStyle::Gold
                               : (rank == 2) ? ImVec4(0.8f,0.8f,0.85f,1.f)
                               : (rank == 3) ? ImVec4(0.72f,0.45f,0.2f,1.f)
                               : UIStyle::TextPrimary;
                    ImGui::TextColored(col, "  #%d", rank); ImGui::NextColumn();
                    ImGui::TextColored(col, "%s", e.agencyName.c_str()); ImGui::NextColumn();
                    ImGui::TextColored(col, "$%s", FormatNum((int)e.revenue).c_str()); ImGui::NextColumn();
                    ImGui::TextColored(col, "%d mo", e.months); ImGui::NextColumn();
                    ++rank;
                    if (rank > 10) break;
                }
                ImGui::Columns(1);
            }
            ImGui::EndTabItem();
        }

        // ── Tab 2: Global ──────────────────────────────────────────────────
        if (ImGui::BeginTabItem("🌐 Global")) {
            ImGui::Spacing();
            ImGui::TextColored(UIStyle::Gold, "Top players worldwide");
            ImGui::SameLine(ImGui::GetContentRegionAvail().x - 160);

            if (lc.fetching.load()) {
                ImGui::TextColored(UIStyle::Muted, "⏳ Fetching...");
            } else {
                if (ImGui::Button("🔄 Refresh", ImVec2(140, 0)))
                    lc.FetchLeaderboard();
            }
            ImGui::Separator();

            // Show mock or live entries
            std::vector<LeaderEntry> display;
            {
                std::lock_guard<std::mutex> lk(lc.entriesMutex);
                display = lc.entries;
            }
            bool isMock = display.empty();
            if (isMock) display = LeaderboardClient::MockEntries();

            if (isMock) {
                ImGui::Spacing();
                ImGui::TextColored(UIStyle::Muted, "Showing demo data — press Refresh or submit a score to load live rankings.");
                ImGui::Spacing();
            } else if (!lc.lastError.empty() && !lc.fetchOK.load()) {
                ImGui::TextColored(UIStyle::Negative, "⚠  %s", lc.lastError.c_str());
            }

            ImGui::Columns(4, "lb_global", true);
            ImGui::SetColumnWidth(0, 60);  ImGui::TextColored(UIStyle::Muted, "Rank");   ImGui::NextColumn();
            ImGui::SetColumnWidth(1, 200); ImGui::TextColored(UIStyle::Muted, "Player"); ImGui::NextColumn();
            ImGui::SetColumnWidth(2, 160); ImGui::TextColored(UIStyle::Muted, "Revenue");ImGui::NextColumn();
            ImGui::TextColored(UIStyle::Muted, "Months"); ImGui::NextColumn();
            ImGui::Separator();

            for (const auto& e : display) {
                ImVec4 col = (e.rank == 1) ? UIStyle::Gold
                           : (e.rank == 2) ? ImVec4(0.8f,0.8f,0.85f,1.f)
                           : (e.rank == 3) ? ImVec4(0.72f,0.45f,0.2f,1.f)
                           : UIStyle::TextPrimary;
                ImGui::TextColored(col, "  #%d", e.rank); ImGui::NextColumn();
                ImGui::TextColored(col, "%s", e.name.c_str()); ImGui::NextColumn();
                ImGui::TextColored(col, "$%s", FormatNum(e.score).c_str()); ImGui::NextColumn();
                ImGui::TextColored(col, "%d mo", e.months); ImGui::NextColumn();
            }
            ImGui::Columns(1);
            ImGui::EndTabItem();
        }

        // ── Tab 3: Submit ──────────────────────────────────────────────────
        if (ImGui::BeginTabItem("📤 Submit Score")) {
            ImGui::Spacing();
            ImGui::TextColored(UIStyle::Gold, "Post your run to the global board");
            ImGui::Separator();
            ImGui::Spacing();

            static char nameInput[64] = "";
            ImGui::Text("Player name:");
            ImGui::SetNextItemWidth(280);
            ImGui::InputText("##pname", nameInput, sizeof(nameInput));
            ImGui::Spacing();
            ImGui::Text("Revenue to submit:  $%.0f", (float)gs.stats.totalRevenue);
            ImGui::Text("Months played:      %d",    gs.stats.monthsPlayed);
            ImGui::Spacing();

            auto& lc2 = LeaderboardClient::Get();
            if (lc2.submitting.load()) {
                ImGui::TextColored(UIStyle::Muted, "⏳ Submitting...");
            } else if (lc2.submitOK.load()) {
                ImGui::TextColored(UIStyle::Positive, "✅ Score submitted! Go to Global tab and refresh.");
            } else {
                bool canSubmit = strlen(nameInput) > 0 && gs.stats.totalRevenue > 0;
                if (!canSubmit) ImGui::BeginDisabled();
                if (UIStyle::GreenButton("Submit to Global Leaderboard", ImVec2(280, 36))) {
                    lc2.SubmitScore(nameInput, (int)gs.stats.totalRevenue, gs.stats.monthsPlayed,
                        [](bool ok){ (void)ok; });
                }
                if (!canSubmit) ImGui::EndDisabled();
                if (!canSubmit)
                    ImGui::TextColored(UIStyle::Muted, "Enter a name and play at least one month.");
                if (!lc2.lastError.empty() && !lc2.submitOK.load())
                    ImGui::TextColored(UIStyle::Negative, "Error: %s", lc2.lastError.c_str());
            }
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
    ImGui::End();
}

} // namespace LeaderboardPanel
