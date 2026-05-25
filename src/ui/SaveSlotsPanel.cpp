#include "SaveSlotsPanel.h"
#include "../systems/SaveSlots.h"
#include "imgui.h"
#include <cstdio>

void SaveSlotsPanel::Render(GameState& gs) {
    if (!gs.showSaveSlots) return;
    ImGui::SetNextWindowPos(ImVec2(325, 40), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(700, 400), ImGuiCond_Always);
    ImGui::Begin("Save / Load", &gs.showSaveSlots,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    ImGui::Text("Choose a save slot:");
    ImGui::Separator();
    ImGui::Spacing();

    for (int i = 0; i < SaveSlots::SLOT_COUNT; i++) {
        ImGui::PushID(i);
        auto& sl = SaveSlots::s_slots[i];

        // Slot header
        char slotLabel[32];
        snprintf(slotLabel, 32, "Slot %d", i + 1);

        ImGui::PushStyleColor(ImGuiCol_ChildBg,
            sl.occupied ? ImVec4(0.08f,0.12f,0.20f,1.f) : ImVec4(0.05f,0.05f,0.08f,1.f));
        ImGui::BeginChild(slotLabel, ImVec2(-1, 90), true);

        if (sl.occupied) {
            ImGui::TextColored(ImVec4(0.3f,0.8f,1.f,1.f),
                "🏢  %s", sl.agencyName.c_str());
            ImGui::SameLine(260);
            ImGui::TextColored(ImVec4(0.5f,0.5f,0.5f,1.f), "Saved: %s", sl.savedAt.c_str());
            ImGui::Text("  Budget: $%.0f  |  Share: %.1f%%  |  Month %d/%d  |  %d clients",
                sl.budget, sl.marketShare, sl.month, sl.year, sl.clients);
            // Progress bar
            ImGui::ProgressBar(sl.marketShare / 100.f, ImVec2(200, 10), "");
        } else {
            ImGui::TextDisabled("  [ Empty Slot ]");
        }

        ImGui::Spacing();
        // Buttons
        if (ImGui::Button("Save Here", ImVec2(120, 26)))
            SaveSlots::Save(i, gs);
        ImGui::SameLine();
        if (!sl.occupied) ImGui::BeginDisabled();
        if (ImGui::Button("Load", ImVec2(80, 26)))
            SaveSlots::Load(i, gs);
        ImGui::SameLine();
        if (ImGui::Button("Delete", ImVec2(80, 26)))
            SaveSlots::Delete(i);
        if (!sl.occupied) ImGui::EndDisabled();

        ImGui::EndChild();
        ImGui::PopStyleColor();
        ImGui::Spacing();
        ImGui::PopID();
    }
    ImGui::End();
}
