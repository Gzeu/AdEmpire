#pragma once
#include "imgui.h"
#include "../core/GameState.h"
#include <string>
#include <vector>
#include <functional>

// ============================================================
//  SidebarNav  —  Unified left navigation  (v0.8)
//  Fixed-width sidebar (180 px) with icon + label buttons
//  for all 16 panels.  Supports live badges (counters,
//  alerts) and a collapsible mode at narrow widths.
//
//  Usage:
//    SidebarNav nav;
//    nav.SetActive(SidebarNav::Page::Dashboard);
//    // in render loop:
//    nav.Render(gameState);
//    if (nav.ActiveChanged()) { /* switch shown panel */ }
// ============================================================

class SidebarNav {
public:
    enum class Page {
        Dashboard = 0,
        LiveMarket,
        Newsfeed,
        Clients,
        Campaigns,
        Negotiations,
        Staff,
        Specializations,
        Templates,
        Achievements,
        Reports,
        Leaderboard,
        MarketMap,
        Branding,
        SaveSlots,
        Settings,
        _COUNT
    };

    // --------------------------------------------------------
    void Render(const GameState& gs) {
        _prevPage   = _active;
        _changed    = false;

        // Sidebar window
        ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize   |
            ImGuiWindowFlags_NoMove     |
            ImGuiWindowFlags_NoScrollbar|
            ImGuiWindowFlags_NoBringToFrontOnFocus;

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(kWidth, ImGui::GetIO().DisplaySize.y));
        ImGui::PushStyleColor(ImGuiCol_WindowBg,
            ImVec4(0.08f, 0.08f, 0.10f, 1.00f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,   ImVec2(0, 1));

        ImGui::Begin("##sidebar_nav", nullptr, flags);

        // ── Logo / game title ────────────────────────────────
        ImGui::PushStyleColor(ImGuiCol_Text,
            ImVec4(0.85f, 0.75f, 0.30f, 1.00f));
        ImGui::SetCursorPosY(12);
        float tw = ImGui::CalcTextSize("AdEmpire").x;
        ImGui::SetCursorPosX((kWidth - tw) * 0.5f);
        ImGui::Text("AdEmpire");
        ImGui::PopStyleColor();

        ImGui::PushStyleColor(ImGuiCol_Text,
            ImVec4(0.35f, 0.35f, 0.40f, 1.00f));
        float sv = ImGui::CalcTextSize("v0.8").x;
        ImGui::SetCursorPosX((kWidth - sv) * 0.5f);
        ImGui::Text("v0.8");
        ImGui::PopStyleColor();

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // ── Nav items ────────────────────────────────────────
        for (int i = 0; i < static_cast<int>(Page::_COUNT); ++i) {
            _RenderItem(static_cast<Page>(i), gs);
        }

        // ── Bottom: market alert pill ────────────────────────
        if (gs.revenueMultiplier < 0.85f) {
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::PushStyleColor(ImGuiCol_Text,
                ImVec4(0.95f, 0.40f, 0.30f, 1.00f));
            float aw = ImGui::CalcTextSize("⚡ Market Event Active").x;
            ImGui::SetCursorPosX(std::max(0.f, (kWidth - aw) * 0.5f));
            ImGui::TextWrapped("⚡ Market Event Active");
            ImGui::PopStyleColor();
        }

        ImGui::End();
        ImGui::PopStyleColor(); // WindowBg
        ImGui::PopStyleVar(2);
    }

    void      SetActive(Page p)  { _active = p; }
    Page      Active()    const  { return _active; }
    bool      ActiveChanged() const { return _changed; }

    static constexpr float kWidth = 180.f;

private:
    Page _active   = Page::Dashboard;
    Page _prevPage = Page::Dashboard;
    bool _changed  = false;

    struct NavItem {
        const char* icon;
        const char* label;
        // lambda to compute badge count (0 = no badge)
        std::function<int(const GameState&)> badge;
    };

    // ── Item definitions (order = Page enum) ────────────────
    inline static const NavItem kItems[] = {
        { "🏠", "Dashboard",       nullptr },
        { "📈", "Live Market",     nullptr },
        { "📰", "Newsfeed",        nullptr },
        { "🤝", "Clients",
            [](const GameState& g){ return static_cast<int>(g.clients.size()); } },
        { "📣", "Campaigns",
            [](const GameState& g){ return static_cast<int>(g.campaigns.size()); } },
        { "💬", "Negotiations",    nullptr },
        { "👥", "Staff",
            [](const GameState& g){ return static_cast<int>(g.staff.size()); } },
        { "⭐", "Specializations", nullptr },
        { "📄", "Templates",       nullptr },
        { "🏆", "Achievements",
            [](const GameState& g){
                int locked = 0;
                for (auto& a : g.achievements) if (!a.unlocked) locked++;
                return locked; // show remaining count
            }},
        { "📊", "Reports",         nullptr },
        { "🥇", "Leaderboard",     nullptr },
        { "🗺️", "Market Map",      nullptr },
        { "🎨", "Branding",        nullptr },
        { "💾", "Save Slots",      nullptr },
        { "⚙️", "Settings",        nullptr },
    };

    // --------------------------------------------------------
    void _RenderItem(Page page, const GameState& gs) {
        int idx = static_cast<int>(page);
        const NavItem& item = kItems[idx];
        bool isActive = (page == _active);

        // Active highlight
        if (isActive) {
            ImVec2 pos = ImGui::GetCursorScreenPos();
            ImGui::GetWindowDrawList()->AddRectFilled(
                pos,
                ImVec2(pos.x + kWidth, pos.y + 32.f),
                IM_COL32(255, 200, 60, 25));
            // left accent bar
            ImGui::GetWindowDrawList()->AddRectFilled(
                pos,
                ImVec2(pos.x + 3.f, pos.y + 32.f),
                IM_COL32(255, 195, 50, 220));
        }

        // Button area
        ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0,0,0,0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
            isActive ? ImVec4(0,0,0,0) : ImVec4(1,1,1,0.06f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(1,1,1,0.10f));
        ImGui::PushStyleColor(ImGuiCol_Text,
            isActive
                ? ImVec4(1.00f, 0.88f, 0.40f, 1.f)   // gold when active
                : ImVec4(0.72f, 0.72f, 0.75f, 1.f));  // muted otherwise

        std::string label = std::string("  ")
            + item.icon + "  " + item.label;
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.f, 0.5f));

        if (ImGui::Button(label.c_str(),
                ImVec2(kWidth, 32.f))) {
            if (!isActive) {
                _active  = page;
                _changed = true;
            }
        }
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(4);

        // Badge
        if (item.badge) {
            int count = item.badge(gs);
            if (count > 0) {
                std::string badge = std::to_string(count);
                ImVec2 bpos = ImGui::GetItemRectMin();
                float bx = bpos.x + kWidth - 28.f;
                float by = bpos.y + 8.f;
                float bw = ImGui::CalcTextSize(badge.c_str()).x + 8.f;
                ImGui::GetWindowDrawList()->AddRectFilled(
                    ImVec2(bx, by),
                    ImVec2(bx + bw, by + 16.f),
                    IM_COL32(255, 195, 50, 200), 8.f);
                ImGui::GetWindowDrawList()->AddText(
                    ImVec2(bx + 4.f, by + 2.f),
                    IM_COL32(20, 18, 10, 255),
                    badge.c_str());
            }
        }
    }
};
