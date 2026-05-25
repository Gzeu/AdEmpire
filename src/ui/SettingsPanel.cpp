#include "SettingsPanel.h"
#include "../systems/SaveSystem.h"
#include "imgui.h"
#include <cstdio>
#include <cstring>
#include <filesystem>

SettingsData SettingsPanel::s_data;

void SettingsPanel::Render(GameState& gs) {
    if (!gs.showSettings) return;
    ImGui::SetNextWindowPos(ImVec2(325, 40), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(700, 560), ImGuiCond_Always);
    ImGui::Begin("Settings", &gs.showSettings,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    if (ImGui::BeginTabBar("##settabs")) {

        // ── DISPLAY
        if (ImGui::BeginTabItem("Display")) {
            static const char* resolutions[] = {
                "1024 x 576", "1280 x 720", "1600 x 900", "1920 x 1080"
            };
            ImGui::Text("Resolution:");
            ImGui::SetNextItemWidth(200);
            ImGui::Combo("##res", &s_data.resolutionIdx, resolutions, 4);
            ImGui::Checkbox("Fullscreen", &s_data.fullscreen);
            ImGui::Checkbox("Show FPS Counter", &s_data.showFPS);
            if (s_data.showFPS) {
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(0.4f,1.f,0.4f,1.f),
                    "  FPS: %.1f", ImGui::GetIO().Framerate);
            }
            ImGui::EndTabItem();
        }

        // ── AUDIO
        if (ImGui::BeginTabItem("Audio")) {
            ImGui::Text("Master Volume");
            ImGui::SliderFloat("##master", &s_data.masterVolume, 0.f, 1.f, "%.0f%%");
            ImGui::Text("Music Volume");
            ImGui::SliderFloat("##music",  &s_data.musicVolume,  0.f, 1.f, "%.0f%%");
            ImGui::Text("SFX Volume");
            ImGui::SliderFloat("##sfx",    &s_data.sfxVolume,    0.f, 1.f, "%.0f%%");
            ImGui::EndTabItem();
        }

        // ── GAMEPLAY
        if (ImGui::BeginTabItem("Gameplay")) {
            ImGui::Checkbox("Auto-Save", &s_data.autoSave);
            if (s_data.autoSave) {
                ImGui::SameLine();
                ImGui::Text("every");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(60);
                ImGui::InputInt("##asi", &s_data.autoSaveMonths);
                if (s_data.autoSaveMonths < 1) s_data.autoSaveMonths = 1;
                ImGui::SameLine();
                ImGui::Text("months");
            }
            ImGui::Checkbox("Confirm before advancing month", &s_data.confirmNextMonth);
            ImGui::EndTabItem();
        }

        // ── SAVE & LOAD
        if (ImGui::BeginTabItem("Save & Load")) {
            ImGui::Text("Save file name:");
            ImGui::SetNextItemWidth(300);
            ImGui::InputText("##sfn", s_data.saveFileName, 64);
            ImGui::SameLine();
            ImGui::TextDisabled(".json");
            ImGui::Spacing();

            char fullPath[128];
            snprintf(fullPath, 128, "%s.json", s_data.saveFileName);

            bool fileExists = std::filesystem::exists(fullPath);
            if (fileExists)
                ImGui::TextColored(ImVec4(0.4f,1.f,0.4f,1.f), "File found: %s", fullPath);
            else
                ImGui::TextColored(ImVec4(0.6f,0.6f,0.6f,1.f), "No file: %s", fullPath);

            ImGui::Spacing();
            if (ImGui::Button("Save Game", ImVec2(150, 36))) {
                bool ok = SaveSystem::Save(gs, fullPath);
                s_data.lastSaveStatus = ok ? "Saved successfully!" : "Save failed!";
            }
            ImGui::SameLine();
            if (!fileExists) ImGui::BeginDisabled();
            if (ImGui::Button("Load Game", ImVec2(150, 36))) {
                bool ok = SaveSystem::Load(gs, fullPath);
                s_data.lastSaveStatus = ok ? "Loaded successfully!" : "Load failed!";
            }
            if (!fileExists) ImGui::EndDisabled();

            if (!s_data.lastSaveStatus.empty())
                ImGui::TextColored(ImVec4(0.4f,1.f,0.4f,1.f),
                    "%s", s_data.lastSaveStatus.c_str());
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::End();
}
