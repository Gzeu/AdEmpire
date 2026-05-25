#pragma once
#include "imgui.h"

namespace Theme {
    inline void ApplyDarkMarketing() {
        ImGuiStyle& s = ImGui::GetStyle();
        ImGui::StyleColorsDark();
        s.WindowRounding    = 8.f;
        s.FrameRounding     = 5.f;
        s.ScrollbarRounding = 6.f;
        s.GrabRounding      = 4.f;
        s.TabRounding       = 6.f;
        s.WindowBorderSize  = 1.f;
        s.FramePadding      = ImVec2(10, 6);
        s.ItemSpacing       = ImVec2(10, 6);
        s.WindowPadding     = ImVec2(14, 12);
        auto* c = s.Colors;
        c[ImGuiCol_WindowBg]        = ImVec4(0.07f, 0.08f, 0.12f, 0.97f);
        c[ImGuiCol_ChildBg]         = ImVec4(0.04f, 0.05f, 0.09f, 0.90f);
        c[ImGuiCol_PopupBg]         = ImVec4(0.08f, 0.09f, 0.14f, 0.98f);
        c[ImGuiCol_Border]          = ImVec4(0.20f, 0.25f, 0.45f, 0.50f);
        c[ImGuiCol_FrameBg]         = ImVec4(0.12f, 0.14f, 0.22f, 0.90f);
        c[ImGuiCol_FrameBgHovered]  = ImVec4(0.18f, 0.22f, 0.38f, 0.90f);
        c[ImGuiCol_TitleBg]         = ImVec4(0.05f, 0.06f, 0.10f, 1.00f);
        c[ImGuiCol_TitleBgActive]   = ImVec4(0.10f, 0.20f, 0.55f, 1.00f);
        c[ImGuiCol_MenuBarBg]       = ImVec4(0.06f, 0.07f, 0.12f, 1.00f);
        c[ImGuiCol_Header]          = ImVec4(0.15f, 0.25f, 0.55f, 0.80f);
        c[ImGuiCol_HeaderHovered]   = ImVec4(0.20f, 0.35f, 0.70f, 0.90f);
        c[ImGuiCol_HeaderActive]    = ImVec4(0.25f, 0.45f, 0.85f, 1.00f);
        c[ImGuiCol_Button]          = ImVec4(0.15f, 0.30f, 0.65f, 1.00f);
        c[ImGuiCol_ButtonHovered]   = ImVec4(0.25f, 0.45f, 0.80f, 1.00f);
        c[ImGuiCol_ButtonActive]    = ImVec4(0.30f, 0.55f, 0.95f, 1.00f);
        c[ImGuiCol_Tab]             = ImVec4(0.10f, 0.18f, 0.40f, 0.90f);
        c[ImGuiCol_TabHovered]      = ImVec4(0.20f, 0.38f, 0.70f, 0.90f);
        c[ImGuiCol_TabActive]       = ImVec4(0.18f, 0.35f, 0.75f, 1.00f);
        c[ImGuiCol_SliderGrab]      = ImVec4(0.25f, 0.50f, 0.90f, 1.00f);
        c[ImGuiCol_CheckMark]       = ImVec4(0.30f, 0.70f, 0.95f, 1.00f);
        c[ImGuiCol_PlotLines]       = ImVec4(0.30f, 0.80f, 0.50f, 1.00f);
        c[ImGuiCol_PlotHistogram]   = ImVec4(0.20f, 0.60f, 0.90f, 1.00f);
        c[ImGuiCol_TextSelectedBg]  = ImVec4(0.20f, 0.45f, 0.80f, 0.60f);
        c[ImGuiCol_Separator]       = ImVec4(0.20f, 0.25f, 0.45f, 0.70f);
    }
}
