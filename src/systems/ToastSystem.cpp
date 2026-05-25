#include "ToastSystem.h"
#include <algorithm>
#include <cmath>

ToastSystem& ToastSystem::Get() {
    static ToastSystem instance;
    return instance;
}

ImVec4 ToastSystem::GetColor(ToastType t) const {
    switch (t) {
        case ToastType::Success: return ImVec4(0.20f, 0.85f, 0.40f, 1.f);
        case ToastType::Error:   return ImVec4(0.95f, 0.25f, 0.25f, 1.f);
        case ToastType::Warning: return ImVec4(0.95f, 0.75f, 0.10f, 1.f);
        default:                 return ImVec4(0.35f, 0.70f, 1.00f, 1.f);
    }
}

const char* ToastSystem::GetIcon(ToastType t) const {
    switch (t) {
        case ToastType::Success: return "[OK]";
        case ToastType::Error:   return "[!!]";
        case ToastType::Warning: return "[!] ";
        default:                 return "[i] ";
    }
}

void ToastSystem::Add(const std::string& text, ToastType type, float lifetime) {
    if ((int)m_toasts.size() >= MAX_TOASTS)
        m_toasts.erase(m_toasts.begin());
    m_toasts.push_back({text, type, lifetime, lifetime, 1.f});
}

void ToastSystem::Update(float deltaTime) {
    for (auto& t : m_toasts) {
        t.remaining -= deltaTime;
        // fade out in last 0.6 seconds
        if (t.remaining < 0.6f)
            t.alpha = std::max(0.f, t.remaining / 0.6f);
    }
    m_toasts.erase(
        std::remove_if(m_toasts.begin(), m_toasts.end(),
            [](const Toast& t) { return t.remaining <= 0.f; }),
        m_toasts.end());
}

void ToastSystem::Render() {
    if (m_toasts.empty()) return;

    ImGuiIO& io = ImGui::GetIO();
    const float padding  = 12.f;
    const float toastH   = 44.f;
    const float toastW   = 360.f;
    float startY = io.DisplaySize.y - padding;

    for (int i = (int)m_toasts.size() - 1; i >= 0; i--) {
        auto& toast = m_toasts[i];
        startY -= toastH + 6.f;

        ImVec2 pos(io.DisplaySize.x - toastW - padding, startY);
        ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(toastW, toastH));
        ImGui::SetNextWindowBgAlpha(0.88f * toast.alpha);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 7.f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.5f);
        ImVec4 col = GetColor(toast.type);
        col.w *= toast.alpha;
        ImGui::PushStyleColor(ImGuiCol_Border, col);

        char wid[32]; snprintf(wid, 32, "##toast%d", i);
        ImGui::Begin(wid, nullptr,
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs |
            ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoSavedSettings);

        ImGui::SetCursorPosY(12.f);
        ImGui::TextColored(col, "%s", GetIcon(toast.type));
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.95f,0.95f,0.95f, toast.alpha));
        ImGui::TextUnformatted(toast.text.c_str());
        ImGui::PopStyleColor();

        ImGui::End();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);
    }
}
