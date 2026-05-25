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

static GameState gs;
static bool      gameStarted      = false;
static bool      pendingNextMonth = false;

static void glfw_error_callback(int error, const char* desc) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, desc);
}

void RenderNavbar() {
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(1280, 40), ImGuiCond_Always);
    ImGui::Begin("##navbar", nullptr,
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBringToFrontOnFocus);

    auto closeAll = [&](){
        gs.showCampaigns=false; gs.showClients=false; gs.showStaff=false;
        gs.showMarketMap=false; gs.showNewsfeed=false; gs.showAchievements=false;
    };

    if (ImGui::Button(" Dashboard  "))  { closeAll(); gs.showDashboard=true; }
    ImGui::SameLine();
    if (ImGui::Button(" Campaigns  "))  { closeAll(); gs.showCampaigns=true; }
    ImGui::SameLine();
    if (ImGui::Button("  Clients   "))  { closeAll(); gs.showClients=true; }
    ImGui::SameLine();
    if (ImGui::Button("   Staff    "))  { closeAll(); gs.showStaff=true; }
    ImGui::SameLine();
    if (ImGui::Button(" Market Map "))  { closeAll(); gs.showMarketMap=true; }
    ImGui::SameLine();
    if (ImGui::Button(" Newsfeed   "))  { closeAll(); gs.showNewsfeed=true; }
    ImGui::SameLine();
    if (ImGui::Button(" 🏆 Achieve "))  { closeAll(); gs.showAchievements=true; }
    ImGui::SameLine();
    if (ImGui::Button(" ⚙ Settings "))  gs.showSettings = !gs.showSettings;
    ImGui::SameLine(0, 20);

    ImGui::TextColored(ImVec4(0.3f,1.f,0.5f,1.f),
        "$%.0f | Mo %d/%d | %.1f%% mkt",
        gs.budget, gs.month, gs.year, gs.playerMarketShare);
    ImGui::SameLine(0, 20);

    if (ImGui::Button(" Next Month >> ")) pendingNextMonth = true;
    ImGui::SameLine();
    if (ImGui::Button(" Save ")) SaveSystem::Save(gs);
    ImGui::End();
}

void RenderGame() {
    RenderNavbar();
    Dashboard::Render(gs);
    CampaignEditor::Render(gs);
    ClientManager::Render(gs);
    MarketMap::Render(gs);
    Newsfeed::Render(gs);
    StaffPanel::Render(gs);
    AchievementsPanel::Render(gs);
    SettingsPanel::Render(gs);
    AchievementsPanel::RenderPopupOverlay();

    // Win overlay
    if (gs.victory) {
        ImVec2 c = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(c, ImGuiCond_Always, ImVec2(0.5f,0.5f));
        ImGui::SetNextWindowSize(ImVec2(420, 220));
        ImGui::Begin("VICTORY", nullptr, ImGuiWindowFlags_NoDecoration);
        ImGui::SetWindowFontScale(2.f);
        ImGui::TextColored(ImVec4(0.3f,1.f,0.4f,1.f), "  🏆 VICTORY!");
        ImGui::SetWindowFontScale(1.f);
        ImGui::Text("You reached 35%% market share!");
        ImGui::Text("Total Revenue: $%.0f", gs.stats.totalRevenue);
        ImGui::Text("Campaigns Done: %d", gs.stats.campaignsCompleted);
        ImGui::Text("Months Played:  %d", gs.stats.monthsPlayed);
        if (ImGui::Button("Play Again", ImVec2(-1,40))) {
            gs = GameState();
            AchievementsSystem::Init();
            gameStarted = false;
            MainMenu::s_showMenu = true;
        }
        ImGui::End();
    }
    // Game over overlay
    if (gs.gameOver) {
        ImVec2 c = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(c, ImGuiCond_Always, ImVec2(0.5f,0.5f));
        ImGui::SetNextWindowSize(ImVec2(400, 200));
        ImGui::Begin("GAME OVER", nullptr, ImGuiWindowFlags_NoDecoration);
        ImGui::SetWindowFontScale(2.f);
        ImGui::TextColored(ImVec4(1.f,0.3f,0.3f,1.f), "  GAME OVER");
        ImGui::SetWindowFontScale(1.f);
        ImGui::Text("Agency went bankrupt (< -$50,000)");
        ImGui::Text("Months survived: %d", gs.stats.monthsPlayed);
        if (ImGui::Button("Try Again", ImVec2(-1,40))) {
            gs = GameState();
            AchievementsSystem::Init();
            gameStarted = false;
            MainMenu::s_showMenu = true;
        }
        ImGui::End();
    }
    // FPS counter
    if (SettingsPanel::s_settings.showFPS) {
        ImGui::SetNextWindowPos(ImVec2(1150, 700));
        ImGui::SetNextWindowBgAlpha(0.4f);
        ImGui::Begin("##fps", nullptr,
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs |
            ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("%.0f FPS", ImGui::GetIO().Framerate);
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
        "AdEmpire — Marketing Tycoon", nullptr, nullptr);
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
    AchievementsSystem::Init();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGLFW_NewFrame();
        ImGui::NewFrame();

        if (!gameStarted) {
            if (MainMenu::Render(gs)) gameStarted = true;
        } else {
            if (pendingNextMonth) {
                EventSystem::TryTriggerEvent(gs);
                AICompetitor::ProcessTurn(gs);
                Simulation::AdvanceMonth(gs);
                AchievementsSystem::Check(gs);
                // Auto-save
                if (SettingsPanel::s_settings.autoSave &&
                    gs.stats.monthsPlayed % SettingsPanel::s_settings.autoSaveInterval == 0)
                    SaveSystem::Save(gs, SettingsPanel::s_settings.saveSlot);
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
