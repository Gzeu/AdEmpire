#pragma once
#include "../core/GameState.h"

class EventSystem {
public:
    static void Init(GameState& gs);
    static void TryTriggerEvent(GameState& gs); // call each month
    static std::vector<NewsEvent> GetAllEvents();
};
