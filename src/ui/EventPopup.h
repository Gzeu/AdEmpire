#pragma once
#include "imgui.h"
#include "../core/GameState.h"
#include "../systems/ToastSystem.h"
#include <string>
#include <unordered_set>

// ─── EventPopup ───────────────────────────────────────────────────────────────
// Modal blocker for CRITICAL / HIGH severity market events.
// Minor events → toast only.  Major events → fullscreen modal with Acknowledge.
// Usage: call EventPopup::Render(gs, toasts) once per frame in your main loop.
// ─────────────────────────────────────────────────────────────────────────────

struct EventPopupEntry {
    std::string id;           // event id e.g. "crypto_winter"
    std::string title;
    std::string description;
    std::string effect;       // human-readable effect
    ImVec4      color;        // accent color for header
    bool        acknowledged = false;
};

class EventPopup {
public:
    // Call once per frame — renders pending modal if any
    static void Render(GameState& gs, ToastSystem& toasts) {
        // Check pendingEventPopup from GameState
        if (!gs.pendingEventPopup) return;
        if (gs.currentEventId.empty()) return;

        // Build entry from currentEventId
        EventPopupEntry entry = BuildEntry(gs.currentEventId, gs.currentEvent);

        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(),
                                ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(520, 0), ImGuiCond_Always);
        ImGui::SetNextWindowBgAlpha(0.97f);

        // Darken background overlay
        ImDrawList* dl = ImGui::GetBackgroundDrawList();
        ImVec2 vp = ImGui::GetMainViewport()->Size;
        dl->AddRectFilled({0,0}, vp, IM_COL32(0,0,0,160));

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration
                               | ImGuiWindowFlags_NoMove
                               | ImGuiWindowFlags_NoSavedSettings
                               | ImGuiWindowFlags_NoBringToDisplayFront;

        if (ImGui::Begin("##EventModal", nullptr, flags)) {
            // Colored header bar
            ImDrawList* wdl = ImGui::GetWindowDrawList();
            ImVec2 wp = ImGui::GetWindowPos();
            ImVec2 ws = ImGui::GetWindowSize();
            wdl->AddRectFilled(wp, {wp.x + ws.x, wp.y + 52},
                IM_COL32((int)(entry.color.x*255),
                         (int)(entry.color.y*255),
                         (int)(entry.color.z*255), 220), 6.0f);

            // Severity badge
            std::string badge = IsCritical(gs.currentEventId) ? "  CRITICAL EVENT  " : "  MARKET EVENT  ";
            ImGui::SetCursorPosY(10);
            ImGui::SetCursorPosX((ws.x - ImGui::CalcTextSize(badge.c_str()).x) * 0.5f);
            ImGui::TextColored({1,1,1,1}, "%s", badge.c_str());

            ImGui::SetCursorPosY(58);
            ImGui::Separator();

            // Title
            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts.Size > 1 ?
                            ImGui::GetIO().Fonts->Fonts[1] : nullptr);
            float tw = ImGui::CalcTextSize(entry.title.c_str()).x;
            ImGui::SetCursorPosX((ws.x - tw) * 0.5f);
            ImGui::SetCursorPosY(70);
            ImGui::TextColored(entry.color, "%s", entry.title.c_str());
            if (ImGui::GetIO().Fonts->Fonts.Size > 1) ImGui::PopFont();

            ImGui::Spacing();
            ImGui::Spacing();

            // Description
            ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + ws.x - 40);
            ImGui::TextColored({0.85f,0.85f,0.85f,1.0f}, "%s", entry.description.c_str());
            ImGui::PopTextWrapPos();

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            // Effect box
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.12f,0.12f,0.15f,1.0f));
            ImGui::BeginChild("##effect", {ws.x - 32, 48}, true);
            ImGui::TextColored({0.6f,0.6f,0.6f,1.0f}, "Effect:");
            ImGui::SameLine();
            ImGui::TextColored(entry.color, "%s", entry.effect.c_str());
            ImGui::EndChild();
            ImGui::PopStyleColor();

            ImGui::Spacing();
            ImGui::Spacing();

            // revenueMultiplier live preview
            float rm = gs.revenueMultiplier;
            ImVec4 rmColor = rm >= 1.0f ? ImVec4(0.3f,0.9f,0.4f,1.0f) : ImVec4(0.9f,0.3f,0.3f,1.0f);
            ImGui::TextColored({0.6f,0.6f,0.6f,1.0f}, "Revenue multiplier now:");
            ImGui::SameLine();
            ImGui::TextColored(rmColor, "x%.2f", rm);

            ImGui::Spacing();
            ImGui::Spacing();

            // Acknowledge button
            float bw = 180.0f;
            ImGui::SetCursorPosX((ws.x - bw) * 0.5f);
            ImGui::PushStyleColor(ImGuiCol_Button,        entry.color);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(entry.color.x*1.1f, entry.color.y*1.1f, entry.color.z*1.1f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(entry.color.x*0.8f, entry.color.y*0.8f, entry.color.z*0.8f, 1.0f));
            if (ImGui::Button("  Acknowledge  ", {bw, 36})) {
                gs.pendingEventPopup = false;
                gs.currentEventId.clear();
                toasts.Push("Event acknowledged: " + entry.title, ToastType::Info);
            }
            ImGui::PopStyleColor(3);
            ImGui::Spacing();
        }
        ImGui::End();
    }

private:
    static bool IsCritical(const std::string& id) {
        static const std::unordered_set<std::string> critical = {
            "crypto_winter", "market_panic", "economic_downturn",
            "regulatory_crackdown", "black_swan"
        };
        return critical.count(id) > 0;
    }

    static EventPopupEntry BuildEntry(const std::string& id, const std::string& rawDesc) {
        EventPopupEntry e;
        e.id = id;

        // Color + title + effect by event id
        if (id == "crypto_winter") {
            e.title       = "Crypto Winter";
            e.description = rawDesc.empty() ? "A prolonged bear market has frozen the crypto sector. Client budgets tied to digital assets are slashed. Fear & Greed index below 20." : rawDesc;
            e.effect      = "Revenue x0.45 for crypto clients. Duration: 30-90 days";
            e.color       = {0.3f, 0.6f, 1.0f, 1.0f}; // icy blue
        } else if (id == "market_panic") {
            e.title       = "Market Panic";
            e.description = rawDesc.empty() ? "Mass sell-off triggered across all markets. Advertising budgets are the first to be cut." : rawDesc;
            e.effect      = "All campaign revenues x0.60 for 14 days";
            e.color       = {1.0f, 0.35f, 0.2f, 1.0f}; // red-orange
        } else if (id == "ai_hype_wave") {
            e.title       = "AI Hype Wave";
            e.description = rawDesc.empty() ? "AI dominates every headline. Clients in tech, media, and fintech are increasing ad spend significantly." : rawDesc;
            e.effect      = "Tech/Media campaigns x1.55 for 21 days";
            e.color       = {0.4f, 0.9f, 0.6f, 1.0f}; // green
        } else if (id == "economic_downturn") {
            e.title       = "Economic Downturn";
            e.description = rawDesc.empty() ? "GDP contraction confirmed. Consumer spending drops. All discretionary ad budgets under review." : rawDesc;
            e.effect      = "All revenues x0.70 for 45 days";
            e.color       = {0.9f, 0.7f, 0.2f, 1.0f}; // amber
        } else if (id == "strong_euro_bonus") {
            e.title       = "Strong Euro Bonus";
            e.description = rawDesc.empty() ? "EUR/USD above 1.15 for 5 consecutive days. European client budgets effectively larger." : rawDesc;
            e.effect      = "EU client revenues x1.20 for 14 days";
            e.color       = {0.5f, 0.8f, 1.0f, 1.0f};
        } else {
            // Generic fallback
            e.title       = id;
            e.description = rawDesc.empty() ? "A significant market event has occurred affecting your agency operations." : rawDesc;
            e.effect      = "See Live Market panel for details";
            e.color       = {0.7f, 0.7f, 0.7f, 1.0f};
        }
        return e;
    }
};
