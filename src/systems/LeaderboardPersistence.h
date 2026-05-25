#pragma once
#include "../systems/Leaderboard.h"
#include "json.hpp"
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>

// ============================================================
//  LeaderboardPersistence v1.1
//  Saves/loads top-50 entries to leaderboard.json on disk
//  (placed next to the binary in the working directory)
//  Zero external deps beyond nlohmann/json (already in project)
// ============================================================

class LeaderboardPersistence {
public:
    static LeaderboardPersistence& Get() {
        static LeaderboardPersistence inst;
        return inst;
    }

    void AddEntry(LeaderboardEntry entry) {
        Load();
        entries_.push_back(std::move(entry));
        // Sort descending by score
        std::sort(entries_.begin(), entries_.end(),
            [](const LeaderboardEntry& a, const LeaderboardEntry& b) {
                return a.score > b.score;
            });
        // Keep top 50
        if (entries_.size() > 50)
            entries_.resize(50);
        Save();
    }

    const std::vector<LeaderboardEntry>& GetEntries() {
        Load();
        return entries_;
    }

    void Clear() {
        entries_.clear();
        Save();
    }

    bool Exists() const {
        std::ifstream f(kPath);
        return f.good();
    }

private:
    static constexpr const char* kPath = "leaderboard.json";
    std::vector<LeaderboardEntry> entries_;
    bool loaded_ = false;

    void Load() {
        if (loaded_) return;
        loaded_ = true;
        try {
            std::ifstream f(kPath);
            if (!f.is_open()) return;
            auto j = nlohmann::json::parse(f);
            entries_.clear();
            for (auto& item : j["entries"]) {
                LeaderboardEntry e;
                e.agencyName  = item.value("agencyName",  "");
                e.score       = item.value("score",       0LL);
                e.marketShare = item.value("marketShare", 0.f);
                e.revenue     = item.value("revenue",     0.f);
                e.months      = item.value("months",      0);
                e.difficulty  = item.value("difficulty",  "Normal");
                entries_.push_back(e);
            }
        } catch (...) { entries_.clear(); }
    }

    void Save() const {
        try {
            nlohmann::json j;
            j["entries"] = nlohmann::json::array();
            for (const auto& e : entries_) {
                j["entries"].push_back({
                    {"agencyName",  e.agencyName},
                    {"score",       e.score},
                    {"marketShare", e.marketShare},
                    {"revenue",     e.revenue},
                    {"months",      e.months},
                    {"difficulty",  e.difficulty}
                });
            }
            std::ofstream f(kPath);
            f << j.dump(2);
        } catch (...) {}
    }

    LeaderboardPersistence()  = default;
    ~LeaderboardPersistence() = default;
    LeaderboardPersistence(const LeaderboardPersistence&) = delete;
    LeaderboardPersistence& operator=(const LeaderboardPersistence&) = delete;
};
