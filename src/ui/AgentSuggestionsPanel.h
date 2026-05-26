#pragma once
#include "imgui.h"
#include "../core/GameState.h"
#include "../network/MarketFeed.h"
#include "../agent/AgentInterface.h"
#include "ToastSystem.h"
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

// ============================================================
//  AgentSuggestionsPanel  —  AI Advisor  (v0.8)
//  Reads MarketState + GameState and renders 3 actionable
//  suggestions in a collapsible sidebar panel.
//
//  Usage:
//    AgentSuggestionsPanel advisor;
//    // in render loop:
//    advisor.Render(gameState, marketFeed.GetState());
// ============================================================

struct AgentSuggestion {
    std::string icon;       // UTF-8 emoji / symbol
    std::string title;
    std::string body;
    std::string action;     // short CTA label
    ImVec4      color;      // accent color for the left bar
    float       confidence; // 0–1  shown as thin progress bar
};

class AgentSuggestionsPanel {
public:
    // --------------------------------------------------------
    void Render(GameState& gs, const MarketState& ms) {
        _Refresh(gs, ms);

        ImGui::PushStyleColor(ImGuiCol_ChildBg,
            ImVec4(0.10f, 0.10f, 0.12f, 1.00f));
        ImGui::BeginChild("##advisor_panel",
            ImVec2(0, 0), false,
            ImGuiWindowFlags_NoScrollbar);

        // ── Header ──────────────────────────────────────────
        ImGui::PushStyleColor(ImGuiCol_Text,
            ImVec4(0.85f, 0.75f, 0.30f, 1.00f)); // gold
        ImGui::Text("  AI Advisor");
        ImGui::PopStyleColor();
        ImGui::SameLine(ImGui::GetContentRegionAvail().x - 56);
        ImGui::PushStyleColor(ImGuiCol_Text,
            ImVec4(0.50f, 0.50f, 0.55f, 1.00f));
        ImGui::Text("last: now");
        ImGui::PopStyleColor();
        ImGui::Separator();
        ImGui::Spacing();

        // ── Suggestions ─────────────────────────────────────
        for (size_t i = 0; i < _suggestions.size(); ++i) {
            _RenderCard(_suggestions[i], static_cast<int>(i));
            ImGui::Spacing();
        }

        // ── Empty state ─────────────────────────────────────
        if (_suggestions.empty()) {
            ImGui::TextDisabled("  No suggestions — market is stable.");
        }

        ImGui::EndChild();
        ImGui::PopStyleColor(); // ChildBg
    }

private:
    std::vector<AgentSuggestion> _suggestions;
    float _lastRefresh = -999.f;

    // --------------------------------------------------------
    //  Heuristic rule engine — runs every 30 s of game time
    // --------------------------------------------------------
    void _Refresh(const GameState& gs, const MarketState& ms) {
        float now = static_cast<float>(ImGui::GetTime());
        if (now - _lastRefresh < 30.f) return;
        _lastRefresh = now;
        _suggestions.clear();

        // Rule 1 — Fear & Greed extreme fear → cut paid search
        if (ms.fearGreedIndex < 25) {
            AgentSuggestion s;
            s.icon = "📉";
            s.title = "Cut Paid Search Budget";
            s.body = "Fear & Greed = " + _fmt(ms.fearGreedIndex, 0)
                + " (Extreme Fear). CPCs drop — pause\n"
                  "performance campaigns, hold brand spend.";
            s.action = "Open Campaign Editor";
            s.color = ImVec4(0.85f, 0.25f, 0.25f, 1.f);
            s.confidence = 1.0f - ms.fearGreedIndex / 100.f;
            _suggestions.push_back(s);
        }

        // Rule 2 — Greed spike → pitch crypto / fintech clients
        if (ms.fearGreedIndex > 72) {
            AgentSuggestion s;
            s.icon = "🚀";
            s.title = "Pitch Crypto / FinTech Clients";
            s.body = "Market Greed = " + _fmt(ms.fearGreedIndex, 0)
                + ". Appetite for risk is high — ideal\n"
                  "window to upsell performance packages.";
            s.action = "Open Client Manager";
            s.color = ImVec4(0.25f, 0.80f, 0.45f, 1.f);
            s.confidence = ms.fearGreedIndex / 100.f;
            _suggestions.push_back(s);
        }

        // Rule 3 — Strong EUR/USD → finance sector bonus
        if (ms.eurUsd > 1.12f) {
            AgentSuggestion s;
            s.icon = "💶";
            s.title = "EUR/USD Strong — Target Finance Sector";
            s.body = "EUR/USD = " + _fmt(ms.eurUsd, 4)
                + ". European finance clients\n"
                  "have bigger USD-equivalent budgets right now.";
            s.action = "View Market Map";
            s.color = ImVec4(0.30f, 0.60f, 0.95f, 1.f);
            s.confidence = std::min(1.f, (ms.eurUsd - 1.05f) / 0.15f);
            _suggestions.push_back(s);
        }

        // Rule 4 — revenueMultiplier < 0.8 → warn player
        if (gs.revenueMultiplier < 0.80f) {
            AgentSuggestion s;
            s.icon = "⚠️";
            s.title = "Revenue Debuffed — Check Active Events";
            s.body = "revenueMultiplier = " + _fmt(gs.revenueMultiplier, 2)
                + ". An active market event\n"
                  "is suppressing income. Review EventLog.";
            s.action = "Open Event Log";
            s.color = ImVec4(0.95f, 0.65f, 0.15f, 1.f);
            s.confidence = 1.0f - gs.revenueMultiplier;
            _suggestions.push_back(s);
        }

        // Rule 5 — market share near win threshold
        if (gs.playerMarketShare >= 0.30f && gs.playerMarketShare < 0.35f) {
            AgentSuggestion s;
            s.icon = "🏆";
            s.title = "Almost There — Final Push!";
            s.body = "Market share = " + _fmt(gs.playerMarketShare * 100.f, 1)
                + "%. Target is 35%.\n"
                  "Double down on top-performing campaigns.";
            s.action = "Open Dashboard";
            s.color = ImVec4(0.85f, 0.75f, 0.30f, 1.f);
            s.confidence = gs.playerMarketShare / 0.35f;
            _suggestions.push_back(s);
        }

        // Cap at 3 most important
        if (_suggestions.size() > 3)
            _suggestions.resize(3);
    }

    // --------------------------------------------------------
    void _RenderCard(const AgentSuggestion& s, int id) {
        ImGui::PushID(id);
        float w  = ImGui::GetContentRegionAvail().x;
        float h  = 72.f;

        // Colored left accent bar
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImGui::GetWindowDrawList()->AddRectFilled(
            p,
            ImVec2(p.x + 3.f, p.y + h),
            ImGui::ColorConvertFloat4ToU32(s.color),
            2.f);

        // Card background
        ImGui::PushStyleColor(ImGuiCol_ChildBg,
            ImVec4(0.14f, 0.14f, 0.17f, 1.f));
        ImGui::SetCursorScreenPos(ImVec2(p.x + 6.f, p.y));
        ImGui::BeginChild(("##card" + std::to_string(id)).c_str(),
            ImVec2(w - 6.f, h), false);

        ImGui::Spacing();
        // Title row
        ImGui::Text(" %s  %s", s.icon.c_str(), s.title.c_str());
        // Body
        ImGui::PushStyleColor(ImGuiCol_Text,
            ImVec4(0.65f, 0.65f, 0.70f, 1.f));
        ImGui::TextWrapped(" %s", s.body.c_str());
        ImGui::PopStyleColor();

        // Confidence bar (thin, bottom of card)
        ImVec2 barPos = ImGui::GetCursorScreenPos();
        float barW    = (w - 12.f) * s.confidence;
        ImGui::GetWindowDrawList()->AddRectFilled(
            ImVec2(barPos.x, barPos.y),
            ImVec2(barPos.x + barW, barPos.y + 2.f),
            ImGui::ColorConvertFloat4ToU32(
                ImVec4(s.color.x, s.color.y, s.color.z, 0.55f)));

        ImGui::EndChild();
        ImGui::PopStyleColor(); // ChildBg

        // Hover → show CTA tooltip
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::PushStyleColor(ImGuiCol_Text,
                ImVec4(0.85f, 0.85f, 0.90f, 1.f));
            ImGui::Text("%s", s.action.c_str());
            ImGui::PopStyleColor();
            ImGui::EndTooltip();
        }

        ImGui::PopID();
    }

    // --------------------------------------------------------
    static std::string _fmt(float v, int decimals) {
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(decimals) << v;
        return ss.str();
    }
};
