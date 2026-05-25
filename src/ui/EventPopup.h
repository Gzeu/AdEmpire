#pragma once
// =============================================================================
// EventPopup.h  —  AdEmpire v0.9
// Animated centered modal shown when a JSON market event fires.
// Features: icon, title, description, effect summary, slide+fade animation.
// Usage:
//   static EventPopup ep;
//   ep.Show(evt);          // call when event triggers
//   ep.Render();           // call every frame
//   if (ep.WasDismissed()) { ... }
// =============================================================================
#include "imgui.h"
#include <string>
#include <cmath>

struct MarketEventData {
    std::string id;
    std::string title;
    std::string description;
    std::string effectText;     // e.g. "+15% Social reach for 2 months"
    std::string category;       // "opportunity"|"threat"|"neutral"|"crisis"
    float       durationMonths = 1.0f;
};

class EventPopup {
public:
    bool IsOpen()         const { return m_open; }
    bool WasDismissed()   const { return m_dismissed; }
    void ClearDismissed()       { m_dismissed = false; }

    void Show(const MarketEventData& evt) {
        m_evt       = evt;
        m_open      = true;
        m_dismissed = false;
        m_timer     = 0.0f;
    }

    void Render() {
        if (!m_open) return;
        m_dismissed = false;

        float dt = ImGui::GetIO().DeltaTime;
        m_timer += dt;

        ImGuiIO& io  = ImGui::GetIO();
        ImVec2   disp = io.DisplaySize;

        // semi-transparent backdrop
        ImDrawList* bg = ImGui::GetBackgroundDrawList();
        float bgA = std::min(0.65f, m_timer * 3.0f);
        bg->AddRectFilled({0,0}, disp, IM_COL32(0,0,0,(int)(255*bgA)));

        // ease-in-out slide from top
        float slideT = std::min(1.0f, m_timer * 4.0f);
        float ease   = 1.0f - powf(1.0f - slideT, 3.0f);

        const float PW = 480.0f, PH = 280.0f;
        float px = disp.x/2.0f - PW/2.0f;
        float py = disp.y/2.0f - PH/2.0f - (1.0f-ease)*40.0f;

        // --- category colour
        ImVec4 catCol = CategoryColor();
        ImU32  catU32 = ImGui::ColorConvertFloat4ToU32(catCol);

        ImGui::SetNextWindowPos({px, py});
        ImGui::SetNextWindowSize({PW, PH});
        ImGui::SetNextWindowBgAlpha(0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0,0});
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 14.0f);
        ImGui::Begin("##eventpopup", nullptr,
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoNav        |
            ImGuiWindowFlags_NoMove       |
            ImGuiWindowFlags_NoSavedSettings);
        ImGui::PopStyleVar(2);

        ImDrawList* dl  = ImGui::GetWindowDrawList();
        ImVec2 wp = ImGui::GetWindowPos();

        // window bg
        dl->AddRectFilled(wp, {wp.x+PW, wp.y+PH},
                          IM_COL32(22,20,17,245), 14.0f);
        dl->AddRect(wp, {wp.x+PW, wp.y+PH}, catU32, 14.0f, 0, 1.5f);

        // category colour header strip
        ImVec4 stripCol = catCol; stripCol.w = 0.15f;
        dl->AddRectFilled(wp, {wp.x+PW, wp.y+52.0f},
                          ImGui::ColorConvertFloat4ToU32(stripCol), 14.0f);
        dl->AddRectFilled({wp.x, wp.y+38.0f}, {wp.x+PW, wp.y+52.0f},
                          ImGui::ColorConvertFloat4ToU32(stripCol));

        // category badge
        const char* catLabel = CategoryLabel();
        ImVec2 clts = ImGui::CalcTextSize(catLabel);
        dl->AddRectFilled(
            {wp.x + PW/2.0f - clts.x/2.0f - 8.0f, wp.y+12.0f},
            {wp.x + PW/2.0f + clts.x/2.0f + 8.0f, wp.y+34.0f},
            ImGui::ColorConvertFloat4ToU32({catCol.x,catCol.y,catCol.z,0.25f}),
            6.0f);
        dl->AddText({wp.x + PW/2.0f - clts.x/2.0f, wp.y+14.0f}, catU32, catLabel);

        // icon
        const char* icon = CategoryIcon();
        ImVec2 its = ImGui::CalcTextSize(icon);
        dl->AddText({wp.x + PW/2.0f - its.x/2.0f, wp.y + 58.0f},
                    catU32, icon);

        // title
        ImVec2 tts = ImGui::CalcTextSize(m_evt.title.c_str());
        dl->AddText(
            {wp.x + PW/2.0f - tts.x/2.0f, wp.y + 92.0f},
            IM_COL32(235,230,220,245), m_evt.title.c_str());

        // divider
        dl->AddLine({wp.x+32, wp.y+116}, {wp.x+PW-32, wp.y+116},
                    IM_COL32(255,255,255,20), 1.0f);

        // description (word-wrap)
        ImGui::SetCursorPos({24.0f, 124.0f});
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.75f,0.73f,0.70f,1.0f));
        ImGui::PushTextWrapPos(PW - 24.0f);
        ImGui::TextUnformatted(m_evt.description.c_str());
        ImGui::PopTextWrapPos();
        ImGui::PopStyleColor();

        // effect text
        if (!m_evt.effectText.empty()) {
            ImGui::SetCursorPos({24.0f, 175.0f});
            ImGui::PushStyleColor(ImGuiCol_Text,
                {catCol.x, catCol.y, catCol.z, 0.9f});
            ImGui::Text("> %s", m_evt.effectText.c_str());
            ImGui::PopStyleColor();
        }

        // duration tag
        if (m_evt.durationMonths > 1.0f) {
            char dur[40];
            snprintf(dur, sizeof(dur), "Duration: %.0f months", m_evt.durationMonths);
            ImVec2 dts = ImGui::CalcTextSize(dur);
            dl->AddText({wp.x + PW/2.0f - dts.x/2.0f, wp.y+202.0f},
                        IM_COL32(160,160,160,160), dur);
        }

        // dismiss button
        ImGui::SetCursorPos({PW/2.0f - 70.0f, 228.0f});
        ImGui::PushStyleColor(ImGuiCol_Button,        {catCol.x,catCol.y,catCol.z,0.85f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {catCol.x+0.1f,catCol.y+0.1f,catCol.z+0.1f,1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  {catCol.x-0.1f,catCol.y-0.1f,catCol.z-0.1f,1.0f});
        ImGui::PushStyleColor(ImGuiCol_Text,          {0.05f,0.04f,0.02f,1.0f});
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 7.0f);
        if (ImGui::Button("Acknowledge", {140.0f, 34.0f})) {
            m_open      = false;
            m_dismissed = true;
        }
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(4);

        ImGui::End();
    }

private:
    bool             m_open      = false;
    bool             m_dismissed = false;
    float            m_timer     = 0.0f;
    MarketEventData  m_evt;

    ImVec4 CategoryColor() const {
        if (m_evt.category == "opportunity") return {0.35f,0.62f,0.27f,1.0f};
        if (m_evt.category == "threat")      return {0.79f,0.29f,0.17f,1.0f};
        if (m_evt.category == "crisis")      return {0.79f,0.29f,0.17f,1.0f};
        return {0.91f,0.63f,0.19f,1.0f};  // neutral = gold
    }
    const char* CategoryLabel() const {
        if (m_evt.category == "opportunity") return "OPPORTUNITY";
        if (m_evt.category == "threat")      return "MARKET THREAT";
        if (m_evt.category == "crisis")      return "CRISIS";
        return "MARKET EVENT";
    }
    const char* CategoryIcon() const {
        if (m_evt.category == "opportunity") return "[+]";
        if (m_evt.category == "threat")      return "[!]";
        if (m_evt.category == "crisis")      return "[X]";
        return "[~]";
    }
};
