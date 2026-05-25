#pragma once
#include "../core/GameState.h"
#include <vector>

class EventSystem {
public:
    static void Init(GameState& gs);
    static void TryTriggerEvent(GameState& gs);
    static std::vector<NewsEvent> GetAllEvents();
};
