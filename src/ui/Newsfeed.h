#pragma once
#include "../core/GameState.h"
#include <string>
#include <vector>

struct NewsEntry {
    std::string text;
    float r, g, b;
};

class Newsfeed {
public:
    static void Render(GameState& gs);
    static void PushNews(const std::string& text, float r = 1.f, float g = 1.f, float b = 1.f);
    static std::vector<NewsEntry> s_entries;
};
