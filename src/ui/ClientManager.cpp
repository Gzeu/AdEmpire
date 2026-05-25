#include "ClientManager.h"
#include "imgui.h"
#include <algorithm>
#include <cstdio>
#include <cstdlib>

void ClientManager::Render(GameState& gs) {
    if (!gs.showClients) return;
    ImGui::SetNextWindowPos(ImVec2(325, 40), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(700, 560), ImGuiCond_Always);
    ImGui::Begin("Clients", &gs.showClients,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    if (ImGui::BeginTabBar("##clienttabs")) {
        // ── My Clients ──────────────────────────────────────────────────────
        if (ImGui::BeginTabItem("My Clients")) {
            int activeCount = 0;
            for (auto& cl : gs.clients) if (cl.active) activeCount++;
            ImGui::Text("Active clients: %d", activeCount);
            ImGui::Separator();

            for (auto& cl : gs.clients) {
                if (!cl.active) continue;
                ImGui::PushID(cl.id);
                ImGui::Text("%s", cl.name.c_str());
                ImGui::SameLine(200);
                ImGui::TextColored(ImVec4(0.7f,0.7f,1.f,1.f),
                    "%s", IndustryNames[(int)cl.industry]);
                ImGui::SameLine(370);
                ImGui::Text("Budget: $%.0f/mo", cl.budget);

                float sat = cl.satisfaction;
                ImVec4 satCol = sat > 70.f ? ImVec4(0.3f,1.f,0.4f,1.f)
                              : sat > 40.f ? ImVec4(1.f,0.8f,0.2f,1.f)
                              :              ImVec4(1.f,0.3f,0.3f,1.f);
                char satLabel[32];
                snprintf(satLabel, 32, "Sat: %.0f%%", sat);
                ImGui::ProgressBar(sat / 100.f, ImVec2(180, 14), satLabel);
                ImGui::SameLine();
                ImGui::TextColored(satCol, "Contract: %dmo left", cl.contractMonths);
                ImGui::Separator();
                ImGui::PopID();
            }
            ImGui::EndTabItem();
        }

        // ── Available Clients (Pitch) ────────────────────────────────────────
        if (ImGui::BeginTabItem("Available (Pitch)")) {
            ImGui::TextColored(ImVec4(0.8f,0.8f,0.3f,1.f),
                "Win clients by pitching. Success chance depends on your reputation.");
            ImGui::Spacing();

            for (auto& cl : gs.clients) {
                if (!cl.available || cl.active) continue;
                ImGui::PushID(cl.id);
                ImGui::Text("%s  |  %s  |  Budget: $%.0f/mo",
                    cl.name.c_str(),
                    IndustryNames[(int)cl.industry],
                    cl.budget);
                ImGui::SameLine(500);
                float winChance = std::min(gs.stats.reputation / 100.f * 0.8f + 0.20f, 0.95f);
                ImGui::TextColored(ImVec4(0.4f,1.f,0.6f,1.f),
                    "Win: %.0f%%", winChance * 100.f);
                ImGui::SameLine();
                if (ImGui::Button("Pitch!")) {
                    if ((float)(rand() % 100) / 100.f < winChance) {
                        cl.active    = true;
                        cl.available = false;
                        gs.stats.clientsAcquired++;
                        gs.playerMarketShare = std::min(gs.playerMarketShare + 1.5f, 100.f);
                    }
                }
                ImGui::Separator();
                ImGui::PopID();
            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::End();
}
