#pragma once
#include "imgui.h"
#include <string>
#include <vector>
#include <chrono>

enum class ToastType { Info, Success, Warning, Error };

struct Toast {
    std::string text;
    ToastType   type;
    float       lifetime; // seconds remaining
    float       maxLife;  // initial lifetime
};

class ToastSystem {
public:
    static ToastSystem& Get();

    void Push(const std::string& text, ToastType type = ToastType::Info, float lifetime = 3.5f);
    void Render(); // call once per frame, draws overlays top-right
    void Update(float dt);

private:
    ToastSystem() = default;
    std::vector<Toast> toasts;

    static ImVec4 ColorForType(ToastType t) {
        switch (t) {
        case ToastType::Success: return ImVec4(0.2f, 0.9f, 0.4f, 1.f);
        case ToastType::Warning: return ImVec4(1.0f, 0.7f, 0.1f, 1.f);
        case ToastType::Error:   return ImVec4(1.0f, 0.3f, 0.3f, 1.f);
        default:                 return ImVec4(0.5f, 0.8f, 1.0f, 1.f);
        }
    }
    static const char* IconForType(ToastType t) {
        switch (t) {
        case ToastType::Success: return "[OK]";
        case ToastType::Warning: return "[!!]";
        case ToastType::Error:   return "[XX]";
        default:                 return "[i] ";
        }
    }
};

inline ToastSystem& ToastSystem::Get() {
    static ToastSystem instance;
    return instance;
}

inline void ToastSystem::Push(const std::string& text, ToastType type, float lifetime) {
    if (toasts.size() >= 6) toasts.erase(toasts.begin());
    toasts.push_back({text, type, lifetime, lifetime});
}

inline void ToastSystem::Update(float dt) {
    for (auto& t : toasts) t.lifetime -= dt;
    toasts.erase(std::remove_if(toasts.begin(), toasts.end(),
        [](const Toast& t) { return t.lifetime <= 0.f; }), toasts.end());
}

inline void ToastSystem::Render() {
    if (toasts.empty()) return;
    ImGuiIO& io = ImGui::GetIO();
    float startY = 50.f;
    for (int i = (int)toasts.size() - 1; i >= 0; i--) {
        auto& t = toasts[i];
        float alpha = std::min(1.f, t.lifetime / 0.4f); // fade last 0.4s
        float progress = 1.f - (t.lifetime / t.maxLife);

        ImGui::SetNextWindowBgAlpha(0.88f * alpha);
        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 320.f, startY), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(310.f, 0.f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.f);
        char wid[32]; snprintf(wid, 32, "##toast%d", i);
        ImGui::Begin(wid, nullptr,
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs |
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoFocusOnAppearing);

        ImGui::TextColored(ColorForType(t.type), "%s %s", IconForType(t.type), t.text.c_str());
        // lifetime progress bar
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ColorForType(t.type));
        ImGui::ProgressBar(1.f - progress, ImVec2(-1.f, 3.f), "");
        ImGui::PopStyleColor();

        startY += ImGui::GetWindowHeight() + 6.f;
        ImGui::End();
        ImGui::PopStyleVar();
    }
}
