#include "ClientManager.h"
#include "imgui.h"
#include <cstdio>
#include <algorithm>
#include <cstdlib>

void ClientManager::Render(GameState& gs) {
    if (!gs.showClients) return;
    ImGui::SetNextWindowPos(ImVec2(325, 40), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(950, 640), ImGuiCond_Always);
    ImGui::Begin("Clients", &gs.showClients,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    if (ImGui::BeginTabBar("##clienttabs")) {
        // My Clients
        if (ImGui::BeginTabItem("My Clients")) {
            int active = 0;
            for (auto& c : gs.clients) if (c.active) active++;
            ImGui::Text("Active clients: %d", active);
            ImGui::Separator();
            for (auto& cl : gs.clients) {
                if (!cl.active) continue;
                ImGui::PushID(cl.id);
                ImGui::Text("%-24s", cl.name.c_str());
                ImGui::SameLine(240);
                ImGui::TextColored(ImVec4(0.7f,0.7f,1.f,1.f), "%-16s", IndustryNames[(int)cl.industry]);
                ImGui::SameLine(380);
                ImGui::Text("$%.0f/mo", cl.budget);
                ImGui::SameLine(480);
                ImVec4 satCol = cl.satisfaction > 70 ? ImVec4(0.3f,1.f,0.4f,1.f)
                              : cl.satisfaction > 40 ? ImVec4(1.f,0.8f,0.2f,1.f)
                              : ImVec4(1.f,0.3f,0.3f,1.f);
                char satLabel[32];
                snprintf(satLabel, 32, "Sat: %.0f%%", cl.satisfaction);
                ImGui::PushStyleColor(ImGuiCol_PlotHistogram, satCol);
                ImGui::ProgressBar(cl.satisfaction / 100.f, ImVec2(160, 16), satLabel);
                ImGui::PopStyleColor();
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(0.6f,0.9f,1.f,1.f), "Contract: %dmo", cl.contractMonths);
                ImGui::Separator();
                ImGui::PopID();
            }
            if (active == 0)
                ImGui::TextDisabled("No active clients. Go pitch some!");
            ImGui::EndTabItem();
        }
        // Available Clients
        if (ImGui::BeginTabItem("Pitch Clients")) {
            ImGui::TextColored(ImVec4(0.8f,0.8f,0.3f,1.f),
                "Win clients by pitching. Success depends on agency reputation.");
            ImGui::Spacing();
            float winChance = std::min(gs.stats.reputation / 100.f * 0.75f + 0.25f, 0.95f);
            ImGui::Text("Your current win chance: %.0f%%  (Reputation: %.0f)",
                winChance * 100.f, gs.stats.reputation);
            ImGui::Separator();
            bool any = false;
            for (auto& cl : gs.clients) {
                if (cl.active || !cl.available) continue;
                any = true;
                ImGui::PushID(cl.id);
                ImGui::Text("%-24s  %-14s  Budget: $%.0f/mo",
                    cl.name.c_str(), IndustryNames[(int)cl.industry], cl.budget);
                ImGui::SameLine(520);
                if (ImGui::Button("  Pitch!  ")) {
                    if ((float)(rand() % 100) / 100.f < winChance) {
                        cl.active    = true;
                        cl.available = false;
                        cl.contractMonths = 6;
                        gs.stats.clientsAcquired++;
                        gs.playerMarketShare = std::min(gs.playerMarketShare + 1.5f, 100.f);
                        Newsfeed::PushNews("Won client: " + cl.name, 0.3f, 1.f, 0.4f);
                    } else {
                        Newsfeed::PushNews("Failed pitch to: " + cl.name, 1.f, 0.5f, 0.2f);
                    }
                }
                ImGui::Separator();
                ImGui::PopID();
            }
            if (!any) ImGui::TextDisabled("No clients available to pitch right now.");
            ImGui::EndTabItem();
        }
        // Lost Clients
        if (ImGui::BeginTabItem("Lost Clients")) {
            for (auto& cl : gs.clients) {
                if (cl.active || cl.available) continue;
                ImGui::TextColored(ImVec4(0.7f,0.4f,0.4f,1.f),
                    "%-24s  %-14s  $%.0f/mo",
                    cl.name.c_str(), IndustryNames[(int)cl.industry], cl.budget);
            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::End();
}
