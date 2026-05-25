#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <cstdio>
#include <cstdlib>
#include <string>

// ── Core ────────────────────────────────────────────────────────────────────
#include "core/GameState.h"
#include "core/Simulation.h"
#include "core/Difficulty.h"

// ── Systems ─────────────────────────────────────────────────────────────────
#include "systems/EventSystem.h"
#include "systems/AICompetitor.h"
#include "systems/SaveSystem.h"
#include "systems/ToastSystem.h"
#include "systems/AchievementSystem.h"   // v0.5
#include "systems/StaffLeveling.h"       // v0.5
#include "systems/SeasonalEvents.h"      // v0.5

// ── v0.8 Systems ─────────────────────────────────────────────────────────────
#include "audio/AudioSystem.h"            // v0.8: OpenAL procedural audio
#include "network/LeaderboardClient.h"    // v0.8: REST global leaderboard

// ── UI ──────────────────────────────────────────────────────────────────────
#include "ui/Theme.h"
#include "ui/UIStyle.h"
#include "ui/MainMenu.h"
#include "ui/Dashboard.h"
#include "ui/CampaignEditor.h"
#include "ui/ClientManager.h"
#include "ui/MarketMap.h"
#include "ui/Newsfeed.h"
#include "ui/StaffPanel.h"
#include "ui/ReportPanel.h"
#include "ui/AchievementsPanel.h"        // v0.5
#include "ui/TemplatesPanel.h"           // v0.5
#include "ui/SaveSlotsPanel.h"           // v0.5
#include "ui/LeaderboardPanel.h"         // v0.8
#include "ui/SettingsPanel.h"            // v0.8

// ── NegotiationPanel / GoalsPanel / SpecializationPanel (v0.2) ───────────────
#include "ui/NegotiationPanel.h"
#include "ui/GoalsPanel.h"
#include "ui/SpecializationPanel.h"

// ── v0.9 Graphics ────────────────────────────────────────────────────────────
#include "ui/SplashScreen.h"             // v0.9: ASCII logo + typewriter intro
#include "ui/VictoryScreen.h"            // v0.9: confetti + stat cards
#include "ui/EventPopup.h"               // v0.9: styled event modal
#include "ui/AgencyBrandingPanel.h"      // v0.9: branding + logo picker
// ChartRenderer.h is included directly inside StatsPanel.cpp

static GameState  gs;
static bool       gameStarted      = false;
static bool       pendingNextMonth = false;
static double     lastTime         = 0.0;

// ── v0.9 global UI objects ───────────────────────────────────────────────────
static SplashScreen        g_splash;
static VictoryScreen       g_victory;
static EventPopup          g_eventPopup;
static AgencyBrandingPanel g_branding;
static bool                g_showBranding = false;  // toggled from MainMenu NewGame

static void glfw_error_callback(int error, const char* desc) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, desc);
}

// ── helper: close all panels except target ───────────────────────────────────
static void openOnly_v(bool& target) {
    bool* all[] = {
        &gs.showCampaigns, &gs.showClients, &gs.showStaff,
        &gs.showMarketMap, &gs.showNewsfeed, &gs.showReport,
        &gs.showAchievements, &gs.showGoals, &gs.showSpecializations,
        &gs.showNegotiation, &gs.showTemplates, &gs.showSaveSlots,
        &gs.showLeaderboard, &gs.showSettings
    };
    for (auto* p : all) *p = false;
    target = true;
}

// ── Navbar ───────────────────────────────────────────────────────────────────
void RenderNavbar() {
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(1280, 40), ImGuiCond_Always);
    ImGui::Begin("##navbar", nullptr,
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBringToFrontOnFocus);

    auto openOnly = [&](bool& target) { openOnly_v(target); };

    if (ImGui::Button(" Dashboard  "))  { gs.showDashboard = true; }
    ImGui::SameLine();
    if (ImGui::Button(" Campaigns  "))  openOnly(gs.showCampaigns);
    ImGui::SameLine();
    if (ImGui::Button("  Clients   "))  openOnly(gs.showClients);
    ImGui::SameLine();
    if (ImGui::Button("   Staff    "))  openOnly(gs.showStaff);
    ImGui::SameLine();
    if (ImGui::Button(" Templates  "))  openOnly(gs.showTemplates);
    ImGui::SameLine();
    if (ImGui::Button(" Market Map "))  openOnly(gs.showMarketMap);
    ImGui::SameLine();
    if (ImGui::Button(" Goals      "))  openOnly(gs.showGoals);
    ImGui::SameLine();
    if (ImGui::Button(" Specials   "))  openOnly(gs.showSpecializations);
    ImGui::SameLine();
    if (ImGui::Button(" Achievmts  "))  openOnly(gs.showAchievements);
    ImGui::SameLine();
    if (ImGui::Button(" Newsfeed   "))  openOnly(gs.showNewsfeed);
    ImGui::SameLine();
    if (ImGui::Button(" Report     "))  openOnly(gs.showReport);
    ImGui::SameLine();
    if (ImGui::Button(" [B]rand    "))  { g_showBranding = true; }        // v0.9
    ImGui::SameLine();
    if (ImGui::Button(" \xf0\x9f\x8f\x86 Board   "))  openOnly(gs.showLeaderboard);  // v0.8
    ImGui::SameLine();
    if (ImGui::Button(" \xe2\x9a\x99 Settings "))  openOnly(gs.showSettings);       // v0.8
    ImGui::SameLine(0, 20);

    ImVec4 budgetCol = gs.budget > 3000 ? UIStyle::Positive
                     : gs.budget > 0   ? UIStyle::Warning
                                       : UIStyle::Negative;
    ImGui::TextColored(budgetCol, "$%.0f", gs.budget);
    ImGui::SameLine(0, 10);
    ImGui::TextColored(UIStyle::Muted, "Mo %d/%d", gs.month, gs.year);
    ImGui::SameLine(0, 10);
    ImGui::TextColored(UIStyle::Gold, "%.1f%%", gs.playerMarketShare);
    ImGui::SameLine(0, 20);

    if (UIStyle::GreenButton(" >> Next Month ")) {
        pendingNextMonth = true;
        SOUND(ButtonClick);   // v0.8: audio feedback
    }
    ImGui::SameLine();
    if (ImGui::Button(" \xf0\x9f\x92\xbe Save ")) {
        openOnly(gs.showSaveSlots);
        SOUND(ButtonClick);   // v0.8
    }
    ImGui::End();
}

// ── Game render ──────────────────────────────────────────────────────────────
void RenderGame(float dt) {
    RenderNavbar();

    Dashboard::Render(gs);

    CampaignEditor::Render(gs);
    ClientManager::Render(gs);
    StaffPanel::Render(gs);
    TemplatesPanel::Render(gs);
    MarketMap::Render(gs);
    GoalsPanel::Render(gs);
    SpecializationPanel::Render(gs);
    AchievementsPanel::Render(gs);
    Newsfeed::Render(gs);
    ReportPanel::Render(gs);
    SaveSlotsPanel::Render(gs);
    LeaderboardPanel::Render(gs);          // v0.8
    SettingsPanel::Render(gs);             // v0.8

    NegotiationPanel::Render(gs);

    // ── v0.9: Agency Branding Panel ────────────────────────────────────────
    if (g_showBranding) {
        static int logoIdx = 0;
        if (g_branding.Render(gs.agencyName, gs.agencyColor, logoIdx)) {
            // Confirmed: apply branding data to gs
            gs.agencyLogo  = std::to_string(logoIdx);
            g_showBranding  = false;
            TOAST_SUCCESS("Branding updated!");
            SOUND(ButtonClick);
        }
    }

    // ── v0.9: Pending event popup ──────────────────────────────────────────
    if (gs.pendingEventPopup) {
        MarketEventData evt;
        evt.id = gs.currentEvent.id;
        evt.title = gs.currentEvent.title;
        evt.description = gs.currentEvent.description;
        evt.effectText = gs.currentEvent.impact;
        evt.category = "neutral"; // TODO: determine from event type
        evt.durationMonths = gs.currentEvent.durationMonths;
        g_eventPopup.Show(evt);
        gs.pendingEventPopup = false;
    }
    g_eventPopup.Render();                 // v0.9

    ToastSystem::Get().Update(dt);
    ToastSystem::Get().Render();
    AudioSystem::Get().Tick();             // v0.8: cleanup finished sources

    // ── v0.9: Victory Screen (replaces basic overlay) ──────────────────────
    if (gs.victory) {
        if (!g_victory.IsOpen()) {
            VictoryStats vstats;
            vstats.finalMarketShare = gs.playerMarketShare;
            vstats.totalRevenue = gs.stats.totalRevenue;
            vstats.monthsPlayed = gs.stats.monthsPlayed;
            vstats.clientsServed = gs.stats.clientsAcquired;
            vstats.achievementsEarned = 0; // TODO: count unlocked achievements
            vstats.campaignsRun = gs.stats.campaignsCompleted;
            vstats.bestMonthRevenue = gs.stats.bestMonthRevenue;
            vstats.agencyName = gs.agencyName;
            vstats.difficulty = "Normal";
            g_victory.Open(vstats);
            SOUND(Victory);
        }
        int vr = g_victory.Render();     // v0.9: animated confetti
        if (vr == 1) {
            // Play Again
            gs           = GameState();
            gameStarted  = false;
            MainMenu::s_showMenu = true;
        } else if (vr == 2) {
            // Submit to Leaderboard
            openOnly_v(gs.showLeaderboard);
        }
    }

    // ── Lose overlay ───────────────────────────────────────────────────────
    if (gs.gameOver) {
        ImVec2 c = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(c, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(440, 240));
        ImGui::Begin("GAME OVER", nullptr, ImGuiWindowFlags_NoDecoration);
        ImGui::SetWindowFontScale(2.2f);
        ImGui::TextColored(UIStyle::Negative, " GAME OVER");
        ImGui::SetWindowFontScale(1.0f);
        ImGui::Text("Agency bankrupt (budget < -$50,000)");
        ImGui::Text("Revenue earned:   $%.0f", gs.stats.totalRevenue);
        ImGui::Text("Months survived:  %d",    gs.stats.monthsPlayed);
        if (ImGui::Button("Try Again", ImVec2(-1, 40)))
            { gs = GameState(); gameStarted = false; MainMenu::s_showMenu = true; }
        ImGui::End();
    }
}

// ── Entry point ──────────────────────────────────────────────────────────────
int main() {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(
        1280, 720, "AdEmpire v0.9 \xe2\x80\x94 Marketing Tycoon", nullptr, nullptr);
    if (!window) return -1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    lastTime = glfwGetTime();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    Theme::ApplyDarkMarketing();

    // ── v0.8: init audio ──────────────────────────────────────────────────
    AudioSystem::Get().Init();

    // ── v0.9: splash runs first, before game loop blocks ─────────────────
    // g_splash renders inside the main loop below via IsDone() check

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        double now = glfwGetTime();
        float  dt  = (float)(now - lastTime);
        lastTime   = now;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ── v0.9: Splash gate ─────────────────────────────────────────────
        if (!g_splash.IsDone()) {
            g_splash.Render();
        }
        // ── Branding gate on first NewGame ────────────────────────────────
        else if (g_showBranding && !gameStarted) {
            static int logoIdx = 0;
            if (g_branding.Render(gs.agencyName, gs.agencyColor, logoIdx)) {
                gs.agencyLogo  = std::to_string(logoIdx);
                g_showBranding = false;
                // Now actually start the game
                AchievementSystem::Init(gs);
                TOAST_SUCCESS("Agency founded! Good luck.");
                SOUND(Notification);
                gameStarted = true;
            }
        }
        else if (!gameStarted) {
            if (MainMenu::Render(gs)) {
                // v0.9: show branding before entering game
                g_showBranding = true;
            }
        } else {
            if (pendingNextMonth) {
                SeasonalEvents::Apply(gs);
                EventSystem::TryTriggerEvent(gs);
                AICompetitor::ProcessTurn(gs);
                StaffLeveling::ProcessMonth(gs);
                AchievementSystem::Check(gs);
                ReportPanel::GenerateMonthlyReport(gs);
                Simulation::AdvanceMonth(gs);

                gs.showReport = true;
                for (bool* p : { &gs.showCampaigns, &gs.showClients, &gs.showStaff,
                                 &gs.showMarketMap, &gs.showNewsfeed,
                                 &gs.showAchievements, &gs.showGoals,
                                 &gs.showSpecializations, &gs.showTemplates,
                                 &gs.showSaveSlots, &gs.showLeaderboard,
                                 &gs.showSettings })
                    *p = false;

                pendingNextMonth = false;

                float profit = gs.monthlyRevenue - gs.monthlyExpenses;
                if (profit >= 0) {
                    TOAST_SUCCESS("Profitable month! +$" + std::to_string((int)profit));
                    SOUND(MonthAdvanced);          // v0.8
                } else {
                    TOAST_WARN("Monthly loss: -$" + std::to_string((int)(-profit)));
                    SOUND(Notification);           // v0.8
                }

                // v0.8/v0.9: victory/gameover sounds handled in RenderGame
                if (gs.gameOver) SOUND(GameOver);
            }
            RenderGame(dt);
        }

        ImGui::Render();
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);
        glClearColor(0.04f, 0.05f, 0.09f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    AudioSystem::Get().Shutdown();  // v0.8: clean OpenAL
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
