#pragma once
// =============================================================================
// AgencyBrandingPanel.h  —  AdEmpire v0.9
// Panel where the player sets agency name, colour accent, and ASCII logo.
// Opened from MainMenu "New Game" flow or from Settings.
// Usage:
//   static AgencyBrandingPanel branding;
//   branding.Render(agencyName, accentColor, logoIndex);
//   // agencyName, accentColor, logoIndex are modified in place
// =============================================================================
#include "imgui.h"
#include <string>
#include <cstring>
#include <array>

class AgencyBrandingPanel {
public:
    // Call this once in MainMenu or Settings.
    // Returns true when player clicks "Confirm"
    bool Render(
        std::string& agencyName,
        ImVec4&      accentColor,
        int&         logoIndex
    ) {
        bool confirmed = false;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,  {24.0f, 24.0f});
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 14.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,    {8.0f, 10.0f});

        ImVec2 disp = ImGui::GetIO().DisplaySize;
        const float PW = 480.0f, PH = 540.0f;
        ImGui::SetNextWindowPos({disp.x/2.0f - PW/2.0f, disp.y/2.0f - PH/2.0f},
                                ImGuiCond_Always);
        ImGui::SetNextWindowSize({PW, PH});
        ImGui::SetNextWindowBgAlpha(0.0f);

        ImGui::Begin("##branding_panel", nullptr,
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoMove       |
            ImGuiWindowFlags_NoSavedSettings);

        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImVec2 wp = ImGui::GetWindowPos();
        dl->AddRectFilled(wp, {wp.x+PW, wp.y+PH},
                          IM_COL32(22,20,17,245), 14.0f);
        // accent border
        ImU32 acc = ImGui::ColorConvertFloat4ToU32(accentColor);
        dl->AddRect(wp, {wp.x+PW, wp.y+PH}, acc, 14.0f, 0, 1.5f);

        // ---- title
        ImGui::SetCursorPosY(20.0f);
        CenteredText("Agency Identity", PW, accentColor);
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // ---- agency name input
        ImGui::Text("Agency Name");
        static char nameBuf[64] = "";
        if (m_firstOpen) {
            strncpy(nameBuf, agencyName.c_str(), 63);
            m_firstOpen = false;
        }
        ImGui::SetNextItemWidth(PW - 48.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 7.0f);
        ImGui::InputText("##agname", nameBuf, sizeof(nameBuf));
        ImGui::PopStyleVar();
        agencyName = nameBuf;

        ImGui::Spacing();

        // ---- accent colour picker (6 presets + custom)
        ImGui::Text("Agency Colour");
        static const ImVec4 kPresets[] = {
            {0.91f,0.63f,0.19f,1.0f},  // Gold   (default)
            {0.35f,0.62f,0.27f,1.0f},  // Green
            {0.29f,0.51f,0.77f,1.0f},  // Blue
            {0.79f,0.29f,0.17f,1.0f},  // Red
            {0.49f,0.31f,0.69f,1.0f},  // Purple
            {0.20f,0.78f,0.74f,1.0f},  // Teal
        };
        constexpr int kPresetCount = 6;
        for (int i = 0; i < kPresetCount; ++i) {
            if (i > 0) ImGui::SameLine();
            ImGui::PushID(i);
            bool selected = (fabsf(accentColor.x - kPresets[i].x) < 0.01f);
            if (selected) {
                ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
                ImGui::PushStyleColor(ImGuiCol_Border, accentColor);
            }
            ImGui::PushStyleColor(ImGuiCol_Button, kPresets[i]);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                {kPresets[i].x+0.08f,kPresets[i].y+0.08f,kPresets[i].z+0.08f,1.0f});
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, kPresets[i]);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 22.0f);
            if (ImGui::Button("  ", {36.0f,36.0f})) accentColor = kPresets[i];
            ImGui::PopStyleVar();
            ImGui::PopStyleColor(3);
            if (selected) { ImGui::PopStyleColor(); ImGui::PopStyleVar(); }
            ImGui::PopID();
        }
        ImGui::SameLine();
        // small custom colour button
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 7.0f);
        if (ImGui::ColorButton("Custom##cc", accentColor, 0, {36.0f,36.0f}))
            ImGui::OpenPopup("ColorPicker");
        ImGui::PopStyleVar();
        if (ImGui::BeginPopup("ColorPicker")) {
            ImGui::ColorPicker4("##cp", &accentColor.x,
                ImGuiColorEditFlags_NoAlpha |
                ImGuiColorEditFlags_PickerHueBar);
            ImGui::EndPopup();
        }

        ImGui::Spacing();

        // ---- ASCII logo selector
        ImGui::Text("Agency Logo (ASCII)");

        static const char* kLogos[] = {
            "  /-\\  \n |   | \n  \\_/  ",    // 0: minimal diamond
            " ___ \n|   |\n|___|\n|   |",    // 1: tower
            " /\\\/\\ \n/ _ _\\\n\\___/",   // 2: mountain
            "[====]\n|    |\n[====]",          // 3: badge
            "  **  \n ****\n  **  ",           // 4: star
            " {##} \n {##}\n  \"\" ",         // 5: building
        };
        static const char* kLogoNames[] = {
            "Diamond","Tower","Mountain","Badge","Star","Building"
        };
        constexpr int kLogoCount = 6;

        float thumbW = (PW - 48.0f - 5.0f*8.0f) / 6.0f;
        for (int i = 0; i < kLogoCount; ++i) {
            if (i > 0) ImGui::SameLine(0, 8.0f);
            ImGui::PushID(100 + i);
            bool sel = (logoIndex == i);
            ImVec4 bg = sel ? ImVec4{accentColor.x,accentColor.y,accentColor.z,0.2f}
                            : ImVec4{1.0f,1.0f,1.0f,0.05f};
            ImGui::PushStyleColor(ImGuiCol_ChildBg, bg);
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 8.0f);
            ImGui::BeginChild(kLogoNames[i], {thumbW, 72.0f}, false);
            ImGui::SetCursorPos({4.0f,4.0f});
            ImGui::PushStyleColor(ImGuiCol_Text,
                sel ? accentColor : ImVec4{0.6f,0.6f,0.6f,1.0f});
            ImGui::TextUnformatted(kLogoNames[i]);
            ImGui::PopStyleColor();
            ImGui::EndChild();
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();
            if (ImGui::IsItemClicked()) logoIndex = i;
            ImGui::PopID();
        }

        // live preview
        ImGui::Spacing();
        ImGui::Text("Preview:");
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4{1.0f,1.0f,1.0f,0.04f});
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 8.0f);
        ImGui::BeginChild("##preview", {PW - 48.0f, 80.0f}, false);
        ImGui::SetCursorPos({10.0f, 8.0f});
        ImGui::PushStyleColor(ImGuiCol_Text, accentColor);
        ImGui::TextUnformatted(agencyName.empty() ? "Agency Name" : agencyName.c_str());
        ImGui::PopStyleColor();
        ImGui::SetCursorPos({10.0f, 30.0f});
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{0.65f,0.63f,0.60f,1.0f});
        ImGui::TextUnformatted(logoIndex >= 0 && logoIndex < kLogoCount ?
            kLogoNames[logoIndex] : "");
        ImGui::PopStyleColor();
        ImGui::EndChild();
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();

        ImGui::Spacing();

        // ---- Confirm button
        ImGui::SetCursorPosX((PW - 200.0f)/2.0f);
        ImGui::PushStyleColor(ImGuiCol_Button,
            {accentColor.x, accentColor.y, accentColor.z, 0.9f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
            {accentColor.x+0.08f,accentColor.y+0.08f,accentColor.z+0.08f,1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,
            {accentColor.x-0.1f,accentColor.y-0.1f,accentColor.z-0.1f,1.0f});
        ImGui::PushStyleColor(ImGuiCol_Text, {0.06f,0.04f,0.01f,1.0f});
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 9.0f);
        if (ImGui::Button("Confirm & Start", {200.0f, 40.0f}))
            confirmed = true;
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(4);

        ImGui::End();
        ImGui::PopStyleVar(3);
        return confirmed;
    }

private:
    bool m_firstOpen = true;

    static void CenteredText(const char* text, float width, ImVec4 col) {
        ImVec2 ts = ImGui::CalcTextSize(text);
        ImGui::SetCursorPosX((width - ts.x) / 2.0f);
        ImGui::PushStyleColor(ImGuiCol_Text, col);
        ImGui::TextUnformatted(text);
        ImGui::PopStyleColor();
    }
};
