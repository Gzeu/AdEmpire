#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <nlohmann/json.hpp>

// ─── LeaderboardPersistence ───────────────────────────────────────────────────
// Saves/loads leaderboard entries to/from leaderboard.json next to the binary.
// Thread-safe: no async writes — called only from UI button click.
// ─────────────────────────────────────────────────────────────────────────────

struct LeaderboardEntry {
    std::string agencyName;
    int         score         = 0;
    bool        won           = false;
    float       peakShare     = 0.0f;  // 0.0 - 1.0
    float       totalRevenue  = 0.0f;
    std::string date;          // ISO date string YYYY-MM-DD
};

class LeaderboardPersistence {
public:
    static constexpr const char* FILE_PATH = "leaderboard.json";
    static constexpr int         MAX_ENTRIES = 50;

    // Add a new entry and persist to disk (top 50 kept, sorted by score desc)
    static void AddEntry(const std::string& name, int score, bool won,
                         float peakShare, float totalRevenue) {
        auto entries = Load();

        LeaderboardEntry e;
        e.agencyName    = name;
        e.score         = score;
        e.won           = won;
        e.peakShare     = peakShare;
        e.totalRevenue  = totalRevenue;
        e.date          = CurrentDate();
        entries.push_back(e);

        // Sort descending by score
        std::sort(entries.begin(), entries.end(),
            [](const LeaderboardEntry& a, const LeaderboardEntry& b){
                return a.score > b.score;
            });

        // Keep top N
        if ((int)entries.size() > MAX_ENTRIES)
            entries.resize(MAX_ENTRIES);

        Save(entries);
    }

    // Load all entries from disk; returns empty vector on missing file
    static std::vector<LeaderboardEntry> Load() {
        std::vector<LeaderboardEntry> out;
        std::ifstream f(FILE_PATH);
        if (!f.is_open()) return out;

        try {
            nlohmann::json j;
            f >> j;
            for (auto& item : j) {
                LeaderboardEntry e;
                e.agencyName   = item.value("name",    "Unknown");
                e.score        = item.value("score",   0);
                e.won          = item.value("won",     false);
                e.peakShare    = item.value("peak",    0.0f);
                e.totalRevenue = item.value("revenue", 0.0f);
                e.date         = item.value("date",    "");
                out.push_back(e);
            }
        } catch (...) { /* corrupt file — ignore */ }
        return out;
    }

    // Returns true if leaderboard file exists on disk
    static bool Exists() {
        std::ifstream f(FILE_PATH);
        return f.is_open();
    }

    // Clear all saved entries
    static void Clear() {
        Save({});
    }

private:
    static void Save(const std::vector<LeaderboardEntry>& entries) {
        nlohmann::json j = nlohmann::json::array();
        for (auto& e : entries) {
            j.push_back({
                {"name",    e.agencyName},
                {"score",   e.score},
                {"won",     e.won},
                {"peak",    e.peakShare},
                {"revenue", e.totalRevenue},
                {"date",    e.date}
            });
        }
        std::ofstream f(FILE_PATH);
        if (f.is_open()) f << j.dump(2);
    }

    static std::string CurrentDate() {
        time_t t = time(nullptr);
        struct tm* tm_info = localtime(&t);
        char buf[16];
        strftime(buf, sizeof(buf), "%Y-%m-%d", tm_info);
        return std::string(buf);
    }
};
