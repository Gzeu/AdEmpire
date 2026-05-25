#pragma once
#include "../core/GameState.h"
#include <string>
#include <vector>
#include <functional>

struct Achievement {
    int         id;
    std::string title;
    std::string description;
    std::string category;  // Revenue, Clients, Campaigns, Staff, Market, Survival, Reputation
    bool        unlocked;
    std::function<bool(const GameState&)> condition;
};

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
