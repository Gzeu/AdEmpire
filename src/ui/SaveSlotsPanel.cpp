#include "SaveSlotsPanel.h"
#include "../systems/SaveSlots.h"
#include "../ui/ToastSystem.h"
#include "imgui.h"
#include <cstdio>

int SaveSlotsPanel::activeSlot = 0;

void SaveSlotsPanel::Render(GameState& gs, bool& show) {
    if (!show) return;
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f,0.5f));
    ImGui::SetNextWindowSize(ImVec2(560, 320));
    ImGui::Begin("Save / Load", &show, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    ImGui::Text("Select a save slot:");
    ImGui::Separator();

    auto slots = SaveSlots::GetAll();
    for (auto& s : slots) {
        ImGui::PushID(s.slot);
        ImGui::BeginGroup();
        char label[128];
        if (s.exists)
            snprintf(label, 128, "Slot %d: %s  |  Mo %d/%d  |  $%.0f  |  %.1f%% share",
                s.slot, s.agencyName.c_str(), s.month, s.year, s.budget, s.marketShare);
        else
            snprintf(label, 128, "Slot %d: [Empty]", s.slot);

        bool isActive = (activeSlot == s.slot);
        if (isActive) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f,0.4f,0.1f,1.f));
        if (ImGui::Button(label, ImVec2(440, 34))) {}
        if (isActive) ImGui::PopStyleColor();

        ImGui::SameLine();
        if (ImGui::SmallButton("Save")) {
            SaveSlots::Save(gs, s.slot);
            activeSlot = s.slot;
            ToastSystem::Get().Push("Game saved to slot " + std::to_string(s.slot),
                ToastType::Success);
        }
        ImGui::SameLine();
        if (s.exists && ImGui::SmallButton("Del")) {
            SaveSlots::Delete(s.slot);
            ToastSystem::Get().Push("Slot " + std::to_string(s.slot) + " deleted",
                ToastType::Warning);
        }
        ImGui::EndGroup();
        ImGui::Separator();
        ImGui::PopID();
    }
    ImGui::End();
}
