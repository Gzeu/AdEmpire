#pragma once
// =============================================================================
// SplashScreen.h  —  AdEmpire v0.9
// Animated logo intro shown once at startup (~2.5 seconds total).
// Phases: fade-in logo → typewriter tagline → brief hold → fade out
// Usage:
//   static SplashScreen splash;
//   if (!splash.IsDone()) { splash.Render(); return; }  // block game loop
// =============================================================================
#include "imgui.h"
#include <cmath>
#include <cstring>

class SplashScreen {
public:
    bool IsDone()  const { return m_done; }
    void Skip()          { m_done = true; }

    void Render() {
        if (m_done) return;

        float dt = ImGui::GetIO().DeltaTime;
        m_timer += dt;

        ImGuiIO& io  = ImGui::GetIO();
        ImVec2   disp = io.DisplaySize;

        // Allow clicking/pressing Esc to skip
        if (ImGui::IsKeyPressed(ImGuiKey_Escape) ||
            ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            m_done = true;
            return;
        }

        // Phase timing:
        //  0.0 - 0.6s  : logo fades in
        //  0.6 - 1.6s  : tagline types out  (1.0 s for full text)
        //  1.6 - 2.2s  : hold
        //  2.2 - 2.8s  : everything fades out
        //  2.8s+       : done
        if (m_timer > 2.8f) { m_done = true; return; }

        ImDrawList* dl = ImGui::GetBackgroundDrawList();

        // full-screen bg
        float fadeOut = m_timer > 2.2f ? 1.0f - (m_timer - 2.2f) / 0.6f : 1.0f;
        dl->AddRectFilled({0,0}, disp, IM_COL32(10,8,6,(int)(255*fadeOut)));

        // ---- logo block --------------------------------------------------------
        float logoAlpha = m_timer < 0.6f ? m_timer / 0.6f : 1.0f;
        logoAlpha *= fadeOut;

        const char* lines[] = {
            " ___       _  _____                 _",
            "|   \\   __| ||  ___|_ __  _ __  (_)_ _  ___",
            "| |\ \ / _` ||  _| | '  \\| '_ \ | | '_|/ -_)",
            "|___/ \\__,_||___| |_|_|_|| .__/ |_|_|  \\___|",
            "                          |_|"
        };
        int nLines = 5;
        float lineH  = ImGui::GetTextLineHeight();
        float totalH = lineH * nLines * 1.3f;
        float startY = disp.y / 2.0f - totalH / 2.0f - 30.0f;

        // subtle gold glow behind text
        dl->AddRectFilled(
            {disp.x/2.0f - 260.0f, startY - 10.0f},
            {disp.x/2.0f + 260.0f, startY + totalH + 10.0f},
            IM_COL32(232,160,48,(int)(12*logoAlpha)), 8.0f);

        for (int i = 0; i < nLines; ++i) {
            ImVec2 ts = ImGui::CalcTextSize(lines[i]);
            float x = disp.x/2.0f - ts.x/2.0f;
            float y = startY + i * lineH * 1.3f;

            // row slides in from left with staggered delay
            float rowDelay = i * 0.06f;
            float rowT     = std::min(1.0f, std::max(0.0f, (m_timer - rowDelay) / 0.5f));
            float ease     = 1.0f - (1.0f - rowT)*(1.0f - rowT);
            float ox       = (1.0f - ease) * -30.0f;  // slide from left

            dl->AddText({x + ox, y},
                IM_COL32(232,160,48,(int)(255*logoAlpha*ease)),
                lines[i]);
        }

        // ---- tagline -----------------------------------------------------------
        float taglineStart = 0.6f;
        float tagAlpha = m_timer > taglineStart ?
            std::min(1.0f, (m_timer - taglineStart) / 0.3f) : 0.0f;
        tagAlpha *= fadeOut;

        if (tagAlpha > 0.0f) {
            const char* tagFull = "Build. Pitch. Dominate the market.";
            int tagLen   = (int)strlen(tagFull);
            // typewriter: reveal one char per 0.03s after tagline starts
            float tElapsed = m_timer - taglineStart - 0.2f;
            int   visible  = (int)(tElapsed / 0.038f);
            if (visible < 0) visible = 0;
            if (visible > tagLen) visible = tagLen;

            char buf[64] = {};
            strncpy(buf, tagFull, visible);

            ImVec2 ts = ImGui::CalcTextSize(buf);
            dl->AddText(
                {disp.x/2.0f - ImGui::CalcTextSize(tagFull).x/2.0f,
                 disp.y/2.0f + 40.0f},
                IM_COL32(200,200,200,(int)(200*tagAlpha)),
                buf);

            // blinking cursor
            if (visible < tagLen) {
                float cx = disp.x/2.0f
                         - ImGui::CalcTextSize(tagFull).x/2.0f
                         + ts.x + 2.0f;
                float cy = disp.y/2.0f + 40.0f;
                float blink = sinf(m_timer * 8.0f) > 0.0f ? 1.0f : 0.0f;
                dl->AddRectFilled(
                    {cx, cy},
                    {cx + 2.0f, cy + ImGui::GetTextLineHeight()},
                    IM_COL32(200,200,200,(int)(180*blink*tagAlpha)));
            }
        }

        // ---- skip hint ---------------------------------------------------------
        if (m_timer > 0.8f && m_timer < 2.2f) {
            float hintA = std::min(1.0f, (m_timer - 0.8f) / 0.4f) * fadeOut * 0.5f;
            const char* hint = "Press ESC or click to skip";
            ImVec2 hs = ImGui::CalcTextSize(hint);
            dl->AddText(
                {disp.x/2.0f - hs.x/2.0f, disp.y - 40.0f},
                IM_COL32(180,180,180,(int)(255*hintA)), hint);
        }

        // block all ImGui windows during splash
        ImGui::SetNextWindowPos({0, 0});
        ImGui::SetNextWindowSize(disp);
        ImGui::SetNextWindowBgAlpha(0.0f);
        ImGui::Begin("##splash_blocker", nullptr,
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoNav        |
            ImGuiWindowFlags_NoMove       |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoInputs);
        ImGui::End();
    }

private:
    float m_timer = 0.0f;
    bool  m_done  = false;
};
