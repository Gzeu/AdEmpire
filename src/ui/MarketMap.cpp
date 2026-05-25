#include "MarketMap.h"
#include "imgui.h"

void MarketMap::Render(GameState& gs) {
    if (!gs.showMarketMap) return;
    ImGui::SetNextWindowPos(ImVec2(325, 40), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(700, 560), ImGuiCond_Always);
    ImGui::Begin("Market Map", &gs.showMarketMap,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    ImGui::Text("Market Share Overview");
    ImGui::Separator();

    auto drawBar = [&](const char* name, float share, ImVec4 color) {
        ImGui::TextColored(color, "%-20s", name);
        ImGui::SameLine();
        char label[32];
        snprintf(label, 32, "%.1f%%", share);
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, color);
        ImGui::ProgressBar(share / 100.f, ImVec2(400, 22), label);
        ImGui::PopStyleColor();
    };

    drawBar(gs.agencyName.c_str(), gs.playerMarketShare,
        ImVec4(0.3f, 0.8f, 1.f, 1.f));

    for (auto& ai : gs.competitors) {
        ImVec4 col = ImVec4(1.f, 0.4f, 0.4f, 1.f);
        if (ai.strategy == "premium") col = ImVec4(1.f, 0.7f, 0.2f, 1.f);
        if (ai.strategy == "niche")   col = ImVec4(0.8f, 0.4f, 1.f, 1.f);
        drawBar(ai.name.c_str(), ai.marketShare, col);
    }

    float other = 100.f - gs.playerMarketShare;
    for (auto& ai : gs.competitors) other -= ai.marketShare;
    if (other > 0) drawBar("Others", other, ImVec4(0.5f,0.5f,0.5f,1.f));

    ImGui::Separator();
    ImGui::Spacing();
    ImGui::Text("Competitors:");
    if (ImGui::BeginTable("##rivals", 4,
        ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Agency");
        ImGui::TableSetupColumn("Strategy");
        ImGui::TableSetupColumn("Clients");
        ImGui::TableSetupColumn("Reputation");
        ImGui::TableHeadersRow();
        for (auto& ai : gs.competitors) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("%s", ai.name.c_str());
            ImGui::TableSetColumnIndex(1); ImGui::Text("%s", ai.strategy.c_str());
            ImGui::TableSetColumnIndex(2); ImGui::Text("%d", ai.clientCount);
            ImGui::TableSetColumnIndex(3);
            char rl[16]; snprintf(rl, 16, "%.0f", ai.reputation);
            ImGui::ProgressBar(ai.reputation / 100.f, ImVec2(-1, 14), rl);
        }
        ImGui::EndTable();
    }
    ImGui::End();
}
