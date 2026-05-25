#pragma once
#include <string>
#include <vector>
#include <functional>
#include "../core/GameState.h"

struct Achievement {
    int         id;
    std::string title;
    std::string description;
    std::string icon;      // emoji shorthand
    bool        unlocked;
    int         unlockedMonth;
    std::function<bool(const GameState&)> condition;
};

class AchievementSystem {
public:
    static AchievementSystem& Get() { static AchievementSystem a; return a; }
    void Init();
    void Tick(const GameState& gs);        // call each month
    void Render(GameState& gs);            // UI panel
    int  CountUnlocked() const;
    std::vector<Achievement> achievements;
    std::vector<std::string> recentlyUnlocked; // for toast integration
};
