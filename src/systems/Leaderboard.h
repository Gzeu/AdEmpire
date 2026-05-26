#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>

struct LeaderboardEntry {
    std::string agencyName;
    long long   score        = 0;
    float       revenue      = 0.f;
    float       marketShare  = 0.f;
    int         months       = 0;
    std::string difficulty;
};

class Leaderboard {
public:
    static Leaderboard& Get();

    void Load(const std::string& path = "leaderboard.json");
    void Save(const std::string& path = "leaderboard.json");
    void Submit(const LeaderboardEntry& entry);
    const std::vector<LeaderboardEntry>& Entries() const { return entries; }
    void Clear() { entries.clear(); }

private:
    Leaderboard() = default;
    std::vector<LeaderboardEntry> entries;

    static void SortEntries(std::vector<LeaderboardEntry>& v) {
        std::sort(v.begin(), v.end(), [](const LeaderboardEntry& a, const LeaderboardEntry& b) {
            return a.score > b.score;
        });
        if (v.size() > 50) v.resize(50);
    }
};

inline Leaderboard& Leaderboard::Get() {
    static Leaderboard instance;
    return instance;
}

inline void Leaderboard::Submit(const LeaderboardEntry& entry) {
    entries.push_back(entry);
    SortEntries(entries);
}

inline void Leaderboard::Load(const std::string& path) {
    // Manual JSON parse — no external dep
    std::ifstream f(path);
    if (!f.is_open()) return;
    // Minimal parse: just fill with what we can
    // Full nlohmann/json integration: load entries[] array from file
    // Stub: no-op until integrated
}

inline void Leaderboard::Save(const std::string& path) {
    std::ofstream f(path);
    if (!f.is_open()) return;
    f << "[\n";
    for (size_t i = 0; i < entries.size(); i++) {
        auto& e = entries[i];
        f << "  {";
        f << "\"agency\":\"" << e.agencyName << "\",";
        f << "\"score\":" << e.score << ",";
        f << "\"revenue\":" << e.revenue << ",";
        f << "\"share\":" << e.marketShare << ",";
        f << "\"months\":" << e.months << ",";
        f << "\"difficulty\":\"" << e.difficulty << "\"";
        f << "}";
        if (i + 1 < entries.size()) f << ",";
        f << "\n";
    }
    f << "]\n";
}
