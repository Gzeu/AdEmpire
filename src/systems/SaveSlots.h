#pragma once
#include "../core/GameState.h"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

struct SaveSlotInfo {
    int         slot;       // 1-3
    std::string agencyName;
    int         month;
    int         year;
    float       budget;
    float       marketShare;
    bool        exists;
};

class SaveSlots {
public:
    static constexpr int MAX_SLOTS = 3;

    static std::string SlotPath(int slot) {
        return "save_slot_" + std::to_string(slot) + ".json";
    }

    static SaveSlotInfo ReadInfo(int slot) {
        SaveSlotInfo info{};
        info.slot = slot;
        std::ifstream f(SlotPath(slot));
        if (!f.is_open()) { info.exists = false; return info; }
        info.exists = true;
        // Simple line-by-line parse
        std::string line;
        while (std::getline(f, line)) {
            auto extract = [&](const std::string& key) -> std::string {
                auto pos = line.find('"' + key + '"');
                if (pos == std::string::npos) return "";
                auto colon = line.find(':', pos);
                if (colon == std::string::npos) return "";
                auto start = line.find_first_not_of(" \t\"", colon + 1);
                auto end   = line.find_last_not_of(", \t\"");
                return line.substr(start, end - start + 1);
            };
            if (line.find("agencyName") != std::string::npos)
                info.agencyName = extract("agencyName");
            if (line.find("\"month\"") != std::string::npos)
                info.month = std::stoi(extract("month").empty()?"0":extract("month"));
            if (line.find("\"year\"") != std::string::npos)
                info.year = std::stoi(extract("year").empty()?"0":extract("year"));
            if (line.find("budget") != std::string::npos)
                try { info.budget = std::stof(extract("budget")); } catch(...){}
            if (line.find("playerMarketShare") != std::string::npos)
                try { info.marketShare = std::stof(extract("playerMarketShare")); } catch(...){}
        }
        return info;
    }

    static bool Save(const GameState& gs, int slot) {
        std::ofstream f(SlotPath(slot));
        if (!f.is_open()) return false;
        f << "{\n";
        f << "  \"agencyName\": \"" << gs.agencyName << "\",\n";
        f << "  \"budget\": " << gs.budget << ",\n";
        f << "  \"month\": " << gs.month << ",\n";
        f << "  \"year\": " << gs.year << ",\n";
        f << "  \"playerMarketShare\": " << gs.playerMarketShare << ",\n";
        f << "  \"reputation\": " << gs.stats.reputation << ",\n";
        f << "  \"totalRevenue\": " << gs.stats.totalRevenue << ",\n";
        f << "  \"monthsPlayed\": " << gs.stats.monthsPlayed << ",\n";
        f << "  \"clientsAcquired\": " << gs.stats.clientsAcquired << ",\n";
        f << "  \"campaignsCompleted\": " << gs.stats.campaignsCompleted << "\n";
        f << "}\n";
        return true;
    }

    static bool Delete(int slot) {
        return std::remove(SlotPath(slot).c_str()) == 0;
    }

    static std::vector<SaveSlotInfo> GetAll() {
        std::vector<SaveSlotInfo> all;
        for (int i = 1; i <= MAX_SLOTS; i++)
            all.push_back(ReadInfo(i));
        return all;
    }
};
