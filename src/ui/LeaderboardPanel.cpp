#include "LeaderboardPanel.h"
#include "../systems/Leaderboard.h"
#include "imgui.h"

void LeaderboardPanel::Render(GameState& gs, bool& show) {
    if (!show) return;
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f,0.5f));
    ImGui::SetNextWindowSize(ImVec2(620, 420));
    ImGui::Begin("\xF0\x9F\x8F\x86 Leaderboard", &show,
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    ImGui::TextColored(ImVec4(1.f,0.85f,0.1f,1.f), "Top 10 Agencies (Local)");
    ImGui::Separator();

    auto entries = Leaderboard::Load();
    if (entries.empty()) {
        ImGui::TextDisabled("No entries yet. Win a game to appear here!");
    } else if (ImGui::BeginTable("##lb", 5,
        ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Rank", ImGuiTableColumnFlags_WidthFixed, 45);
        ImGui::TableSetupColumn("Agency");
        ImGui::TableSetupColumn("Revenue");
        ImGui::TableSetupColumn("Share");
        ImGui::TableSetupColumn("Months");
        ImGui::TableHeadersRow();
        for (int i = 0; i < (int)entries.size(); i++) {
            auto& e = entries[i];
            ImGui::TableNextRow();
            ImVec4 col = (i == 0) ? ImVec4(1.f,0.85f,0.1f,1.f) :
                         (i == 1) ? ImVec4(0.8f,0.8f,0.8f,1.f) :
                         (i == 2) ? ImVec4(0.8f,0.5f,0.2f,1.f) :
                                    ImVec4(1.f,1.f,1.f,0.8f);
            ImGui::TableSetColumnIndex(0);
            ImGui::TextColored(col, "#%d", i + 1);
            ImGui::TableSetColumnIndex(1);
            ImGui::TextColored(col, "%s", e.agencyName.c_str());
            ImGui::TableSetColumnIndex(2);
            ImGui::TextColored(col, "$%.0f", e.totalRevenue);
            ImGui::TableSetColumnIndex(3);
            ImGui::TextColored(col, "%.1f%%", e.marketShare);
            ImGui::TableSetColumnIndex(4);
            ImGui::TextColored(col, "%d mo", e.monthsPlayed);
        }
        ImGui::EndTable();
    }
    ImGui::Spacing();
    if (ImGui::Button("Submit Current Score", ImVec2(-1, 36)))
        Leaderboard::Submit(gs.agencyName, gs.stats.totalRevenue,
            gs.playerMarketShare, gs.stats.monthsPlayed, gs.stats.clientsAcquired);
    ImGui::End();
}
