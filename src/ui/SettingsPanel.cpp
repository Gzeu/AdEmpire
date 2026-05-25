#include "SettingsPanel.h"
#include "../systems/SaveSystem.h"
#include "imgui.h"
#include <cstdio>

GameSettings SettingsPanel::s_settings;

constexpr const char* GameSettings::ResolutionOptions[];
constexpr int GameSettings::ResolutionW[];
constexpr int GameSettings::ResolutionH[];

void SettingsPanel::Render(GameState& gs) {
    if (!gs.showSettings) return;
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f,0.5f));
    ImGui::SetNextWindowSize(ImVec2(480, 500));
    ImGui::Begin("Settings", &gs.showSettings,
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    auto& S = s_settings;

    // ── Display ────────────────────────────────────────────────────────────
    if (ImGui::CollapsingHeader("Display", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::SetNextItemWidth(200);
        ImGui::Combo("Resolution", &S.resolutionIdx,
            GameSettings::ResolutionOptions, 4);
        ImGui::Checkbox("Fullscreen (restart required)", &S.fullscreen);
        ImGui::Checkbox("Show FPS Counter", &S.showFPS);
    }

    // ── Audio ──────────────────────────────────────────────────────────────
    if (ImGui::CollapsingHeader("Audio", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::SliderFloat("Master Volume", &S.masterVolume, 0.f, 1.f, "%.0f%%",
            ImGuiSliderFlags_None);
        ImGui::SliderFloat("Music Volume",  &S.musicVolume,  0.f, 1.f);
        ImGui::SliderFloat("SFX Volume",    &S.sfxVolume,    0.f, 1.f);
    }

    // ── Gameplay ───────────────────────────────────────────────────────────
    if (ImGui::CollapsingHeader("Gameplay", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Checkbox("Auto-Save", &S.autoSave);
        if (S.autoSave) {
            ImGui::SameLine();
            ImGui::SetNextItemWidth(100);
            ImGui::SliderInt("every N months", &S.autoSaveInterval, 1, 12);
        }
        ImGui::Checkbox("Confirm before Next Month", &S.confirmNextMonth);
    }

    // ── Save / Load ────────────────────────────────────────────────────────
    if (ImGui::CollapsingHeader("Save & Load", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::SetNextItemWidth(200);
        ImGui::InputText("Save File", S.saveSlot, 32);
        ImGui::Spacing();
        if (ImGui::Button("Save Now", ImVec2(140, 32)))
            SaveSystem::Save(gs, S.saveSlot);
        ImGui::SameLine();
        if (ImGui::Button("Load Save", ImVec2(140, 32)))
            SaveSystem::Load(gs, S.saveSlot);
        if (!SaveSystem::HasSave(S.saveSlot))
            ImGui::TextDisabled("No save file found at '%s'", S.saveSlot);
        else
            ImGui::TextColored(ImVec4(0.4f,1.f,0.5f,1.f),
                "Save exists: %s", S.saveSlot);
    }

    ImGui::Separator();
    if (ImGui::Button("Close", ImVec2(-1, 32)))
        gs.showSettings = false;
    ImGui::End();
}
