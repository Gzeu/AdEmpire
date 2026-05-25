#pragma once
#include "../core/GameState.h"

struct SettingsData {
    int   resolutionIdx   = 2;   // 0=1024x576, 1=1280x720, 2=1600x900, 3=1920x1080
    bool  fullscreen      = false;
    bool  showFPS         = true;
    float masterVolume    = 0.8f;
    float musicVolume     = 0.6f;
    float sfxVolume       = 0.7f;
    bool  autoSave        = true;
    int   autoSaveMonths  = 3;
    bool  confirmNextMonth = false;
    char  saveFileName[64] = "save";
    std::string lastSaveStatus = "";
};

class SettingsPanel {
public:
    static void Render(GameState& gs);
    static SettingsData s_data;
};
