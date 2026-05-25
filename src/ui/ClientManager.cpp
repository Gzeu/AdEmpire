#include "ClientManager.h"
#include "imgui.h"
#include <algorithm>
#include <cstdlib>
#include <cstdio>

void ClientManager::Render(GameState& gs) {
    if (!gs.showClients) return;
    ImGui::SetNextWindowPos(ImVec2(325, 40), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(700, 560), ImGuiCond_Always);
    ImGui::Begin("Clients", &gs.showClients,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    if (ImGui::BeginTabBar("##clienttabs")) {
        if (ImGui::BeginTabItem("My Clients")) {
            bool anyActive = false;
            for (auto& cl : gs.clients) {
                if (!cl.active) continue;
                anyActive = true;
                ImGui::PushID(cl.id);
                ImGui::Text("%s", cl.name.c_str());
                ImGui::SameLine(200);
                ImGui::TextColored(ImVec4(0.7f,0.7f,1.f,1.f), "%s",
                    IndustryNames[static_cast<int>(cl.industry)]);
                ImGui::SameLine(370);
                ImGui::Text("Budget: $%.0f/mo", cl.budget);
                ImVec4 satCol =
                    cl.satisfaction > 70.f ? ImVec4(0.3f,1.f,0.4f,1.f) :
                    cl.satisfaction > 40.f ? ImVec4(1.f,0.8f,0.2f,1.f) :
                                             ImVec4(1.f,0.3f,0.3f,1.f);
                char satLabel[32];
                snprintf(satLabel, sizeof(satLabel), "Sat: %.0f%%", cl.satisfaction);
                ImGui::ProgressBar(cl.satisfaction / 100.f, ImVec2(180,14), satLabel);
                ImGui::SameLine();
                ImGui::TextColored(satCol, "Contract: %dmo left", cl.contractMonths);
                ImGui::Separator();
                ImGui::PopID();
            }
            if (!anyActive)
                ImGui::TextDisabled("No active clients. Pitch some from Available tab!");
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Available (Pitch)")) {
            ImGui::TextColored(ImVec4(0.8f,0.8f,0.3f,1.f),
                "Win chance scales with your agency reputation.");
            ImGui::Spacing();
            bool anyAvail = false;
            for (auto& cl : gs.clients) {
                if (!cl.available || cl.active) continue;
                anyAvail = true;
                ImGui::PushID(cl.id);
                ImGui::Text("%s  |  %s  |  Budget: $%.0f/mo",
                    cl.name.c_str(),
                    IndustryNames[static_cast<int>(cl.industry)],
                    cl.budget);
                ImGui::SameLine(500);
                float winChance = std::min(gs.stats.reputation / 100.f * 0.8f + 0.2f, 0.95f);
                ImGui::TextColored(ImVec4(0.4f,1.f,0.6f,1.f), "Win: %.0f%%", winChance * 100.f);
                ImGui::SameLine();
                if (ImGui::Button("Pitch!")) {
                    if (static_cast<float>(rand() % 100) / 100.f < winChance) {
                        cl.active    = true;
                        cl.available = false;
                        gs.stats.clientsAcquired++;
                        gs.playerMarketShare += 1.5f;
                    }
                }
                ImGui::Separator();
                ImGui::PopID();
            }
            if (!anyAvail)
                ImGui::TextDisabled("No clients available to pitch right now.");
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::End();
}
