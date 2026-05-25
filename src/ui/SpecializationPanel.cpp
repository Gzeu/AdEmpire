#include "SpecializationPanel.h"
#include "imgui.h"

void SpecializationPanel::Render(GameState& gs) {
    if (!gs.showSpecializations) return;
    ImGui::SetNextWindowPos(ImVec2(325, 40), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(700, 560), ImGuiCond_Always);
    ImGui::Begin("Specializations", &gs.showSpecializations,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    ImGui::TextColored(ImVec4(0.3f,0.8f,1.f,1.f),
        "Earn industry specializations by working with clients.");
    ImGui::TextDisabled("Each unlocks a campaign performance bonus for that industry.");
    ImGui::Separator();
    ImGui::Spacing();

    for (auto& sp : gs.specializations) {
        ImGui::PushID(&sp);
        ImVec4 headerCol = sp.unlocked
            ? ImVec4(0.3f,1.f,0.5f,1.f)
            : ImVec4(0.6f,0.6f,0.6f,1.f);
        ImGui::TextColored(headerCol, "%s  %s",
            sp.unlocked ? "[UNLOCKED]" : "[LOCKED]  ",
            sp.name.c_str());
        ImGui::SameLine(380);
        ImGui::TextColored(ImVec4(1.f,0.8f,0.3f,1.f),
            "+%.0f%% campaign bonus", (sp.bonusMultiplier - 1.f) * 100.f);

        ImGui::TextDisabled("  %s", sp.description.c_str());
        auto it = gs.stats.industryClientCount.find((int)sp.industry);
        int have = (it != gs.stats.industryClientCount.end()) ? it->second : 0;
        float pct = std::min((float)have / sp.clientsRequired, 1.f);
        char lbl[48];
        snprintf(lbl, 48, "%d / %d clients", have, sp.clientsRequired);
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram,
            sp.unlocked ? ImVec4(0.3f,1.f,0.4f,1.f) : ImVec4(0.4f,0.5f,0.8f,1.f));
        ImGui::ProgressBar(pct, ImVec2(-1, 14), lbl);
        ImGui::PopStyleColor();
        ImGui::Separator();
        ImGui::PopID();
    }
    ImGui::End();
}
