#pragma once
#include "imgui.h"
#include <string>
#include <vector>
#include <chrono>

enum class ToastType { Info, Success, Warning, Error };

struct Toast {
    std::string text;
    ToastType   type;
    float       lifetime;   // seconds remaining
    float       maxLife;
};

class ToastSystem {
public:
    static ToastSystem& Get() { static ToastSystem t; return t; }

    void Push(const std::string& msg, ToastType type = ToastType::Info, float life = 3.5f) {
        toasts.push_back({msg, type, life, life});
        if (toasts.size() > 6) toasts.erase(toasts.begin());
    }

    void Tick(float dt) {
        for (auto& t : toasts) t.lifetime -= dt;
        toasts.erase(std::remove_if(toasts.begin(), toasts.end(),
            [](const Toast& t){ return t.lifetime <= 0; }), toasts.end());
    }

    void Render(float screenW, float screenH) {
        float yOffset = screenH - 60.f;
        for (int i = (int)toasts.size() - 1; i >= 0; i--) {
            auto& t = toasts[i];
            float alpha = std::min(t.lifetime / 0.5f, 1.f); // fade out last 0.5s
            ImVec4 col;
            switch (t.type) {
                case ToastType::Success: col = ImVec4(0.2f,0.85f,0.4f,alpha); break;
                case ToastType::Warning: col = ImVec4(1.f,0.75f,0.1f,alpha); break;
                case ToastType::Error:   col = ImVec4(1.f,0.3f,0.3f,alpha);  break;
                default:                 col = ImVec4(0.5f,0.8f,1.f,alpha);  break;
            }
            ImGui::SetNextWindowBgAlpha(0.88f * alpha);
            ImGui::SetNextWindowPos(ImVec2(screenW - 380.f, yOffset), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(370.f, 42.f));
            char wid[32]; snprintf(wid, 32, "##toast%d", i);
            ImGui::Begin(wid, nullptr,
                ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs |
                ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);
            ImGui::TextColored(col, "%s", t.text.c_str());
            // Progress bar
            ImGui::ProgressBar(t.lifetime / t.maxLife, ImVec2(-1, 4), "");
            ImGui::End();
            yOffset -= 52.f;
        }
    }

    std::vector<Toast> toasts;
};
