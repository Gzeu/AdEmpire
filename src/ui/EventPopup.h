#pragma once
#include "imgui.h"
#include "../core/GameState.h"
#include "ToastSystem.h"
#include <string>
#include <unordered_set>

// ============================================================
//  EventPopup v1.1
//  CRITICAL events  → fullscreen modal overlay (blocking)
//  Normal events    → toast (non-blocking, unchanged)
//
//  Critical event IDs:
//    crypto_winter, market_panic, economic_downturn,
//    regulatory_crackdown, black_swan
// ============================================================

class EventPopup {
public:
    void Show(const GameEvent& evt) {
        pendingEvent_ = evt;
        hasPending_   = true;
    }

    // Returns true while modal is visible
    bool Render(GameState& gs, ToastSystem& /*toasts*/) {
        if (!hasPending_ && !isOpen_) return false;

        if (hasPending_) {
            currentEvent_ = pendingEvent_;
            hasPending_   = false;
            if (IsCritical(currentEvent_.id)) {
                isOpen_ = true;
            } else {
                // Non-critical: fire as toast and skip modal
                TOAST_WARN(currentEvent_.title + ": " + currentEvent_.description);
                return false;
            }
        }

        if (!isOpen_) return false;

        // ── Full-screen dark overlay ───────────────────────────────────────
        ImGuiIO& io = ImGui::GetIO();
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(io.DisplaySize);
        ImGui::SetNextWindowBgAlpha(0.72f);
        ImGui::Begin("##event_overlay", nullptr,
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNav);
        ImGui::End();

        // ── Modal window ──────────────────────────────────────────────────
        ImVec2 center = io.DisplaySize;
        center.x *= 0.5f; center.y *= 0.5f;
        ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(580, 370));
        ImGui::SetNextWindowBgAlpha(0.97f);
        ImGui::Begin("##event_modal", nullptr,
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNav);

        // ── Colored header bar ────────────────────────────────────────────
        ImVec4 hdrColor = GetEventColor(currentEvent_.id);
        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImVec2 wp = ImGui::GetWindowPos();
        dl->AddRectFilled(wp, ImVec2(wp.x + 580, wp.y + 52), ImGui::ColorConvertFloat4ToU32(hdrColor));

        ImGui::SetCursorPos(ImVec2(16, 12));
        ImGui::SetWindowFontScale(1.45f);
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "%s", currentEvent_.title.c_str());
        ImGui::SetWindowFontScale(1.0f);

        ImGui::SetCursorPosY(62);
        ImGui::PushTextWrapPos(ImGui::GetContentRegionAvail().x);
        ImGui::TextColored(ImVec4(0.88f, 0.88f, 0.88f, 1.f), "%s", currentEvent_.description.c_str());
        ImGui::PopTextWrapPos();

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // ── Effect box ────────────────────────────────────────────────────
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.14f, 1.f));
        ImGui::BeginChild("##eff_box", ImVec2(-1, 110), true);
        ImGui::TextColored(ImVec4(0.6f, 0.8f, 1.0f, 1.f), "MARKET IMPACT");
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Text("Effect:  %s", currentEvent_.impact.c_str());
        // Show live revenueMultiplier
        float rm = std::clamp(gs.revenueMultiplier, 0.3f, 2.0f);
        ImVec4 rmCol = rm >= 1.0f ? ImVec4(0.2f, 0.9f, 0.4f, 1.f)
                                  : ImVec4(0.9f, 0.3f, 0.3f, 1.f);
        ImGui::TextColored(rmCol, "Revenue Multiplier:  x%.2f  (%+.0f%%)",
            rm, (rm - 1.0f) * 100.f);
        ImGui::Text("Duration:  %d month(s)", currentEvent_.durationMonths);
        ImGui::EndChild();
        ImGui::PopStyleColor();

        ImGui::Spacing();

        // ── Acknowledge button ────────────────────────────────────────────
        ImGui::SetCursorPosX((580 - 180) * 0.5f);
        ImGui::PushStyleColor(ImGuiCol_Button,        hdrColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(hdrColor.x*1.15f, hdrColor.y*1.15f, hdrColor.z*1.15f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(hdrColor.x*0.85f, hdrColor.y*0.85f, hdrColor.z*0.85f, 1.f));
        if (ImGui::Button("  Acknowledge  ", ImVec2(180, 38))) {
            isOpen_ = false;
        }
        ImGui::PopStyleColor(3);

        ImGui::End();
        return isOpen_;
    }

private:
    GameEvent  currentEvent_;
    GameEvent  pendingEvent_;
    bool       hasPending_ = false;
    bool       isOpen_     = false;

    static const std::unordered_set<std::string>& CriticalIds() {
        static const std::unordered_set<std::string> ids = {
            "crypto_winter", "market_panic", "economic_downturn",
            "regulatory_crackdown", "black_swan"
        };
        return ids;
    }

    static bool IsCritical(const std::string& id) {
        return CriticalIds().count(id) > 0;
    }

    static ImVec4 GetEventColor(const std::string& id) {
        if (id == "crypto_winter")        return ImVec4(0.18f, 0.45f, 0.75f, 1.f); // icy blue
        if (id == "market_panic")         return ImVec4(0.78f, 0.18f, 0.12f, 1.f); // red-orange
        if (id == "economic_downturn")    return ImVec4(0.55f, 0.28f, 0.05f, 1.f); // dark amber
        if (id == "regulatory_crackdown") return ImVec4(0.45f, 0.10f, 0.50f, 1.f); // deep purple
        if (id == "black_swan")           return ImVec4(0.08f, 0.08f, 0.08f, 1.f); // near black
        if (id == "ai_hype_wave")         return ImVec4(0.10f, 0.60f, 0.30f, 1.f); // green
        return ImVec4(0.25f, 0.25f, 0.35f, 1.f); // default slate
    }
};
