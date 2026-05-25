#pragma once
#include "imgui.h"
#include "imgui_internal.h"

// ─────────────────────────────────────────────────────────────────────────────
// DockSpaceLayout  — persistent docking layout for AdEmpire
//
// Panels docked by default:
//   LEFT  (25%): StaffPanel + SpecializationPanel (tabbed)
//   CENTER(50%): Dashboard  + CampaignEditor      (tabbed)
//   RIGHT (25%): ClientManager + MarketMap        (tabbed)
//   BOTTOM(20%): Newsfeed + LeaderboardPanel      (tabbed)
//
// Usage in main loop (after ImGui::NewFrame, before any Begin()):
//   DockSpaceLayout::Render(gs);
//   // then call your panel Begin() calls as usual
// ─────────────────────────────────────────────────────────────────────────────

namespace DockSpaceLayout {

    inline ImGuiID g_DockID = 0;

    inline void Build(ImGuiID dockspace_id) {
        ImGui::DockBuilderRemoveNode(dockspace_id);
        ImGui::DockBuilderAddNode(dockspace_id,
            ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace_id,
            ImGui::GetMainViewport()->Size);

        ImGuiID dock_center = dockspace_id;
        ImGuiID dock_left, dock_right, dock_bottom;

        // Split: left 25%
        ImGui::DockBuilderSplitNode(dock_center, ImGuiDir_Left, 0.25f,
            &dock_left, &dock_center);
        // Split: right 25% of remaining
        ImGui::DockBuilderSplitNode(dock_center, ImGuiDir_Right, 0.33f,
            &dock_right, &dock_center);
        // Split: bottom 20%
        ImGui::DockBuilderSplitNode(dock_center, ImGuiDir_Down, 0.20f,
            &dock_bottom, &dock_center);

        // Assign windows to slots
        ImGui::DockBuilderDockWindow("Staff",            dock_left);
        ImGui::DockBuilderDockWindow("Specializations",  dock_left);
        ImGui::DockBuilderDockWindow("Goals",            dock_left);

        ImGui::DockBuilderDockWindow("Dashboard",        dock_center);
        ImGui::DockBuilderDockWindow("Campaign Editor",  dock_center);
        ImGui::DockBuilderDockWindow("Report",           dock_center);
        ImGui::DockBuilderDockWindow("Achievements",     dock_center);
        ImGui::DockBuilderDockWindow("Templates",        dock_center);

        ImGui::DockBuilderDockWindow("Clients",          dock_right);
        ImGui::DockBuilderDockWindow("Market Map",       dock_right);
        ImGui::DockBuilderDockWindow("Leaderboard",      dock_right);
        ImGui::DockBuilderDockWindow("Settings",         dock_right);

        ImGui::DockBuilderDockWindow("Newsfeed",         dock_bottom);
        ImGui::DockBuilderDockWindow("Save Slots",       dock_bottom);

        ImGui::DockBuilderFinish(dockspace_id);
    }

    // Call every frame from your main render loop,
    // BEFORE calling any panel Begin().
    inline void Render() {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGuiWindowFlags host_flags =
            ImGuiWindowFlags_NoTitleBar       |
            ImGuiWindowFlags_NoCollapse       |
            ImGuiWindowFlags_NoResize         |
            ImGuiWindowFlags_NoMove           |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus       |
            ImGuiWindowFlags_NoBackground     |
            ImGuiWindowFlags_MenuBar;         // keep your menu bar

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,   0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,    ImVec2(0, 0));
        ImGui::Begin("##AdEmpireHost", nullptr, host_flags);
        ImGui::PopStyleVar(3);

        g_DockID = ImGui::GetID("##MainDockSpace");

        // First-time layout build
        static bool first_time = true;
        if (first_time) {
            first_time = false;
            Build(g_DockID);
        }

        ImGui::DockSpace(g_DockID, ImVec2(0.0f, 0.0f),
            ImGuiDockNodeFlags_PassthruCentralNode);

        ImGui::End();
    }

    // Reset layout — call from a "Reset Layout" menu item
    inline void Reset() {
        static bool pending_reset = false;
        pending_reset = true;
        if (pending_reset && g_DockID) {
            Build(g_DockID);
            pending_reset = false;
        }
    }

} // namespace DockSpaceLayout
