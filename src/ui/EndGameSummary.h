#pragma once
#include "imgui.h"
#include "../core/GameState.h"
#include "../systems/StatsTracker.h"
#include "../systems/Leaderboard.h"
#include "LeaderboardPersistence.h"
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

// ─── EndGameSummary ───────────────────────────────────────────────────────────
// Full-screen end-game summary panel.
// Trigger: gs.gameOver == true || gs.marketShare >= WIN_THRESHOLD (35%)
// Call: EndGameSummary::Render(gs) once per frame when win/lose condition met.
// ─────────────────────────────────────────────────────────────────────────────

class EndGameSummary {
public:
    static constexpr float WIN_MARKET_SHARE = 0.35f;

    static void Render(GameState& gs) {
        bool won = gs.marketShare >= WIN_MARKET_SHARE;

        ImGui::SetNextWindowPos({0, 0});
        ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size);
        ImGui::SetNextWindowBgAlpha(0.96f);

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration
                               | ImGuiWindowFlags_NoMove
                               | ImGuiWindowFlags_NoSavedSettings;

        if (!ImGui::Begin("##EndGame", nullptr, flags)) { ImGui::End(); return; }

        ImVec2 ws = ImGui::GetWindowSize();

        // ── Background gradient ──
        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImVec2 wp = ImGui::GetWindowPos();
        ImU32 topCol    = won ? IM_COL32(10,40,20,245)  : IM_COL32(40,10,10,245);
        ImU32 bottomCol = won ? IM_COL32(5,20,40,245)   : IM_COL32(20,5,5,245);
        dl->AddRectFilledMultiColor(wp, {wp.x+ws.x, wp.y+ws.y},
            topCol, topCol, bottomCol, bottomCol);

        ImGui::SetCursorPosY(40);

        // ── Win / Lose title ──
        const char* headline = won ? "VICTORY — MARKET LEADER" : "GAME OVER";
        ImVec4 headColor = won ? ImVec4(0.3f,1.0f,0.5f,1.0f) : ImVec4(1.0f,0.3f,0.3f,1.0f);
        float hw = ImGui::CalcTextSize(headline).x;
        ImGui::SetCursorPosX((ws.x - hw) * 0.5f);
        ImGui::TextColored(headColor, "%s", headline);

        ImGui::Spacing();
        ImGui::Spacing();

        // ── Agency name + score ──
        float score = ComputeScore(gs);
        std::string scoreLine = gs.agencyName + "   |   Score: " + std::to_string((int)score);
        float sl = ImGui::CalcTextSize(scoreLine.c_str()).x;
        ImGui::SetCursorPosX((ws.x - sl) * 0.5f);
        ImGui::TextColored({0.9f,0.9f,0.9f,1.0f}, "%s", scoreLine.c_str());

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // ── Stats grid (2 columns) ──
        float colW = ws.x * 0.42f;
        float startX = (ws.x - colW * 2 - 20) * 0.5f;
        ImGui::SetCursorPosX(startX);

        if (ImGui::BeginTable("##stats", 2, ImGuiTableFlags_None, {colW*2+20, 0})) {
            ImGui::TableSetupColumn("Col1", ImGuiTableColumnFlags_WidthFixed, colW);
            ImGui::TableSetupColumn("Col2", ImGuiTableColumnFlags_WidthFixed, colW);

            // Row 1
            ImGui::TableNextRow(); ImGui::TableNextColumn();
            StatCard("Market Share Peak",
                std::to_string((int)(gs.stats.peakMarketShare * 100)) + "%",
                {0.3f,0.9f,0.5f,1.0f});
            ImGui::TableNextColumn();
            StatCard("Total Revenue",
                FormatMoney(gs.stats.totalRevenue),
                {0.9f,0.75f,0.2f,1.0f});

            // Row 2
            ImGui::TableNextRow(); ImGui::TableNextColumn();
            StatCard("Best Month Revenue",
                FormatMoney(gs.stats.bestMonthRevenue),
                {0.5f,0.8f,1.0f,1.0f});
            ImGui::TableNextColumn();
            StatCard("Campaigns Completed",
                std::to_string(gs.stats.campaignsCompleted),
                {0.9f,0.5f,0.9f,1.0f});

            // Row 3
            ImGui::TableNextRow(); ImGui::TableNextColumn();
            StatCard("Contracts Fulfilled",
                std::to_string(gs.stats.contractsFulfilled),
                {0.3f,0.9f,0.9f,1.0f});
            ImGui::TableNextColumn();
            StatCard("Achievements Unlocked",
                std::to_string(gs.stats.achievementsUnlocked) + " / 15",
                {1.0f,0.65f,0.2f,1.0f});

            // Row 4
            ImGui::TableNextRow(); ImGui::TableNextColumn();
            StatCard("Market Bonus Ticks",
                std::to_string(gs.stats.marketBonusTicks),
                {0.4f,1.0f,0.4f,1.0f});
            ImGui::TableNextColumn();
            StatCard("Staff Max Level",
                std::to_string(gs.stats.maxStaffLevel),
                {0.8f,0.8f,0.4f,1.0f});

            ImGui::EndTable();
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // ── Win condition display ──
        std::string winCond = won
            ? "Won by reaching " + std::to_string((int)(gs.marketShare*100)) + "% market share"
            : "Lost — market share dropped below 5% or agency bankrupt";
        float wl = ImGui::CalcTextSize(winCond.c_str()).x;
        ImGui::SetCursorPosX((ws.x - wl) * 0.5f);
        ImGui::TextColored(won ? ImVec4(0.5f,1.0f,0.6f,1.0f) : ImVec4(1.0f,0.4f,0.4f,1.0f),
            "%s", winCond.c_str());

        ImGui::Spacing();
        ImGui::Spacing();

        // ── Save to leaderboard button ──
        float bw = 220.0f;
        ImGui::SetCursorPosX((ws.x - bw * 2.0f - 20) * 0.5f);

        ImGui::PushStyleColor(ImGuiCol_Button, {0.2f,0.6f,0.3f,1.0f});
        if (ImGui::Button("  Save to Leaderboard  ", {bw, 38})) {
            LeaderboardPersistence::AddEntry(gs.agencyName, (int)score, won,
                gs.stats.peakMarketShare, gs.stats.totalRevenue);
        }
        ImGui::PopStyleColor();

        ImGui::SameLine(0, 20);

        ImGui::PushStyleColor(ImGuiCol_Button, {0.5f,0.3f,0.1f,1.0f});
        if (ImGui::Button("  Return to Main Menu  ", {bw, 38})) {
            gs.gameOver    = false;
            gs.showEndGame = false;
            gs.phase       = GamePhase::MainMenu;
        }
        ImGui::PopStyleColor();

        ImGui::End();
    }

private:
    static float ComputeScore(const GameState& gs) {
        float s = 0.0f;
        s += gs.stats.totalRevenue    * 0.001f;
        s += gs.stats.peakMarketShare * 50000.0f;
        s += gs.stats.campaignsCompleted * 120.0f;
        s += gs.stats.contractsFulfilled * 250.0f;
        s += gs.stats.achievementsUnlocked * 500.0f;
        s += gs.stats.marketBonusTicks * 2.0f;
        s -= gs.stats.marketPenaltyTicks * 1.0f;
        return std::max(0.0f, s);
    }

    static void StatCard(const char* label, const std::string& value, ImVec4 color) {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.08f,0.08f,0.10f,1.0f));
        ImGui::BeginChild(label, {0, 64}, true);
        ImGui::TextColored({0.55f,0.55f,0.55f,1.0f}, "%s", label);
        ImGui::TextColored(color, "%s", value.c_str());
        ImGui::EndChild();
        ImGui::PopStyleColor();
        ImGui::Spacing();
    }

    static std::string FormatMoney(float v) {
        if (v >= 1e6f) return std::to_string((int)(v/1e6f)) + "M$";
        if (v >= 1e3f) return std::to_string((int)(v/1e3f)) + "K$";
        return std::to_string((int)v) + "$";
    }
};
