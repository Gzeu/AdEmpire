#pragma once
#include "imgui.h"

// Central place for all UI colors and style constants
namespace UIStyle {
    // ---- Brand colors ----
    inline constexpr ImVec4 Accent      = {0.25f, 0.55f, 1.00f, 1.f};
    inline constexpr ImVec4 AccentHover = {0.35f, 0.70f, 1.00f, 1.f};
    inline constexpr ImVec4 Positive    = {0.20f, 0.85f, 0.40f, 1.f};
    inline constexpr ImVec4 Negative    = {0.95f, 0.28f, 0.28f, 1.f};
    inline constexpr ImVec4 Warning     = {0.95f, 0.75f, 0.10f, 1.f};
    inline constexpr ImVec4 Muted       = {0.55f, 0.60f, 0.70f, 1.f};
    inline constexpr ImVec4 Gold        = {1.00f, 0.80f, 0.10f, 1.f};
    inline constexpr ImVec4 TextPrimary = {0.95f, 0.95f, 1.00f, 1.f};

    // ---- KPI card sizes ----
    inline constexpr float KPI_W = 145.f;
    inline constexpr float KPI_H = 72.f;

    // Draw a KPI card (title + large value + subtitle)
    inline void KpiCard(const char* title, const char* value,
                        const char* sub = nullptr,
                        ImVec4 valueColor = {0.95f,0.95f,1.f,1.f}) {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.10f,0.13f,0.22f,1.f));
        ImGui::BeginChild(title, ImVec2(KPI_W, KPI_H), true);
        ImGui::TextColored(Muted, "%s", title);
        ImGui::SetWindowFontScale(1.35f);
        ImGui::TextColored(valueColor, "%s", value);
        ImGui::SetWindowFontScale(1.0f);
        if (sub) ImGui::TextColored(Muted, "%s", sub);
        ImGui::EndChild();
        ImGui::PopStyleColor();
    }

    // Highlighted primary action button
    inline bool PrimaryButton(const char* label, ImVec2 size = {0,0}) {
        ImGui::PushStyleColor(ImGuiCol_Button,        Accent);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, AccentHover);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.15f,0.40f,0.85f,1.f));
        bool r = ImGui::Button(label, size);
        ImGui::PopStyleColor(3);
        return r;
    }

    // Positive/green button
    inline bool GreenButton(const char* label, ImVec2 size = {0,0}) {
        ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.12f,0.55f,0.25f,1.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.18f,0.70f,0.35f,1.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.10f,0.45f,0.20f,1.f));
        bool r = ImGui::Button(label, size);
        ImGui::PopStyleColor(3);
        return r;
    }

    // Separator with label
    inline void SectionHeader(const char* label) {
        ImGui::Spacing();
        ImGui::TextColored(Accent, "%s", label);
        ImGui::Separator();
        ImGui::Spacing();
    }
}
