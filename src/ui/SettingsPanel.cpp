#include "SettingsPanel.h"
#include "imgui.h"
#include "../systems/AchievementSystem.h"

Difficulty SettingsPanel::selectedDifficulty = Difficulty::Normal;

void SettingsPanel::Render(GameState& gs) {
    if (!gs.showSettings) return;
    ImGui::SetNextWindowPos(ImVec2(325, 40), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(700, 560), ImGuiCond_Always);
    ImGui::Begin("Settings", &gs.showSettings,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    if (ImGui::BeginTabBar("##settabs")) {
        // ── Game Stats
        if (ImGui::BeginTabItem("Agency Stats")) {
            ImGui::Text("Agency: %s", gs.agencyName.c_str());
            ImGui::Text("Month: %d / Year: %d", gs.month, gs.year);
            ImGui::Text("Total Revenue: $%.0f", gs.stats.totalRevenue);
            ImGui::Text("Best Month:    $%.0f", gs.stats.bestMonthRevenue);
            ImGui::Text("Campaigns Done: %d", gs.stats.campaignsCompleted);
            ImGui::Text("Clients Won:    %d", gs.stats.clientsAcquired);
            ImGui::Text("Clients Lost:   %d", gs.stats.clientsLost);
            ImGui::Text("Market Share:   %.1f%%", gs.playerMarketShare);
            ImGui::Separator();
            // Competitor summary
            ImGui::Text("Competitors:");
            for (auto& ai : gs.competitors)
                ImGui::BulletText("%s — %.1f%% share, %d clients",
                    ai.name.c_str(), ai.marketShare, ai.clientCount);
            ImGui::EndTabItem();
        }
        // ── Achievements
        if (ImGui::BeginTabItem("Achievements")) {
            auto& sys = AchievementSystem::Get();
            int u = sys.CountUnlocked(), t = (int)sys.achievements.size();
            char p[32]; snprintf(p, 32, "%d/%d", u, t);
            ImGui::ProgressBar((float)u/t, ImVec2(-1,18), p);
            ImGui::Spacing();
            for (auto& a : sys.achievements) {
                ImGui::PushID(a.id);
                if (a.unlocked)
                    ImGui::TextColored(ImVec4(0.3f,1.f,0.4f,1.f),
                        "[X] %s — %s", a.title.c_str(), a.description.c_str());
                else
                    ImGui::TextColored(ImVec4(0.4f,0.4f,0.4f,1.f),
                        "[ ] %s — %s", a.title.c_str(), a.description.c_str());
                ImGui::PopID();
            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::End();
}
