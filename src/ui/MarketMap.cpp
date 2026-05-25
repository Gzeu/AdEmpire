#include "MarketMap.h"
#include "imgui.h"
#include <cstdio>
#include <algorithm>

void MarketMap::Render(GameState& gs) {
    if (!gs.showMarketMap) return;
    ImGui::SetNextWindowPos(ImVec2(325, 40), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(950, 640), ImGuiCond_Always);
    ImGui::Begin("Market Map", &gs.showMarketMap,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    ImGui::TextColored(ImVec4(0.3f,0.8f,1.f,1.f), "Global Market Share");
    ImGui::Separator();

    auto drawBar = [&](const char* name, float share, ImVec4 color) {
        char label[48];
        snprintf(label, 48, "%.1f%%", share);
        ImGui::TextColored(color, "%-22s", name);
        ImGui::SameLine(200);
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, color);
        ImGui::ProgressBar(share / 100.f, ImVec2(580, 24), label);
        ImGui::PopStyleColor();
    };

    drawBar(gs.agencyName.c_str(), gs.playerMarketShare, ImVec4(0.3f,0.8f,1.f,1.f));
    for (auto& ai : gs.competitors) {
        ImVec4 col = ImVec4(1.f,0.4f,0.4f,1.f);
        if (ai.strategy == "premium") col = ImVec4(1.f,0.75f,0.2f,1.f);
        if (ai.strategy == "niche")   col = ImVec4(0.8f,0.4f,1.f,1.f);
        drawBar(ai.name.c_str(), ai.marketShare, col);
    }
    float other = 100.f - gs.playerMarketShare;
    for (auto& ai : gs.competitors) other -= ai.marketShare;
    other = std::max(other, 0.f);
    if (other > 0.5f) drawBar("Other Agencies", other, ImVec4(0.45f,0.45f,0.45f,1.f));

    ImGui::Separator();
    ImGui::Spacing();

    // Target progress
    ImGui::TextColored(ImVec4(0.4f,1.f,0.6f,1.f), "Victory Target: 35%% Market Share");
    char progLabel[48];
    snprintf(progLabel, 48, "%.1f%% / 35%%", gs.playerMarketShare);
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.3f,0.9f,0.5f,1.f));
    ImGui::ProgressBar(gs.playerMarketShare / 35.f, ImVec2(-1, 28), progLabel);
    ImGui::PopStyleColor();

    ImGui::Separator();
    ImGui::Spacing();
    ImGui::Text("Competitor Intelligence:");
    if (ImGui::BeginTable("##rivals", 5,
        ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit)) {
        ImGui::TableSetupColumn("Agency",     ImGuiTableColumnFlags_WidthFixed, 160);
        ImGui::TableSetupColumn("Strategy",   ImGuiTableColumnFlags_WidthFixed, 90);
        ImGui::TableSetupColumn("Clients",    ImGuiTableColumnFlags_WidthFixed, 70);
        ImGui::TableSetupColumn("Reputation", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Aggression", ImGuiTableColumnFlags_WidthFixed, 100);
        ImGui::TableHeadersRow();
        for (auto& ai : gs.competitors) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("%s", ai.name.c_str());
            ImGui::TableSetColumnIndex(1); ImGui::Text("%s", ai.strategy.c_str());
            ImGui::TableSetColumnIndex(2); ImGui::Text("%d", ai.clientCount);
            ImGui::TableSetColumnIndex(3);
            char rl[16]; snprintf(rl, 16, "%.0f", ai.reputation);
            ImGui::ProgressBar(ai.reputation / 100.f, ImVec2(-1, 14), rl);
            ImGui::TableSetColumnIndex(4);
            ImVec4 aggCol = ai.aggressiveness > 0.7f ? ImVec4(1.f,0.3f,0.3f,1.f)
                          : ai.aggressiveness > 0.4f ? ImVec4(1.f,0.7f,0.2f,1.f)
                          : ImVec4(0.4f,1.f,0.5f,1.f);
            char al[16]; snprintf(al, 16, "%.0f%%", ai.aggressiveness * 100.f);
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, aggCol);
            ImGui::ProgressBar(ai.aggressiveness, ImVec2(-1, 14), al);
            ImGui::PopStyleColor();
        }
        ImGui::EndTable();
    }
    ImGui::End();
}
