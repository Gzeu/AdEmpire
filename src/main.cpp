#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/GameState.h"
#include "core/Simulation.h"
#include "systems/EventSystem.h"
#include "systems/AICompetitor.h"
#include "systems/SaveSystem.h"
#include "ui/Theme.h"
#include "ui/MainMenu.h"
#include "ui/Dashboard.h"
#include "ui/CampaignEditor.h"
#include "ui/ClientManager.h"
#include "ui/MarketMap.h"
#include "ui/Newsfeed.h"
#include "ui/StaffPanel.h"

static GameState gs;
static bool      gameStarted      = false;
static bool      pendingNextMonth = false;

static void glfw_error_callback(int error, const char* desc) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, desc);
}

static void RenderNavbar() {
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, 40), ImGuiCond_Always);
    ImGui::Begin("##navbar", nullptr,
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar  | ImGuiWindowFlags_NoBringToFrontOnFocus);

    if (ImGui::Button(" Dashboard "))  { gs.showDashboard=true; gs.showCampaigns=false; gs.showClients=false; gs.showStaff=false; gs.showMarketMap=false; gs.showNewsfeed=false; }
    ImGui::SameLine();
    if (ImGui::Button(" Campaigns "))  { gs.showCampaigns=true; gs.showClients=false; gs.showStaff=false; gs.showMarketMap=false; gs.showNewsfeed=false; }
    ImGui::SameLine();
    if (ImGui::Button("  Clients  "))  { gs.showClients=true; gs.showCampaigns=false; gs.showStaff=false; gs.showMarketMap=false; gs.showNewsfeed=false; }
    ImGui::SameLine();
    if (ImGui::Button("   Staff   "))  { gs.showStaff=true; gs.showCampaigns=false; gs.showClients=false; gs.showMarketMap=false; gs.showNewsfeed=false; }
    ImGui::SameLine();
    if (ImGui::Button(" Market Map ")) { gs.showMarketMap=true; gs.showCampaigns=false; gs.showClients=false; gs.showStaff=false; gs.showNewsfeed=false; }
    ImGui::SameLine();
    if (ImGui::Button(" Newsfeed  "))  { gs.showNewsfeed=true; gs.showCampaigns=false; gs.showClients=false; gs.showStaff=false; gs.showMarketMap=false; }
    ImGui::SameLine(0, 24);

    ImGui::TextColored(ImVec4(0.3f,1.f,0.5f,1.f),
        "$%.0f | Mo %d/%d | Share %.1f%%",
        gs.budget, gs.month, gs.year, gs.playerMarketShare);
    ImGui::SameLine(0, 20);
    if (ImGui::Button(" Next Month >> ")) pendingNextMonth = true;
    ImGui::SameLine();
    if (ImGui::Button(" Save ")) SaveSystem::Save(gs);
    ImGui::End();
}

static void RenderGame() {
    RenderNavbar();
    Dashboard::Render(gs);
    CampaignEditor::Render(gs);
    ClientManager::Render(gs);
    MarketMap::Render(gs);
    Newsfeed::Render(gs);
    StaffPanel::Render(gs);

    if (gs.victory) {
        ImVec2 c = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(c, ImGuiCond_Always, ImVec2(0.5f,0.5f));
        ImGui::SetNextWindowSize(ImVec2(400,200));
        ImGui::Begin("##victory", nullptr, ImGuiWindowFlags_NoDecoration);
        ImGui::SetWindowFontScale(2.f);
        ImGui::TextColored(ImVec4(0.3f,1.f,0.4f,1.f), "  VICTORY!");
        ImGui::SetWindowFontScale(1.f);
        ImGui::Text("You reached 35%%%% market share!");
        ImGui::Text("Total Revenue: $%.0f", gs.stats.totalRevenue);
        if (ImGui::Button("Play Again", ImVec2(-1,40))) {
            gs = GameState(); gameStarted = false; MainMenu::s_showMenu = true;
        }
        ImGui::End();
    }
    if (gs.gameOver) {
        ImVec2 c = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(c, ImGuiCond_Always, ImVec2(0.5f,0.5f));
        ImGui::SetNextWindowSize(ImVec2(400,200));
        ImGui::Begin("##gameover", nullptr, ImGuiWindowFlags_NoDecoration);
        ImGui::SetWindowFontScale(2.f);
        ImGui::TextColored(ImVec4(1.f,0.3f,0.3f,1.f), "  GAME OVER");
        ImGui::SetWindowFontScale(1.f);
        ImGui::Text("Agency went bankrupt (< -$50,000)");
        if (ImGui::Button("Try Again", ImVec2(-1,40))) {
            gs = GameState(); gameStarted = false; MainMenu::s_showMenu = true;
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
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(1280, 720,
        "AdEmpire - Marketing Tycoon", nullptr, nullptr);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplGLFW_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    Theme::ApplyDarkMarketing();

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
