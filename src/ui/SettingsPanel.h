#pragma once
#include "../core/GameState.h"
#include <string>

struct GameSettings {
    int   resolutionIdx  = 1;   // index into ResolutionOptions
    float masterVolume   = 0.8f;
    float musicVolume    = 0.5f;
    float sfxVolume      = 0.7f;
    bool  fullscreen     = false;
    bool  showFPS        = true;
    bool  autoSave       = true;
    int   autoSaveInterval = 5; // months
    bool  confirmNextMonth = false;
    char  saveSlot[32]   = "save.json";

    static constexpr const char* ResolutionOptions[] = {
        "1280x720", "1366x768", "1600x900", "1920x1080"
    };
    static constexpr int ResolutionW[] = { 1280, 1366, 1600, 1920 };
    static constexpr int ResolutionH[] = {  720,  768,  900, 1080 };
};

class SettingsPanel {
public:
    static void Render(GameState& gs);
    static GameSettings s_settings;
};
