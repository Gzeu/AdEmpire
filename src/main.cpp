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

// ── NegotiationPanel / GoalsPanel / SpecializationPanel (v0.2) ───────────────
#include "ui/NegotiationPanel.h"
#include "ui/GoalsPanel.h"
#include "ui/SpecializationPanel.h"

static GameState gs;
static bool      gameStarted      = false;
static bool      pendingNextMonth = false;
static double    lastTime         = 0.0;

static void glfw_error_callback(int error, const char* desc) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, desc);
}

// ── Navbar ───────────────────────────────────────────────────────────────────
void RenderNavbar() {
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(1280, 40), ImGuiCond_Always);
    ImGui::Begin("##navbar", nullptr,
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBringToFrontOnFocus);

    // Helper: open one panel, close all others
    auto openOnly = [&](bool& target) {
        bool* all[] = {
            &gs.showCampaigns, &gs.showClients, &gs.showStaff,
            &gs.showMarketMap, &gs.showNewsfeed, &gs.showReport,
            &gs.showAchievements, &gs.showGoals, &gs.showSpecializations,
            &gs.showNegotiation, &gs.showTemplates, &gs.showSaveSlots
        };
        for (auto* p : all) *p = false;
        target = true;
    };

    if (ImGui::Button(" Dashboard  "))  { gs.showDashboard = true; }
    ImGui::SameLine();
    if (ImGui::Button(" Campaigns  "))  openOnly(gs.showCampaigns);
    ImGui::SameLine();
    if (ImGui::Button("  Clients   "))  openOnly(gs.showClients);
    ImGui::SameLine();
    if (ImGui::Button("   Staff    "))  openOnly(gs.showStaff);
    ImGui::SameLine();
    if (ImGui::Button(" Templates  "))  openOnly(gs.showTemplates);   // v0.5
    ImGui::SameLine();
    if (ImGui::Button(" Market Map "))  openOnly(gs.showMarketMap);
    ImGui::SameLine();
    if (ImGui::Button(" Goals      "))  openOnly(gs.showGoals);
    ImGui::SameLine();
    if (ImGui::Button(" Specials   "))  openOnly(gs.showSpecializations);
    ImGui::SameLine();
    if (ImGui::Button(" Achievmts  "))  openOnly(gs.showAchievements); // v0.5
    ImGui::SameLine();
    if (ImGui::Button(" Newsfeed   "))  openOnly(gs.showNewsfeed);
    ImGui::SameLine();
    if (ImGui::Button(" Report     "))  openOnly(gs.showReport);
    ImGui::SameLine(0, 20);

    // Live budget strip
    ImVec4 budgetCol = gs.budget > 3000 ? UIStyle::Positive
                     : gs.budget > 0   ? UIStyle::Warning
                                       : UIStyle::Negative;
    ImGui::TextColored(budgetCol, "$%.0f", gs.budget);
    ImGui::SameLine(0, 10);
    ImGui::TextColored(UIStyle::Muted, "Mo %d/%d", gs.month, gs.year);
    ImGui::SameLine(0, 10);
    ImGui::TextColored(UIStyle::Gold, "%.1f%%", gs.playerMarketShare);
    ImGui::SameLine(0, 20);

    if (UIStyle::GreenButton(" >> Next Month ")) pendingNextMonth = true;
    ImGui::SameLine();
    if (ImGui::Button(" 💾 Save "))  openOnly(gs.showSaveSlots);     // v0.5: opens slot panel
    ImGui::End();
}

// ── Game render ──────────────────────────────────────────────────────────────
void RenderGame(float dt) {
    RenderNavbar();

    // Always-visible sidebar
    Dashboard::Render(gs);

    // Main panel (mutually exclusive)
    CampaignEditor::Render(gs);
    ClientManager::Render(gs);
    StaffPanel::Render(gs);
    TemplatesPanel::Render(gs);        // v0.5
    MarketMap::Render(gs);
    GoalsPanel::Render(gs);
    SpecializationPanel::Render(gs);
    AchievementsPanel::Render(gs);     // v0.5
    Newsfeed::Render(gs);
    ReportPanel::Render(gs);
    SaveSlotsPanel::Render(gs);        // v0.5

    // Overlay panels (triggered contextually)
    NegotiationPanel::Render(gs);

    // Toasts always on top
    ToastSystem::Get().Update(dt);
    ToastSystem::Get().Render();

    // ── Win overlay ────────────────────────────────────────────────────────
    if (gs.victory) {
        ImVec2 c = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(c, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(440, 240));
        ImGui::Begin("VICTORY!", nullptr, ImGuiWindowFlags_NoDecoration);
        ImGui::SetWindowFontScale(2.2f);
        ImGui::TextColored(UIStyle::Positive, "  VICTORY!");
        ImGui::SetWindowFontScale(1.0f);
        ImGui::Text("You reached 35%% market share!");
        ImGui::Text("Total Revenue:  $%.0f", gs.stats.totalRevenue);
        ImGui::Text("Months played:  %d",    gs.stats.monthsPlayed);
        if (UIStyle::GreenButton("Play Again", ImVec2(-1, 40)))
            { gs = GameState(); gameStarted = false; MainMenu::s_showMenu = true; }
        ImGui::End();
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
        1280, 720, "AdEmpire v0.6 \u2014 Marketing Tycoon", nullptr, nullptr);
    if (!window) return -1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    lastTime = glfwGetTime();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui_ImplGLFW_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    Theme::ApplyDarkMarketing();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        double now = glfwGetTime();
        float  dt  = (float)(now - lastTime);
        lastTime   = now;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGLFW_NewFrame();
        ImGui::NewFrame();

        if (!gameStarted) {
            if (MainMenu::Render(gs)) {
                gameStarted = true;
                // ── v0.5 system init ──────────────────────────────────────
                AchievementSystem::Init(gs);
                TOAST_SUCCESS("Agency founded! Good luck.");
            }
        } else {
            if (pendingNextMonth) {
                // ── Monthly tick order ─────────────────────────────────────
                SeasonalEvents::Apply(gs);               // v0.5: seasonal mods
                EventSystem::TryTriggerEvent(gs);
                AICompetitor::ProcessTurn(gs);
                StaffLeveling::ProcessMonth(gs);         // v0.5: skill growth + promotions
                AchievementSystem::Check(gs);            // v0.5: achievement eval
                ReportPanel::GenerateReport(gs);         // snapshot before advance
                Simulation::AdvanceMonth(gs);

                // Auto-open report
                gs.showReport = true;
                for (bool* p : { &gs.showCampaigns, &gs.showClients, &gs.showStaff,
                                 &gs.showMarketMap, &gs.showNewsfeed,
                                 &gs.showAchievements, &gs.showGoals,
                                 &gs.showSpecializations, &gs.showTemplates,
                                 &gs.showSaveSlots })
                    *p = false;

                pendingNextMonth = false;

                // Toast profit summary
                float profit = gs.monthlyRevenue - gs.monthlyExpenses;
                if (profit >= 0)
                    TOAST_SUCCESS("Profitable month! +$" + std::to_string((int)profit));
                else
                    TOAST_WARN("Monthly loss: -$" + std::to_string((int)(-profit)));
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

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGLFW_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
