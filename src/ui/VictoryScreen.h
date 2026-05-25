#pragma once
// =============================================================================
// VictoryScreen.h  —  AdEmpire v0.9
// Full-window overlay shown when player reaches 35% market share.
// Features: particle confetti, animated score reveal, replay stats.
// Usage:
//   static VictoryScreen vs;
//   if (gameWon && !vs.IsOpen()) vs.Open(finalStats);
//   vs.Render();  // call every frame; returns true while still showing
// =============================================================================
#include "imgui.h"
#include <vector>
#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>

struct VictoryStats {
    float   finalMarketShare  = 0.0f;   // 0-100
    float   totalRevenue      = 0.0f;
    int     monthsPlayed      = 0;
    int     clientsServed     = 0;
    int     achievementsEarned= 0;
    int     campaignsRun      = 0;
    float   bestMonthRevenue  = 0.0f;
    std::string agencyName    = "Your Agency";
    std::string difficulty    = "Normal";
};

class VictoryScreen {
public:
    bool IsOpen() const { return m_open; }

    void Open(const VictoryStats& stats) {
        m_stats   = stats;
        m_open    = true;
        m_timer   = 0.0f;
        m_phase   = Phase::FadeIn;
        m_confettiAge = 0.0f;
        srand((unsigned)time(nullptr));
        SpawnConfetti();
    }

    // Returns false when the player clicked "Play Again" or "Main Menu"
    // action: 0=still open, 1=play again, 2=main menu
    int Render() {
        if (!m_open) return 0;

        float dt = ImGui::GetIO().DeltaTime;
        m_timer += dt;
        m_confettiAge += dt;
        UpdateConfetti(dt);

        ImGuiIO& io = ImGui::GetIO();
        ImVec2 disp = io.DisplaySize;

        // --- full-screen dark overlay
        ImDrawList* bg = ImGui::GetBackgroundDrawList();
        float alpha = std::min(1.0f, m_timer * 2.0f);
        bg->AddRectFilled({0,0}, disp,
            IM_COL32(10,8,6,(int)(220*alpha)));

        // --- confetti (background layer)
        DrawConfetti(bg);

        // --- central card
        const float CW = 560.0f, CH = 460.0f;
        ImVec2 cardPos = { disp.x/2.0f - CW/2.0f, disp.y/2.0f - CH/2.0f };

        float slide = std::min(1.0f, m_timer * 3.0f);
        float ease  = 1.0f - (1.0f - slide)*(1.0f - slide); // ease-out
        cardPos.y += (1.0f - ease) * 60.0f;                  // slide in from below

        ImGui::SetNextWindowPos(cardPos);
        ImGui::SetNextWindowSize({CW, CH});
        ImGui::SetNextWindowBgAlpha(0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0,0});
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 16.0f);
        ImGui::Begin("##victory", nullptr,
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoNav        |
            ImGuiWindowFlags_NoMove       |
            ImGuiWindowFlags_NoSavedSettings);
        ImGui::PopStyleVar(2);

        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImVec2 wp = ImGui::GetWindowPos();
        ImVec2 ws = ImGui::GetWindowSize();

        // card background
        dl->AddRectFilled(wp, {wp.x+ws.x, wp.y+ws.y},
                          IM_COL32(22,20,17,240), 16.0f);
        // gold border glow
        float pulse = 0.6f + 0.4f * sinf(m_timer * 2.5f);
        dl->AddRect(wp, {wp.x+ws.x, wp.y+ws.y},
                    IM_COL32(232,160,48,(int)(140*pulse)), 16.0f, 0, 2.0f);

        // inner glow stripe
        dl->AddRectFilledMultiColor(
            wp, {wp.x+ws.x, wp.y+80.0f},
            IM_COL32(232,160,48,30), IM_COL32(232,160,48,30),
            IM_COL32(0,0,0,0), IM_COL32(0,0,0,0));

        ImGui::SetCursorPos({0, 28});

        // trophy icon (ASCII art in ImGui text)
        ImGui::SetCursorPosX(CW/2.0f - 24.0f);
        ImGui::TextUnformatted("  [===]  ");
        ImGui::SetCursorPosX(CW/2.0f - 36.0f);
        ImGui::TextUnformatted("   \\  /   ");
        // --- big title
        const char* title1 = "EMPIRE COMPLETE";
        ImVec2 t1s = ImGui::CalcTextSize(title1);
        ImGui::SetCursorPos({CW/2.0f - t1s.x/2.0f, 80.0f});
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.91f,0.627f,0.19f,1.0f));
        ImGui::TextUnformatted(title1);
        ImGui::PopStyleColor();

        // agency name
        ImVec2 n2s = ImGui::CalcTextSize(m_stats.agencyName.c_str());
        ImGui::SetCursorPos({CW/2.0f - n2s.x/2.0f, 108.0f});
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f,0.8f,0.8f,0.8f));
        ImGui::TextUnformatted(m_stats.agencyName.c_str());
        ImGui::PopStyleColor();

        // divider
        dl->AddLine({wp.x+32, wp.y+138}, {wp.x+CW-32, wp.y+138},
                    IM_COL32(232,160,48,60), 1.0f);

        // --- stats grid (2x3)
        float revealT = std::max(0.0f, (m_timer - 0.4f) * 2.0f);
        struct StatEntry { const char* label; char value[32]; ImVec4 col; };
        StatEntry entries[6] = {
            {"Market Share",    "", {0.91f,0.63f,0.19f,1.0f}},
            {"Total Revenue",   "", {0.35f,0.62f,0.27f,1.0f}},
            {"Months Played",   "", {0.80f,0.80f,0.80f,0.9f}},
            {"Clients Served",  "", {0.80f,0.80f,0.80f,0.9f}},
            {"Achievements",    "", {0.91f,0.63f,0.19f,1.0f}},
            {"Campaigns Run",   "", {0.80f,0.80f,0.80f,0.9f}},
        };
        snprintf(entries[0].value, 32, "%.1f%%",  m_stats.finalMarketShare);
        snprintf(entries[1].value, 32, "$%.0fk",  m_stats.totalRevenue/1000.0f);
        snprintf(entries[2].value, 32, "%d",       m_stats.monthsPlayed);
        snprintf(entries[3].value, 32, "%d",       m_stats.clientsServed);
        snprintf(entries[4].value, 32, "%d / 15",  m_stats.achievementsEarned);
        snprintf(entries[5].value, 32, "%d",       m_stats.campaignsRun);

        float startY = 155.0f;
        float colW   = CW / 3.0f;
        for (int i = 0; i < 6; ++i) {
            int row = i / 3, col = i % 3;
            float bx = col * colW + 12.0f;
            float by = startY + row * 80.0f;

            float cardAlpha = std::min(1.0f, revealT - i*0.12f);
            if (cardAlpha <= 0.0f) continue;

            ImVec2 cmin = {wp.x + bx, wp.y + by};
            ImVec2 cmax = {wp.x + bx + colW - 16.0f, wp.y + by + 66.0f};
            // subtle card bg
            dl->AddRectFilled(cmin, cmax,
                IM_COL32(255,255,255,(int)(12*cardAlpha)), 8.0f);

            // value
            ImGui::SetCursorPos({bx+8.0f, by+10.0f});
            ImGui::PushStyleColor(ImGuiCol_Text,
                {entries[i].col.x, entries[i].col.y, entries[i].col.z,
                 entries[i].col.w * cardAlpha});
            ImGui::TextUnformatted(entries[i].value);
            ImGui::PopStyleColor();

            // label
            ImGui::SetCursorPos({bx+8.0f, by+36.0f});
            ImGui::PushStyleColor(ImGuiCol_Text,
                {0.6f,0.6f,0.6f, 0.7f*cardAlpha});
            ImGui::TextUnformatted(entries[i].label);
            ImGui::PopStyleColor();
        }

        // difficulty badge
        ImGui::SetCursorPos({CW/2.0f - 50.0f, 330.0f});
        ImGui::PushStyleColor(ImGuiCol_Text, {0.56f,0.56f,0.56f,0.7f});
        ImGui::Text("Difficulty: %s", m_stats.difficulty.c_str());
        ImGui::PopStyleColor();

        // --- buttons
        float buttonY = 375.0f;
        float bw = 180.0f, bh = 38.0f;

        // Play Again
        ImGui::SetCursorPos({CW/2.0f - bw - 8.0f, buttonY});
        ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.91f,0.63f,0.19f,0.90f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.96f,0.71f,0.29f,1.00f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.75f,0.50f,0.10f,1.00f));
        ImGui::PushStyleColor(ImGuiCol_Text,          ImVec4(0.10f,0.07f,0.02f,1.00f));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
        bool playAgain = ImGui::Button("Play Again", {bw, bh});
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(4);

        // Main Menu
        ImGui::SetCursorPos({CW/2.0f + 8.0f, buttonY});
        ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(1,1,1,0.08f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1,1,1,0.14f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(1,1,1,0.20f));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
        bool mainMenu = ImGui::Button("Main Menu", {bw, bh});
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(3);

        ImGui::End();

        if (playAgain) { m_open = false; return 1; }
        if (mainMenu)  { m_open = false; return 2; }
        return 0;
    }

private:
    enum class Phase { FadeIn, Show };
    Phase m_phase   = Phase::FadeIn;
    bool  m_open    = false;
    float m_timer   = 0.0f;
    float m_confettiAge = 0.0f;
    VictoryStats m_stats;

    struct Particle {
        float x, y, vx, vy, rot, rotV, life, maxLife;
        ImVec4 col;
        float size;
    };
    std::vector<Particle> m_particles;

    static float RandF(float lo, float hi) {
        return lo + (hi - lo) * (float)rand() / RAND_MAX;
    }
    static ImVec4 RandColor() {
        static ImVec4 palette[] = {
            {0.91f,0.63f,0.19f,1.0f},  // gold
            {0.35f,0.62f,0.27f,1.0f},  // green
            {0.29f,0.51f,0.77f,1.0f},  // blue
            {0.79f,0.29f,0.17f,1.0f},  // red
            {0.49f,0.31f,0.69f,1.0f},  // purple
            {0.95f,0.95f,0.95f,1.0f},  // white
        };
        return palette[rand() % 6];
    }

    void SpawnConfetti() {
        m_particles.clear();
        ImVec2 disp = ImGui::GetIO().DisplaySize;
        for (int i = 0; i < 180; ++i) {
            Particle p;
            p.x     = RandF(0.0f, disp.x);
            p.y     = RandF(-100.0f, -10.0f);
            p.vx    = RandF(-60.0f, 60.0f);
            p.vy    = RandF(80.0f, 240.0f);
            p.rot   = RandF(0.0f, 6.28f);
            p.rotV  = RandF(-4.0f, 4.0f);
            p.maxLife = RandF(2.5f, 5.5f);
            p.life  = p.maxLife;
            p.col   = RandColor();
            p.size  = RandF(4.0f, 10.0f);
            m_particles.push_back(p);
        }
    }

    void UpdateConfetti(float dt) {
        ImVec2 disp = ImGui::GetIO().DisplaySize;
        // respawn every 1.2 s
        if (m_confettiAge > 1.2f) {
            m_confettiAge = 0.0f;
            for (int i = 0; i < 30; ++i) {
                Particle p;
                p.x = RandF(0.0f, disp.x);
                p.y = -10.0f;
                p.vx = RandF(-50.0f, 50.0f);
                p.vy = RandF(90.0f, 200.0f);
                p.rot = RandF(0.0f, 6.28f);
                p.rotV = RandF(-3.0f, 3.0f);
                p.maxLife = RandF(2.0f, 4.0f);
                p.life = p.maxLife;
                p.col = RandColor();
                p.size = RandF(4.0f, 9.0f);
                m_particles.push_back(p);
            }
        }
        for (auto& p : m_particles) {
            p.x   += p.vx * dt;
            p.y   += p.vy * dt;
            p.rot += p.rotV * dt;
            p.vy  += 60.0f * dt;  // gravity
            p.life -= dt;
        }
        m_particles.erase(std::remove_if(m_particles.begin(), m_particles.end(),
            [](const Particle& p){ return p.life <= 0.0f || p.y > 1200.0f; }),
            m_particles.end());
    }

    void DrawConfetti(ImDrawList* dl) {
        for (const auto& p : m_particles) {
            if (p.life <= 0.0f) continue;
            float alpha = std::min(1.0f, p.life / (p.maxLife * 0.3f));
            ImU32 col = IM_COL32(
                (int)(p.col.x*255),
                (int)(p.col.y*255),
                (int)(p.col.z*255),
                (int)(alpha*200));
            // rotated rectangle (confetti piece)
            float cs = cosf(p.rot), sn = sinf(p.rot);
            float hw = p.size * 0.5f, hh = p.size * 0.25f;
            ImVec2 corners[4] = {
                { p.x + cs*(-hw) - sn*(-hh),  p.y + sn*(-hw) + cs*(-hh) },
                { p.x + cs*(+hw) - sn*(-hh),  p.y + sn*(+hw) + cs*(-hh) },
                { p.x + cs*(+hw) - sn*(+hh),  p.y + sn*(+hw) + cs*(+hh) },
                { p.x + cs*(-hw) - sn*(+hh),  p.y + sn*(-hw) + cs*(+hh) },
            };
            dl->AddConvexPolyFilled(corners, 4, col);
        }
    }
};
