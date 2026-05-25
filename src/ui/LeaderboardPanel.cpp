#include "LeaderboardPanel.h"
#include "../systems/Leaderboard.h"
#include "imgui.h"

void LeaderboardPanel::SubmitScore(const GameState& gs, const std::string& difficulty) {
    LeaderboardEntry e;
    e.agencyName   = gs.agencyName;
    e.totalRevenue = gs.stats.totalRevenue;
    e.marketShare  = gs.playerMarketShare;
    e.monthsPlayed = gs.stats.monthsPlayed;
    e.year         = gs.year;
    e.difficulty   = difficulty;
    Leaderboard::Get().Submit(e);
    Leaderboard::Get().Save();
}

void LeaderboardPanel::Render(GameState& gs, bool& show) {
    if (!show) return;
    ImGui::SetNextWindowPos(ImVec2(325, 40), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(700, 400), ImGuiCond_Always);
    ImGui::Begin("Leaderboard — Top 10", &show,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    auto& lb = Leaderboard::Get();
    if (lb.Entries().empty()) {
        ImGui::TextDisabled("No scores yet. Finish a game to submit.");
        ImGui::End(); return;
    }

    const char* medals[] = { "\xf0\x9f\xa5\x87", "\xf0\x9f\xa5\x88", "\xf0\x9f\xa5\x89" }; // UTF-8 gold/silver/bronze
    if (ImGui::BeginTable("##lb", 6,
        ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp)) {
        ImGui::TableSetupColumn("#",          ImGuiTableColumnFlags_WidthFixed, 30.f);
        ImGui::TableSetupColumn("Agency",     ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Revenue",    ImGuiTableColumnFlags_WidthFixed, 100.f);
        ImGui::TableSetupColumn("Share",      ImGuiTableColumnFlags_WidthFixed, 60.f);
        ImGui::TableSetupColumn("Months",     ImGuiTableColumnFlags_WidthFixed, 60.f);
        ImGui::TableSetupColumn("Difficulty", ImGuiTableColumnFlags_WidthFixed, 80.f);
        ImGui::TableHeadersRow();

        int rank = 1;
        for (auto& e : lb.Entries()) {
            ImGui::TableNextRow();
            ImVec4 rowColor = rank == 1 ? ImVec4(1.f,0.85f,0.1f,0.15f)
                            : rank == 2 ? ImVec4(0.7f,0.7f,0.7f,0.10f)
                            : rank == 3 ? ImVec4(0.8f,0.5f,0.2f,0.10f)
                            : ImVec4(0,0,0,0);
            if (rank <= 3) ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0,
                ImGui::ColorConvertFloat4ToU32(rowColor));

            ImGui::TableSetColumnIndex(0);
            if (rank <= 3) ImGui::TextColored(rowColor, "%d", rank);
            else ImGui::Text("%d", rank);

            ImGui::TableSetColumnIndex(1); ImGui::Text("%s", e.agencyName.c_str());
            ImGui::TableSetColumnIndex(2); ImGui::Text("$%.0f", e.totalRevenue);
            ImGui::TableSetColumnIndex(3); ImGui::Text("%.1f%%", e.marketShare);
            ImGui::TableSetColumnIndex(4); ImGui::Text("%d", e.monthsPlayed);
            ImGui::TableSetColumnIndex(5); ImGui::Text("%s", e.difficulty.c_str());
            rank++;
        }
        ImGui::EndTable();
    }
    ImGui::End();
}
