#pragma once
#include "imgui.h"
#include <string>

namespace FontManager {
    inline ImFont* Regular = nullptr;
    inline ImFont* Large   = nullptr;
    inline ImFont* Small   = nullptr;

    inline void Load(const std::string& basePath = "assets/fonts/") {
        ImGuiIO& io = ImGui::GetIO();
        // Try loading custom font, fallback to default
        std::string path = basePath + "Roboto-Regular.ttf";
        ImFontConfig cfg;
        cfg.OversampleH = 2; cfg.OversampleV = 2;

        FILE* f = fopen(path.c_str(), "rb");
        if (f) {
            fclose(f);
            Regular = io.Fonts->AddFontFromFileTTF(path.c_str(), 16.f, &cfg);
            Large   = io.Fonts->AddFontFromFileTTF(path.c_str(), 22.f, &cfg);
            Small   = io.Fonts->AddFontFromFileTTF(path.c_str(), 13.f, &cfg);
        }
        // Fallback
        if (!Regular) Regular = io.Fonts->AddFontDefault();
        if (!Large)   Large   = io.Fonts->AddFontDefault();
        if (!Small)   Small   = io.Fonts->AddFontDefault();

        io.Fonts->Build();
    }

    inline void PushLarge()  { ImGui::PushFont(Large);   }
    inline void PushSmall()  { ImGui::PushFont(Small);   }
    inline void PushRegular(){ ImGui::PushFont(Regular); }
    inline void Pop()        { ImGui::PopFont(); }
}
