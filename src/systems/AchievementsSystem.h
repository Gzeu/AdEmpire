#pragma once
#include "../core/GameState.h"
#include <string>
#include <vector>

struct Achievement {
    std::string id;
    std::string title;
    std::string description;
    std::string icon;       // emoji label
    bool        unlocked = false;
    int         monthUnlocked = 0;
};

class AchievementsSystem {
public:
    static void Init();
    static void Check(const GameState& gs);
    static const std::vector<Achievement>& GetAll();
    static int  GetUnlockedCount();
    // For UI overlay
    static bool        s_showPopup;
    static std::string s_popupText;
    static float       s_popupTimer; // seconds
};
