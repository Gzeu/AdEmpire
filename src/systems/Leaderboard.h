#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>

struct LeaderboardEntry {
    std::string agencyName;
    float       totalRevenue;
    float       marketShare;
    int         monthsPlayed;
    int         clientsAcquired;
};

class Leaderboard {
public:
    static constexpr const char* PATH = "leaderboard.json";
    static constexpr int MAX_ENTRIES  = 10;

    static void Submit(const std::string& name, float revenue,
                       float share, int months, int clients) {
        auto entries = Load();
        entries.push_back({name, revenue, share, months, clients});
        // Sort by total revenue desc
        std::sort(entries.begin(), entries.end(),
            [](const LeaderboardEntry& a, const LeaderboardEntry& b){
                return a.totalRevenue > b.totalRevenue; });
        if ((int)entries.size() > MAX_ENTRIES)
            entries.resize(MAX_ENTRIES);
        Save(entries);
    }

    static std::vector<LeaderboardEntry> Load() {
        std::vector<LeaderboardEntry> result;
        std::ifstream f(PATH);
        if (!f.is_open()) return result;
        std::string line;
        LeaderboardEntry cur{};
        while (std::getline(f, line)) {
            if (line.find("agencyName") != std::string::npos) {
                auto s = line.find('"', line.find(':') + 1) + 1;
                auto e = line.find('"', s);
                cur.agencyName = line.substr(s, e - s);
            }
            if (line.find("totalRevenue") != std::string::npos)
                try { cur.totalRevenue = std::stof(line.substr(line.find(':') + 1)); } catch(...){}
            if (line.find("marketShare") != std::string::npos)
                try { cur.marketShare = std::stof(line.substr(line.find(':') + 1)); } catch(...){}
            if (line.find("monthsPlayed") != std::string::npos)
                try { cur.monthsPlayed = std::stoi(line.substr(line.find(':') + 1)); } catch(...){}
            if (line.find("clientsAcquired") != std::string::npos) {
                try { cur.clientsAcquired = std::stoi(line.substr(line.find(':') + 1)); } catch(...){}
                result.push_back(cur); cur = {};
            }
        }
        return result;
    }

    static void Save(const std::vector<LeaderboardEntry>& entries) {
        std::ofstream f(PATH);
        f << "[\n";
        for (int i = 0; i < (int)entries.size(); i++) {
            auto& e = entries[i];
            f << "  {\n";
            f << "    \"agencyName\": \"" << e.agencyName << "\",\n";
            f << "    \"totalRevenue\": " << e.totalRevenue << ",\n";
            f << "    \"marketShare\": " << e.marketShare << ",\n";
            f << "    \"monthsPlayed\": " << e.monthsPlayed << ",\n";
            f << "    \"clientsAcquired\": " << e.clientsAcquired << "\n";
            f << "  }" << (i < (int)entries.size()-1 ? "," : "") << "\n";
        }
        f << "]\n";
    }
};
