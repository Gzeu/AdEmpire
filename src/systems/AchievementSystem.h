#pragma once
#include "../core/GameState.h"
#include <string>
#include <vector>
#include <functional>

struct Achievement {
    std::string id;
    std::string title;
    std::string description;
    std::string icon;          // emoji
    bool        unlocked = false;
    int         monthUnlocked = -1;
    std::function<bool(const GameState&)> condition;
};

class AchievementSystem {
public:
    static void Init();
    static void Check(GameState& gs);    // call each month
    static std::vector<Achievement>& All();
    static int  UnlockedCount();
};
