#pragma once
#include "imgui.h"
#include "../core/GameState.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>

// ============================================================
//  RevenueMultiplierWidget  —  Dashboard header chip  (v0.8)
//  Shows the current revenueMultiplier with color coding
//  (green / yellow / red) and a tooltip explaining the cause.
//
//  Usage (inside Dashboard.cpp render, before BeginTabBar):
//    RevenueMultiplierWidget::Render(gameState);
//    ImGui::SameLine();
//    // ... rest of header ...
// ============================================================

struct RevenueMultiplierWidget {

    static void Render(const GameState& gs) {
        float m = gs.revenueMultiplier;

        // ── Color coding ─────────────────────────────────────
        ImVec4 col;
        const char* mood;
        if (m >= 1.20f)      { col = {0.20f,0.85f,0.45f,1.f}; mood = "Boosted";  }
        else if (m >= 1.00f) { col = {0.30f,0.75f,0.40f,1.f}; mood = "Normal";   }
        else if (m >= 0.80f) { col = {0.95f,0.75f,0.15f,1.f}; mood = "Reduced";  }
        else if (m >= 0.60f) { col = {0.95f,0.50f,0.10f,1.f}; mood = "Weakened"; }
        else                 { col = {0.90f,0.22f,0.20f,1.f}; mood = "Critical"; }

        // ── Format value string ──────────────────────────────
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(2) << m << "x";
        std::string valStr = ss.str();

        // ── Pill background ──────────────────────────────────
        ImVec2 pos   = ImGui::GetCursorScreenPos();
        float  tw    = ImGui::CalcTextSize(valStr.c_str()).x;
        float  moodW = ImGui::CalcTextSize(mood).x;
        float  pillW = tw + moodW + 32.f;
        float  pillH = 22.f;

        ImU32 bgCol = ImGui::ColorConvertFloat4ToU32(
            ImVec4(col.x*0.18f, col.y*0.18f, col.z*0.18f, 1.f));
        ImU32 fgCol = ImGui::ColorConvertFloat4ToU32(col);
        ImU32 borderCol = ImGui::ColorConvertFloat4ToU32(
            ImVec4(col.x, col.y, col.z, 0.45f));

        ImGui::GetWindowDrawList()->AddRectFilled(
            pos,
            ImVec2(pos.x + pillW, pos.y + pillH),
            bgCol, 5.f);
        ImGui::GetWindowDrawList()->AddRect(
            pos,
            ImVec2(pos.x + pillW, pos.y + pillH),
            borderCol, 5.f, 0, 1.2f);

        // ── Revenue icon ─────────────────────────────────────
        ImGui::GetWindowDrawList()->AddText(
            ImVec2(pos.x + 6.f, pos.y + 4.f),
            fgCol, "\xf0\x9f\x92\xb0"); // 💰 UTF-8

        // ── Multiplier value text ─────────────────────────────
        ImGui::GetWindowDrawList()->AddText(
            ImVec2(pos.x + 22.f, pos.y + 4.f),
            fgCol, valStr.c_str());

        // ── Mood label ───────────────────────────────────────
        ImGui::GetWindowDrawList()->AddText(
            ImVec2(pos.x + 22.f + tw + 4.f, pos.y + 4.f),
            ImGui::ColorConvertFloat4ToU32(
                ImVec4(col.x, col.y, col.z, 0.65f)),
            mood);

        // Invisible button so widget responds to hover / tooltip
        ImGui::InvisibleButton("##rmw", ImVec2(pillW, pillH));

        // ── Tooltip ──────────────────────────────────────────
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();

            ImGui::PushStyleColor(ImGuiCol_Text, col);
            ImGui::Text("Revenue Multiplier: %s", valStr.c_str());
            ImGui::PopStyleColor();

            ImGui::Separator();
            ImGui::PushStyleColor(ImGuiCol_Text,
                ImVec4(0.75f, 0.75f, 0.78f, 1.f));

            // Explain the value range to the player
            if (m >= 1.20f)
                ImGui::TextWrapped("Active bonus event boosting revenue"
                    " above baseline. Capitalize now!");
            else if (m >= 1.00f)
                ImGui::TextWrapped("Market conditions are normal.\n"
                    "No active multipliers.");
            else if (m >= 0.80f)
                ImGui::TextWrapped("A market event is reducing revenue."
                    "\nCheck the Event Log for details.");
            else if (m >= 0.60f)
                ImGui::TextWrapped("Significant revenue suppression active."
                    "\nPause costly campaigns until resolved.");
            else
                ImGui::TextWrapped("CRITICAL: Revenue near floor.\n"
                    "Resolve the active crisis event immediately.");

            ImGui::Spacing();
            ImGui::TextDisabled("Range: 0.30 (crisis) — 2.00 (boom)");
            ImGui::TextDisabled("Driven by: MarketEventBridge events");

            ImGui::PopStyleColor();
            ImGui::EndTooltip();
        }
    }
};
