#pragma once
#include <string>
#include <vector>
#include "imgui.h"

enum class ToastType { Success, Error, Info, Warning };

struct Toast {
    std::string text;
    ToastType   type;
    float       lifetime;   // seconds total
    float       remaining;  // seconds left
    float       alpha;      // fade value 0-1
};

class ToastSystem {
public:
    static ToastSystem& Get();

    void Add(const std::string& text, ToastType type = ToastType::Info, float lifetime = 3.5f);
    void Update(float deltaTime);
    void Render();  // call after ImGui::NewFrame, before ImGui::Render

private:
    std::vector<Toast> m_toasts;
    static constexpr int MAX_TOASTS = 6;

    ImVec4 GetColor(ToastType t) const;
    const char* GetIcon(ToastType t) const;
};

// Convenience macros
#define TOAST_SUCCESS(msg) ToastSystem::Get().Add(msg, ToastType::Success)
#define TOAST_ERROR(msg)   ToastSystem::Get().Add(msg, ToastType::Error)
#define TOAST_INFO(msg)    ToastSystem::Get().Add(msg, ToastType::Info)
#define TOAST_WARN(msg)    ToastSystem::Get().Add(msg, ToastType::Warning)
