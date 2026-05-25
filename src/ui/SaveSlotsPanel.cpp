#include "SaveSlotsPanel.h"
#include "../systems/SaveSystem.h"
#include "../systems/ToastSystem.h"
#include "imgui.h"
#include <cstdio>
#include <string>

static const char* SLOT_FILES[] = { "save_slot1.json", "save_slot2.json", "save_slot3.json" };

void SaveSlotsPanel::Render(GameState& gs) {
    if (!gs.showSaveSlots) return;
    ImGui::SetNextWindowPos(ImVec2(325, 40), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(700, 320), ImGuiCond_Always);
    ImGui::Begin("Save Slots", &gs.showSaveSlots,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    ImGui::TextColored(ImVec4(0.7f,0.9f,1.f,1.f),
        "Choose a slot to save or load your game.");
    ImGui::Separator();

    for (int i = 0; i < 3; i++) {
        ImGui::PushID(i);
        bool occupied = SaveSystem::HasSave(SLOT_FILES[i]);

        ImGui::PushStyleColor(ImGuiCol_ChildBg,
            occupied ? ImVec4(0.06f,0.14f,0.10f,0.95f)
                     : ImVec4(0.06f,0.06f,0.12f,0.80f));
        ImGui::BeginChild("##slot", ImVec2(-1, 64), true);

        // Slot title
        ImGui::TextColored(ImVec4(0.5f,0.8f,1.f,1.f), "Slot %d", i + 1);
        ImGui::SameLine(80);
        if (occupied) {
            ImGui::TextColored(ImVec4(0.3f,1.f,0.4f,1.f), "%s", gs.agencyName.c_str());
            ImGui::SameLine(260);
            ImGui::Text("Mo %d/%d  |  $%.0f  |  %.1f%% share",
                gs.month, gs.year, gs.budget, gs.playerMarketShare);
        } else {
            ImGui::TextDisabled("Empty slot");
        }

        // Action buttons
        ImGui::Spacing();
        if (ImGui::Button("Save", ImVec2(80, 22))) {
            SaveSystem::Save(gs, SLOT_FILES[i]);
            TOAST_SUCCESS("Game saved to Slot " + std::to_string(i + 1));
        }
        ImGui::SameLine();
        if (!occupied) ImGui::BeginDisabled();
        if (ImGui::Button("Load", ImVec2(80, 22))) {
            SaveSystem::Load(gs, SLOT_FILES[i]);
            TOAST_SUCCESS("Slot " + std::to_string(i + 1) + " loaded.");
        }
        ImGui::SameLine();
        if (ImGui::Button("Delete", ImVec2(80, 22))) {
            std::remove(SLOT_FILES[i]);
            TOAST_WARN("Slot " + std::to_string(i + 1) + " deleted.");
        }
        if (!occupied) ImGui::EndDisabled();

        ImGui::EndChild();
        ImGui::PopStyleColor();
        ImGui::Spacing();
        ImGui::PopID();
    }
    ImGui::End();
}
