#pragma once
#include "imgui.h"
#include "../core/GameState.h"
#include "../systems/LeaderboardPersistence.h"
#include "UIStyle.h"
#include <string>
#include <algorithm>

// ============================================================
//  EndGameSummary v1.1
//  Full-screen end-game screen: win / lose
//  Score formula:
//    revenue * 0.001 + peakShare * 50000 + campaigns * 120
//    + contracts * 250 + achievements * 500
// ============================================================

namespace EndGameSummary {

inline long long CalcScore(const GameState& gs) {
    int achievementCount = 0;
    for (const auto& a : gs.achievements)
        if (a.unlocked) achievementCount++;

    return (long long)(
        gs.stats.totalRevenue        * 0.001f
        + gs.playerMarketShare       * 50000.f
        + gs.stats.campaignsCompleted * 120
        + gs.stats.clientsAcquired   * 250
        + achievementCount           * 500
    );
}

inline void Render(GameState& gs) {
    bool isWin = gs.victory;

    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(io.DisplaySize);

    // Background gradient via overlay rect
    ImDrawList* bg = ImGui::GetBackgroundDrawList();
    ImU32 topCol = isWin
        ? IM_COL32(10, 40, 20, 240)
        : IM_COL32(40, 10, 10, 240);
    ImU32 botCol = IM_COL32(8, 8, 12, 255);
    bg->AddRectFilledMultiColor(
        ImVec2(0, 0), io.DisplaySize,
        topCol, topCol, botCol, botCol);

    ImGui::Begin("##endgame", nullptr,
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus);

    float cx = io.DisplaySize.x * 0.5f;
    float cy = 80.f;

    // ── Headline ──────────────────────────────────────────────────────────
    ImGui::SetWindowFontScale(2.6f);
    ImVec4 headlineCol = isWin
        ? ImVec4(0.2f, 1.0f, 0.5f, 1.f)
        : ImVec4(1.0f, 0.25f, 0.2f, 1.f);
    const char* headline = isWin ? "VICTORY — MARKET LEADER" : "BANKRUPT — GAME OVER";
    float tw = ImGui::CalcTextSize(headline).x;
    ImGui::SetCursorPos(ImVec2((io.DisplaySize.x - tw) * 0.5f, cy));
    ImGui::TextColored(headlineCol, "%s", headline);
    ImGui::SetWindowFontScale(1.0f);

    // Agency name
    ImGui::SetCursorPosY(cy + 70);
    float aw = ImGui::CalcTextSize(gs.agencyName.c_str()).x * 1.2f;
    ImGui::SetCursorPosX((io.DisplaySize.x - aw) * 0.5f);
    ImGui::SetWindowFontScale(1.2f);
    ImGui::TextColored(UIStyle::Gold, "%s", gs.agencyName.c_str());
    ImGui::SetWindowFontScale(1.0f);

    // ── Stats grid (4 columns × 2 rows) ──────────────────────────────────
    float gridY = cy + 140;
    ImGui::SetCursorPos(ImVec2(80, gridY));

    struct StatCard { const char* label; std::string value; ImVec4 color; };

    int achievementCount = 0;
    for (const auto& a : gs.achievements)
        if (a.unlocked) achievementCount++;

    int peakStaffLevel = 0;
    for (const auto& s : gs.staff)
        peakStaffLevel = std::max(peakStaffLevel, s.level);

    StatCard cards[] = {
        { "Market Share Peak",  std::to_string((int)gs.playerMarketShare) + "%",    ImVec4(0.3f,0.9f,0.5f,1) },
        { "Total Revenue",      "$" + std::to_string((int)gs.stats.totalRevenue),   ImVec4(1.0f,0.8f,0.2f,1) },
        { "Best Month",         "$" + std::to_string((int)gs.stats.bestMonthRevenue), ImVec4(0.9f,0.6f,0.2f,1) },
        { "Campaigns Run",      std::to_string(gs.stats.campaignsCompleted),         ImVec4(0.4f,0.8f,1.0f,1) },
        { "Clients Acquired",   std::to_string(gs.stats.clientsAcquired),            ImVec4(0.8f,0.5f,1.0f,1) },
        { "Achievements",       std::to_string(achievementCount) + " / 15",          ImVec4(1.0f,0.9f,0.3f,1) },
        { "Market Bull Ticks",  std::to_string(gs.stats.marketBonusTicks),           ImVec4(0.2f,1.0f,0.4f,1) },
        { "Best Staff Level",   LevelNames[std::min(peakStaffLevel, 4)],             ImVec4(0.7f,0.7f,0.9f,1) },
    };

    float cardW = 240.f, cardH = 80.f, padX = 20.f, padY = 14.f;
    float startX = (io.DisplaySize.x - (4 * cardW + 3 * padX)) * 0.5f;

    for (int i = 0; i < 8; i++) {
        int col = i % 4, row = i / 4;
        float x = startX + col * (cardW + padX);
        float y = gridY + row * (cardH + padY);
        ImGui::SetCursorPos(ImVec2(x, y));
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.12f, 0.13f, 0.18f, 1.f));
        ImGui::BeginChild(("##sc" + std::to_string(i)).c_str(), ImVec2(cardW, cardH), true);
        ImGui::SetCursorPosY(8);
        ImGui::TextColored(ImVec4(0.55f, 0.55f, 0.65f, 1.f), "%s", cards[i].label);
        ImGui::SetWindowFontScale(1.35f);
        ImGui::TextColored(cards[i].color, "%s", cards[i].value.c_str());
        ImGui::SetWindowFontScale(1.0f);
        ImGui::EndChild();
        ImGui::PopStyleColor();
    }

    // ── Final Score ───────────────────────────────────────────────────────
    long long score = CalcScore(gs);
    float scoreY = gridY + 2 * (cardH + padY) + 24;
    ImGui::SetCursorPosY(scoreY);
    ImGui::SetWindowFontScale(1.6f);
    std::string scoreStr = "FINAL SCORE:  " + std::to_string(score);
    float sw = ImGui::CalcTextSize(scoreStr.c_str()).x;
    ImGui::SetCursorPosX((io.DisplaySize.x - sw) * 0.5f);
    ImGui::TextColored(UIStyle::Gold, "%s", scoreStr.c_str());
    ImGui::SetWindowFontScale(1.0f);

    // ── Buttons ───────────────────────────────────────────────────────────
    float btnY = scoreY + 60;
    float btnW = 200.f;
    float btnTotalW = btnW * 2 + 20;
    float btnX = (io.DisplaySize.x - btnTotalW) * 0.5f;

    ImGui::SetCursorPos(ImVec2(btnX, btnY));
    if (ImGui::Button("Save to Leaderboard", ImVec2(btnW, 42))) {
        LeaderboardEntry entry;
        entry.agencyName  = gs.agencyName;
        entry.score       = score;
        entry.marketShare = gs.playerMarketShare;
        entry.revenue     = gs.stats.totalRevenue;
        entry.months      = gs.stats.monthsPlayed;
        entry.difficulty  = "Normal";
        LeaderboardPersistence::Get().AddEntry(entry);
        TOAST_SUCCESS("Score saved to leaderboard!");
    }

    ImGui::SameLine(0, 20);
    ImGui::SetCursorPosX(btnX + btnW + 20);
    ImGui::SetCursorPosY(btnY);
    if (ImGui::Button("Return to Main Menu", ImVec2(btnW, 42))) {
        gs = GameState();
        gs.showEndGame = false;
        extern bool gameStarted;
        gameStarted = false;
    }

    ImGui::End();
}

} // namespace EndGameSummary
