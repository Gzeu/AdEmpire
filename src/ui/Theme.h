#pragma once
#include "imgui.h"

// ─────────────────────────────────────────────────────────────────────────────
// AdEmpire Theme  v1.0
// Features:
//   • Roboto-Medium 16px embedded font (loaded from assets/fonts/Roboto-Medium.ttf)
//   • Docking mode enabled  (ImGuiConfigFlags_DockingEnable)
//   • Refined dark-marketing palette with accent orange highlights
//   • Helper: PushAccent / PopAccent for one-off accent-colored buttons
// ─────────────────────────────────────────────────────────────────────────────

namespace Theme {

    // Call once after ImGui::CreateContext() and before the render loop.
    // Returns the loaded Roboto font pointer (use for large headings if needed).
    inline ImFont* Init() {
        ImGuiIO& io = ImGui::GetIO();

        // ── Docking ────────────────────────────────────────────────────────────
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        // Optional: detach panels into separate OS windows
        // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        io.ConfigDockingWithShift = false;   // drag any panel to dock freely
        io.ConfigDockingAlwaysTabBar = false;

        // ── Font ──────────────────────────────────────────────────────────────
        ImFont* roboto = nullptr;
        ImFontConfig fontCfg;
        fontCfg.OversampleH = 3;
        fontCfg.OversampleV = 2;
        fontCfg.PixelSnapH  = false;

        // Primary: Roboto-Medium 16px
        // Place Roboto-Medium.ttf in assets/fonts/ (auto-fetched by CMake).
        // Fallback: if file missing, ImGui uses its built-in ProggyClean.
        roboto = io.Fonts->AddFontFromFileTTF(
            "assets/fonts/Roboto-Medium.ttf", 16.0f, &fontCfg);
        if (!roboto) {
            roboto = io.Fonts->AddFontDefault(); // graceful fallback
        }

        // Secondary: same font at 13px for dense labels (optional)
        fontCfg.SizePixels = 13.0f;
        // io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto-Medium.ttf", 13.0f, &fontCfg);

        io.FontGlobalScale = 1.0f;
        io.Fonts->Build();

        // ── Style ─────────────────────────────────────────────────────────────
        ApplyDarkMarketing();

        return roboto;
    }

    inline void ApplyDarkMarketing() {
        ImGuiStyle& s = ImGui::GetStyle();
        ImGui::StyleColorsDark();

        // Geometry
        s.WindowRounding      = 8.f;
        s.ChildRounding       = 6.f;
        s.FrameRounding       = 5.f;
        s.PopupRounding       = 6.f;
        s.ScrollbarRounding   = 6.f;
        s.GrabRounding        = 4.f;
        s.TabRounding         = 6.f;
        s.WindowBorderSize    = 1.f;
        s.FrameBorderSize     = 0.f;
        s.FramePadding        = ImVec2(10, 6);
        s.ItemSpacing         = ImVec2(10, 6);
        s.ItemInnerSpacing    = ImVec2(6, 4);
        s.WindowPadding       = ImVec2(14, 12);
        s.IndentSpacing       = 18.f;
        s.ScrollbarSize       = 12.f;
        s.GrabMinSize         = 10.f;

        // Docking tab bar
        s.TabBarBorderSize    = 1.f;
        s.DockingSeparatorSize = 3.f;

        auto* c = s.Colors;

        // ── Backgrounds ──────────────────────────────────────────────────────
        c[ImGuiCol_WindowBg]            = ImVec4(0.07f, 0.08f, 0.12f, 0.97f);
        c[ImGuiCol_ChildBg]             = ImVec4(0.04f, 0.05f, 0.09f, 0.90f);
        c[ImGuiCol_PopupBg]             = ImVec4(0.08f, 0.09f, 0.14f, 0.98f);
        c[ImGuiCol_Border]              = ImVec4(0.20f, 0.25f, 0.45f, 0.50f);
        c[ImGuiCol_BorderShadow]        = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

        // ── Frame / Input ────────────────────────────────────────────────────
        c[ImGuiCol_FrameBg]             = ImVec4(0.12f, 0.14f, 0.22f, 0.90f);
        c[ImGuiCol_FrameBgHovered]      = ImVec4(0.18f, 0.22f, 0.38f, 0.90f);
        c[ImGuiCol_FrameBgActive]       = ImVec4(0.22f, 0.28f, 0.50f, 1.00f);

        // ── Title bars ───────────────────────────────────────────────────────
        c[ImGuiCol_TitleBg]             = ImVec4(0.05f, 0.06f, 0.10f, 1.00f);
        c[ImGuiCol_TitleBgActive]       = ImVec4(0.10f, 0.20f, 0.55f, 1.00f);
        c[ImGuiCol_TitleBgCollapsed]    = ImVec4(0.05f, 0.06f, 0.10f, 0.80f);
        c[ImGuiCol_MenuBarBg]           = ImVec4(0.06f, 0.07f, 0.12f, 1.00f);

        // ── Scrollbar ────────────────────────────────────────────────────────
        c[ImGuiCol_ScrollbarBg]         = ImVec4(0.04f, 0.05f, 0.09f, 0.60f);
        c[ImGuiCol_ScrollbarGrab]       = ImVec4(0.20f, 0.30f, 0.55f, 0.80f);
        c[ImGuiCol_ScrollbarGrabHovered]= ImVec4(0.28f, 0.42f, 0.70f, 0.90f);
        c[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.35f, 0.52f, 0.85f, 1.00f);

        // ── Checkmark / Slider / Grab ────────────────────────────────────────
        c[ImGuiCol_CheckMark]           = ImVec4(0.30f, 0.70f, 0.95f, 1.00f);
        c[ImGuiCol_SliderGrab]          = ImVec4(0.25f, 0.50f, 0.90f, 1.00f);
        c[ImGuiCol_SliderGrabActive]    = ImVec4(0.35f, 0.62f, 1.00f, 1.00f);

        // ── Buttons ──────────────────────────────────────────────────────────
        c[ImGuiCol_Button]              = ImVec4(0.15f, 0.30f, 0.65f, 1.00f);
        c[ImGuiCol_ButtonHovered]       = ImVec4(0.25f, 0.45f, 0.80f, 1.00f);
        c[ImGuiCol_ButtonActive]        = ImVec4(0.30f, 0.55f, 0.95f, 1.00f);

        // ── Headers ──────────────────────────────────────────────────────────
        c[ImGuiCol_Header]              = ImVec4(0.15f, 0.25f, 0.55f, 0.80f);
        c[ImGuiCol_HeaderHovered]       = ImVec4(0.20f, 0.35f, 0.70f, 0.90f);
        c[ImGuiCol_HeaderActive]        = ImVec4(0.25f, 0.45f, 0.85f, 1.00f);

        // ── Separator ────────────────────────────────────────────────────────
        c[ImGuiCol_Separator]           = ImVec4(0.20f, 0.25f, 0.45f, 0.70f);
        c[ImGuiCol_SeparatorHovered]    = ImVec4(0.30f, 0.45f, 0.75f, 0.90f);
        c[ImGuiCol_SeparatorActive]     = ImVec4(0.40f, 0.60f, 0.90f, 1.00f);

        // ── Resize grip ──────────────────────────────────────────────────────
        c[ImGuiCol_ResizeGrip]          = ImVec4(0.20f, 0.35f, 0.65f, 0.40f);
        c[ImGuiCol_ResizeGripHovered]   = ImVec4(0.28f, 0.48f, 0.80f, 0.70f);
        c[ImGuiCol_ResizeGripActive]    = ImVec4(0.35f, 0.58f, 0.95f, 0.95f);

        // ── Tabs ─────────────────────────────────────────────────────────────
        c[ImGuiCol_Tab]                 = ImVec4(0.10f, 0.18f, 0.40f, 0.90f);
        c[ImGuiCol_TabHovered]          = ImVec4(0.20f, 0.38f, 0.70f, 0.90f);
        c[ImGuiCol_TabActive]           = ImVec4(0.18f, 0.35f, 0.75f, 1.00f);
        c[ImGuiCol_TabUnfocused]        = ImVec4(0.08f, 0.13f, 0.30f, 0.90f);
        c[ImGuiCol_TabUnfocusedActive]  = ImVec4(0.13f, 0.25f, 0.55f, 1.00f);

        // ── Docking ──────────────────────────────────────────────────────────
        c[ImGuiCol_DockingPreview]      = ImVec4(0.20f, 0.45f, 0.90f, 0.70f);
        c[ImGuiCol_DockingEmptyBg]      = ImVec4(0.04f, 0.05f, 0.09f, 1.00f);

        // ── Plot ─────────────────────────────────────────────────────────────
        c[ImGuiCol_PlotLines]           = ImVec4(0.30f, 0.80f, 0.50f, 1.00f);
        c[ImGuiCol_PlotLinesHovered]    = ImVec4(0.50f, 1.00f, 0.70f, 1.00f);
        c[ImGuiCol_PlotHistogram]       = ImVec4(0.20f, 0.60f, 0.90f, 1.00f);
        c[ImGuiCol_PlotHistogramHovered]= ImVec4(0.35f, 0.78f, 1.00f, 1.00f);

        // ── Text / Selection ─────────────────────────────────────────────────
        c[ImGuiCol_TextSelectedBg]      = ImVec4(0.20f, 0.45f, 0.80f, 0.60f);
        c[ImGuiCol_NavHighlight]        = ImVec4(0.30f, 0.55f, 0.90f, 1.00f);
        c[ImGuiCol_NavWindowingHighlight]= ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        c[ImGuiCol_NavWindowingDimBg]   = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        c[ImGuiCol_ModalWindowDimBg]    = ImVec4(0.05f, 0.05f, 0.10f, 0.60f);
    }

    // ── Accent helpers ────────────────────────────────────────────────────────
    // Use for "Pitch Client", "Launch Campaign" CTA buttons
    inline void PushAccentButton() {
        ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.80f, 0.42f, 0.00f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.95f, 0.55f, 0.10f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(1.00f, 0.65f, 0.20f, 1.00f));
    }
    inline void PopAccentButton() { ImGui::PopStyleColor(3); }

    // Use for "Warning / Low Budget" labels
    inline void PushWarningText() {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.00f, 0.55f, 0.15f, 1.00f));
    }
    inline void PopWarningText() { ImGui::PopStyleColor(1); }

    // Use for "Goal Reached / Profit" labels
    inline void PushSuccessText() {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.35f, 0.90f, 0.45f, 1.00f));
    }
    inline void PopSuccessText() { ImGui::PopStyleColor(1); }

} // namespace Theme
