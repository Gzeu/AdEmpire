#include "SpecializationPanel.h"
#include "imgui.h"
#include <cstdio>

void SpecializationPanel::Render(GameState& gs) {
    if (!gs.showSpecializations) return;
    ImGui::SetNextWindowPos(ImVec2(325, 40), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(700, 560), ImGuiCond_Always);
    ImGui::Begin("Specializations", &gs.showSpecializations,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    ImGui::TextColored(ImVec4(0.8f,0.6f,1.f,1.f),
        "Serve enough clients in an industry to unlock specialization bonuses.");
    ImGui::Separator();
    ImGui::Spacing();

    if (gs.specializations.empty()) {
        ImGui::TextDisabled("No specializations loaded. Start a new game.");
        ImGui::End();
        return;
    }

    // 2-column grid
    ImGui::Columns(2, "##specs", false);
    for (auto& spec : gs.specializations) {
        ImGui::PushID((int)spec.industry);

        bool unlocked = spec.unlocked;
        ImVec4 headerCol = unlocked ? ImVec4(0.3f,1.f,0.4f,1.f) : ImVec4(0.6f,0.6f,0.6f,1.f);
        const char* icon = unlocked ? "\xE2\x9C\x94 " : "\xF0\x9F\x94\x92 ";

        ImGui::TextColored(headerCol, "%s%s",
            icon, IndustryNames[(int)spec.industry]);

        float progress = spec.clientsRequired > 0
            ? (float)spec.clientsServed / spec.clientsRequired : 1.f;
        progress = std::min(progress, 1.f);

        char label[32];
        snprintf(label, 32, "%d / %d clients",
            spec.clientsServed, spec.clientsRequired);

        ImVec4 barCol = unlocked ? ImVec4(0.2f,0.8f,0.3f,1.f) : ImVec4(0.4f,0.4f,0.7f,1.f);
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, barCol);
        ImGui::ProgressBar(progress, ImVec2(-1,16), label);
        ImGui::PopStyleColor();

        if (unlocked)
            ImGui::TextColored(ImVec4(0.4f,1.f,0.6f,1.f),
                "Bonus: +%.0f%% on all campaigns",
                (spec.bonusMultiplier - 1.f) * 100.f);
        else
            ImGui::TextDisabled("Locked — need %d more client(s)",
                spec.clientsRequired - spec.clientsServed);

        ImGui::Spacing();
        ImGui::PopID();
        ImGui::NextColumn();
    }
    ImGui::Columns(1);
    ImGui::End();
}
