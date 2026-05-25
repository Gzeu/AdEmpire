#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <cstdio>
#include <cstdlib>

#include "core/GameState.h"
#include "core/Simulation.h"
#include "systems/EventSystem.h"
#include "systems/AICompetitor.h"
#include "systems/SaveSystem.h"
#include "systems/AchievementsSystem.h"
#include "systems/FitScoreSystem.h"
#include "systems/GoalSystem.h"
#include "systems/SpecializationSystem.h"
#include "ui/Theme.h"
#include "ui/MainMenu.h"
#include "ui/Dashboard.h"
#include "ui/CampaignEditor.h"
#include "ui/ClientManager.h"
#include "ui/MarketMap.h"
#include "ui/Newsfeed.h"
#include "ui/StaffPanel.h"
#include "ui/AchievementsPanel.h"
#include "ui/SettingsPanel.h"
#include "ui/NegotiationPanel.h"
#include "ui/GoalsPanel.h"
#include "ui/SpecializationPanel.h"

static GameState gs;
static bool      gameStarted       = false;
static bool      pendingNextMonth  = false;
static bool      confirmDialogOpen = false;

static void glfw_error_callback(int error, const char* desc) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, desc);
}

void RenderNavbar() {
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(1280, 40), ImGuiCond_Always);
    ImGui::Begin("##navbar", nullptr,
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBringToFrontOnFocus);

    auto navBtn = [&](const char* label, bool& flag) {
        if (ImGui::Button(label)) {
            gs.showDashboard = gs.showCampaigns = gs.showClients =
            gs.showStaff = gs.showMarketMap = gs.showNewsfeed =
            gs.showSettings = gs.showAchievements =
            gs.showGoals = gs.showSpecializations = gs.showNegotiation = false;
            flag = true;
        }
        ImGui::SameLine();
    };

    navBtn(" Dashboard ",      gs.showDashboard);
    navBtn(" Campaigns ",      gs.showCampaigns);
    navBtn(" Clients ",        gs.showClients);
    navBtn(" Staff ",          gs.showStaff);
    navBtn(" Market Map ",     gs.showMarketMap);
    navBtn(" Newsfeed ",       gs.showNewsfeed);
    navBtn(" Goals ",          gs.showGoals);
    navBtn(" Specializations ",gs.showSpecializations);
    navBtn(" \xF0\x9F\x8F\x86 Achieve ",   gs.showAchievements);
    navBtn(" \xE2\x9A\x99 Settings ",       gs.showSettings);

    ImGui::TextColored(ImVec4(0.3f,1.f,0.5f,1.f),
        "   $%.0f | Mo %d/%d | Share %.1f%%",
        gs.budget, gs.month, gs.year, gs.playerMarketShare);
    ImGui::SameLine(0, 20);

    if (ImGui::Button(" >> Next Month ")) {
        if (SettingsPanel::s_data.confirmNextMonth)
            confirmDialogOpen = true;
        else
            pendingNextMonth = true;
    }
    ImGui::SameLine();
    if (ImGui::Button(" Save "))
        SaveSystem::Save(gs);
    ImGui::End();
}

void RenderConfirmDialog() {
    if (!confirmDialogOpen) return;
    ImVec2 c = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(c, ImGuiCond_Always, ImVec2(0.5f,0.5f));
    ImGui::SetNextWindowSize(ImVec2(320,120));
    ImGui::Begin("Confirm", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    ImGui::Text("Advance to next month?");
    ImGui::Spacing();
    if (ImGui::Button("Yes", ImVec2(120,32))) { pendingNextMonth = true; confirmDialogOpen = false; }
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(120,32))) confirmDialogOpen = false;
    ImGui::End();
}

void RenderGame() {
    RenderNavbar();
    RenderConfirmDialog();
    Dashboard::Render(gs);
    CampaignEditor::Render(gs);
    ClientManager::Render(gs);
    MarketMap::Render(gs);
    Newsfeed::Render(gs);
    StaffPanel::Render(gs);
    AchievementsPanel::Render(gs);
    SettingsPanel::Render(gs);
    AchievementsSystem::RenderPopups();
    NegotiationPanel::Render(gs);
    GoalsPanel::Render(gs);
    SpecializationPanel::Render(gs);

    // FPS overlay
    if (SettingsPanel::s_data.showFPS) {
        ImGui::SetNextWindowPos(ImVec2(1180, 44), ImGuiCond_Always);
        ImGui::SetNextWindowBgAlpha(0.4f);
        ImGui::Begin("##fps", nullptr,
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs |
            ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);
        ImGui::TextColored(ImVec4(0.4f,1.f,0.4f,1.f),
            "FPS %.0f", ImGui::GetIO().Framerate);
        ImGui::End();
    }

    // Win overlay
    if (gs.victory) {
        ImVec2 c = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(c, ImGuiCond_Always, ImVec2(0.5f,0.5f));
        ImGui::SetNextWindowSize(ImVec2(460,260));
        ImGui::Begin("VICTORY!", nullptr, ImGuiWindowFlags_NoDecoration);
        ImGui::SetWindowFontScale(2.2f);
        ImGui::TextColored(ImVec4(0.3f,1.f,0.4f,1.f), "  VICTORY!");
        ImGui::SetWindowFontScale(1.f);
        ImGui::Text("You reached 35%% market share!");
        ImGui::Text("Total Revenue:    $%.0f", gs.stats.totalRevenue);
        ImGui::Text("Months played:    %d",    gs.stats.monthsPlayed);
        ImGui::Text("Clients acquired: %d",    gs.stats.clientsAcquired);
        ImGui::Text("Best month:       $%.0f", gs.stats.bestMonthRevenue);
        ImGui::Spacing();
        if (ImGui::Button("Play Again", ImVec2(-1,40))) {
            gs = GameState();
            gameStarted = false;
            MainMenu::s_showMenu = true;
        }
        ImGui::End();
    }

    // Game Over overlay
    if (gs.gameOver && !gs.victory) {
        ImVec2 c = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(c, ImGuiCond_Always, ImVec2(0.5f,0.5f));
        ImGui::SetNextWindowSize(ImVec2(420,200));
        ImGui::Begin("GAME OVER", nullptr, ImGuiWindowFlags_NoDecoration);
        ImGui::SetWindowFontScale(2.2f);
        ImGui::TextColored(ImVec4(1.f,0.3f,0.3f,1.f), " GAME OVER");
        ImGui::SetWindowFontScale(1.f);
        ImGui::Text("Budget went below -$50,000.");
        ImGui::Text("Revenue earned: $%.0f", gs.stats.totalRevenue);
        ImGui::Spacing();
        if (ImGui::Button("Try Again", ImVec2(-1,40))) {
            gs = GameState();
            gameStarted = false;
            MainMenu::s_showMenu = true;
        }
        ImGui::End();
    }
}

int main() {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 720,
        "AdEmpire \u2014 Marketing Tycoon v0.2", nullptr, nullptr);
    if (!window) return -1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplGLFW_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    Theme::ApplyDarkMarketing();

    int monthsSinceAutoSave = 0;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGLFW_NewFrame();
        ImGui::NewFrame();

        if (!gameStarted) {
            if (MainMenu::Render(gs)) {
                // v0.2 systems init on new game
                GoalSystem::InitQuarterlyGoals(gs);
                SpecializationSystem::Init(gs);
                gameStarted = true;
            }
        } else {
            if (pendingNextMonth) {
                EventSystem::TryTriggerEvent(gs);
                AICompetitor::ProcessTurn(gs);
                Simulation::AdvanceMonth(gs);
                AchievementsSystem::CheckAll(gs);
                // v0.2 monthly updates
                FitScoreSystem::UpdateCapacity(gs);
                GoalSystem::UpdateGoals(gs);
                GoalSystem::CheckCompletion(gs);
                SpecializationSystem::Update(gs);
                // Auto-save
                monthsSinceAutoSave++;
                if (SettingsPanel::s_data.autoSave &&
                    monthsSinceAutoSave >= SettingsPanel::s_data.autoSaveMonths) {
                    SaveSystem::Save(gs);
                    monthsSinceAutoSave = 0;
                }
                pendingNextMonth = false;
            }
            RenderGame();
        }

        ImGui::Render();
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);
        glClearColor(0.04f, 0.05f, 0.09f, 1.0f);
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
