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
#include "systems/AchievementSystem.h"
#include "systems/StaffLeveling.h"
#include "systems/SeasonalEvents.h"

// ── v0.8 Systems ─────────────────────────────────────────────────────────────
#include "audio/AudioSystem.h"
#include "network/LeaderboardClient.h"

// ── v1.0 Market Systems ──────────────────────────────────────────────────────
#include "network/MarketFeed.h"          // v1.0: CoinGecko + Fear&Greed + FX
#include "network/MarketEventBridge.h"   // v1.0: real market → game events

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
#include "ui/AchievementsPanel.h"
#include "ui/TemplatesPanel.h"
#include "ui/SaveSlotsPanel.h"
#include "ui/LeaderboardPanel.h"
#include "ui/SettingsPanel.h"
#include "ui/NegotiationPanel.h"
#include "ui/GoalsPanel.h"
#include "ui/SpecializationPanel.h"
#include "ui/SplashScreen.h"
#include "ui/VictoryScreen.h"
#include "ui/EventPopup.h"
#include "ui/AgencyBrandingPanel.h"
#include "ui/LiveMarketPanel.h"          // v1.0: Live Market tab

static GameState  gs;
static bool       gameStarted      = false;
static bool       pendingNextMonth = false;
static double     lastTime         = 0.0;

// ── v1.0: Market feed timer ──────────────────────────────────────────────────
static double     marketFetchTimer = 0.0;
constexpr double  MARKET_FETCH_INTERVAL = 300.0; // 5 minutes

// ── v0.9 global UI objects ───────────────────────────────────────────────────
static SplashScreen        g_splash;
static VictoryScreen       g_victory;
static EventPopup          g_eventPopup;
static AgencyBrandingPanel g_branding;
static bool                g_showBranding = false;

static void glfw_error_callback(int error, const char* desc) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, desc);
}

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
    if (ImGui::Button(" [B]rand    "))  { g_showBranding = true; }
    ImGui::SameLine();
    if (ImGui::Button(" \xf0\x9f\x8f\x86 Board   "))  openOnly(gs.showLeaderboard);
    ImGui::SameLine();
    if (ImGui::Button(" \xe2\x9a\x99 Settings "))  openOnly(gs.showSettings);
    ImGui::SameLine(0, 20);

    // ── v1.0: Live market indicator in navbar ──────────────────────────────
    const auto& ms = MarketFeed::Get().GetState();
    ImVec4 mktCol = ms.btcChange24h > 2.0f  ? ImVec4(0.2f, 0.9f, 0.4f, 1.0f)
                  : ms.btcChange24h < -2.0f ? ImVec4(0.9f, 0.3f, 0.3f, 1.0f)
                  :                           ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
    ImGui::TextColored(mktCol, "BTC %+.1f%%", ms.btcChange24h);
    ImGui::SameLine(0, 6);
    ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "|");
    ImGui::SameLine(0, 6);
    // Fear & Greed color: red <25, orange <45, yellow <55, green >55
    ImVec4 fgCol = ms.fearGreedIndex < 25 ? ImVec4(0.9f, 0.2f, 0.2f, 1.0f)
                 : ms.fearGreedIndex < 45 ? ImVec4(0.9f, 0.6f, 0.1f, 1.0f)
                 : ms.fearGreedIndex < 55 ? ImVec4(0.9f, 0.9f, 0.2f, 1.0f)
                 :                          ImVec4(0.2f, 0.85f, 0.4f, 1.0f);
    ImGui::TextColored(fgCol, "F&G %d", ms.fearGreedIndex);
    ImGui::SameLine(0, 12);

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
        SOUND(ButtonClick);
    }
    ImGui::SameLine();
    if (ImGui::Button(" \xf0\x9f\x92\xbe Save ")) {
        openOnly(gs.showSaveSlots);
        SOUND(ButtonClick);
    }
    ImGui::End();
}

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
    LeaderboardPanel::Render(gs);
    SettingsPanel::Render(gs);
    NegotiationPanel::Render(gs);

    if (g_showBranding) {
        static int logoIdx = 0;
        if (g_branding.Render(gs.agencyName, gs.agencyColor, logoIdx)) {
            gs.agencyLogo  = std::to_string(logoIdx);
            g_showBranding  = false;
            TOAST_SUCCESS("Branding updated!");
            SOUND(ButtonClick);
        }
    }

    if (gs.pendingEventPopup) {
        MarketEventData evt;
        evt.id = gs.currentEvent.id;
        evt.title = gs.currentEvent.title;
        evt.description = gs.currentEvent.description;
        evt.effectText = gs.currentEvent.impact;
        evt.category = "neutral";
        evt.durationMonths = gs.currentEvent.durationMonths;
        g_eventPopup.Show(evt);
        gs.pendingEventPopup = false;
    }
    g_eventPopup.Render();

    ToastSystem::Get().Update(dt);
    ToastSystem::Get().Render();
    AudioSystem::Get().Tick();

    if (gs.victory) {
        if (!g_victory.IsOpen()) {
            VictoryStats vstats;
            vstats.finalMarketShare = gs.playerMarketShare;
            vstats.totalRevenue = gs.stats.totalRevenue;
            vstats.monthsPlayed = gs.stats.monthsPlayed;
            vstats.clientsServed = gs.stats.clientsAcquired;
            vstats.achievementsEarned = 0;
            vstats.campaignsRun = gs.stats.campaignsCompleted;
            vstats.bestMonthRevenue = gs.stats.bestMonthRevenue;
            vstats.agencyName = gs.agencyName;
            vstats.difficulty = "Normal";
            g_victory.Open(vstats);
            SOUND(Victory);
        }
        int vr = g_victory.Render();
        if (vr == 1) {
            gs           = GameState();
            gameStarted  = false;
            MainMenu::s_showMenu = true;
        } else if (vr == 2) {
            openOnly_v(gs.showLeaderboard);
        }
    }

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

int main() {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(
        1280, 720, "AdEmpire v1.0 \xe2\x80\x94 Marketing Tycoon", nullptr, nullptr);
    if (!window) return -1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    lastTime = glfwGetTime();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;  // v1.0: docking
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    Theme::Init();  // v1.0: Roboto + full palette (replaces ApplyDarkMarketing)

    AudioSystem::Get().Init();

    // ── v1.0: Initial market fetch (async, non-blocking) ──────────────────
    MarketFeed::Get().FetchAsync();
    marketFetchTimer = 0.0;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        double now = glfwGetTime();
        float  dt  = (float)(now - lastTime);
        lastTime   = now;

        // ── v1.0: Refresh market data every 5 minutes ─────────────────────
        marketFetchTimer += dt;
        if (marketFetchTimer >= MARKET_FETCH_INTERVAL) {
            MarketFeed::Get().FetchAsync();
            marketFetchTimer = 0.0;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (!g_splash.IsDone()) {
            g_splash.Render();
        }
        else if (g_showBranding && !gameStarted) {
            static int logoIdx = 0;
            if (g_branding.Render(gs.agencyName, gs.agencyColor, logoIdx)) {
                gs.agencyLogo  = std::to_string(logoIdx);
                g_showBranding = false;
                AchievementSystem::Init(gs);
                TOAST_SUCCESS("Agency founded! Good luck.");
                SOUND(Notification);
                gameStarted = true;
            }
        }
        else if (!gameStarted) {
            if (MainMenu::Render(gs)) {
                g_showBranding = true;
            }
        } else {
            if (pendingNextMonth) {
                // ── v1.0: Apply real market modifiers before advancing ─────
                MarketEventBridge::Get().EvaluateAndTrigger(gs);
                float revMult = MarketEventBridge::Get().GetRevenueMultiplier();
                gs.revenueMultiplier = revMult;  // applied in Simulation::AdvanceMonth

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

                // ── Toast with market context ──────────────────────────────
                float profit = gs.monthlyRevenue - gs.monthlyExpenses;
                std::string mktNote = "";
                if (revMult > 1.3f) mktNote = " (Market: BULL +" + std::to_string((int)((revMult-1)*100)) + "%)";
                if (revMult < 0.8f) mktNote = " (Market: BEAR " + std::to_string((int)((revMult-1)*100)) + "%)";
                if (profit >= 0) {
                    TOAST_SUCCESS("Profitable month! +$" + std::to_string((int)profit) + mktNote);
                    SOUND(MonthAdvanced);
                } else {
                    TOAST_WARN("Monthly loss: -$" + std::to_string((int)(-profit)) + mktNote);
                    SOUND(Notification);
                }

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

    AudioSystem::Get().Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
