#pragma once
#include "../core/GameState.h"
#include <string>
#include <vector>
#include <functional>

struct AchievementPopup {
    std::string title;
    float       timer;  // seconds remaining
};

class AchievementsSystem {
public:
    static void Init();
    static void CheckAll(const GameState& gs);
    static void Render(GameState& gs);
    static void RenderPopups();
    static bool HasPopup();

    static std::vector<Achievement>      s_achievements;
    static std::vector<AchievementPopup> s_popups;
};
