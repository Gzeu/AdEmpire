#pragma once
#include "../core/GameState.h"
#include <string>

class SaveSystem {
public:
    static bool Save(const GameState& gs, const std::string& path = "save.json");
    static bool Load(GameState& gs,       const std::string& path = "save.json");
    static bool HasSave(const std::string& path = "save.json");
};
