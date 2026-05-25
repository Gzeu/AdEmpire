#pragma once
#include "../core/GameState.h"
#include <string>
#include <array>
#include <fstream>
#include <sstream>

struct SaveSlot {
    bool        occupied   = false;
    std::string agencyName;
    float       budget     = 0;
    float       marketShare= 0;
    int         month      = 0;
    int         year       = 0;
    int         clients    = 0;
    std::string savedAt;       // timestamp string
    std::string filepath;      // e.g. "save_slot1.json"
};

class SaveSlots {
public:
    static constexpr int SLOT_COUNT = 3;
    static std::array<SaveSlot, SLOT_COUNT> s_slots;

    static void     LoadMeta();          // read existing save files to populate summaries
    static bool     Save(int slot, const GameState& gs);
    static bool     Load(int slot, GameState& gs);
    static void     Delete(int slot);
    static SaveSlot GetInfo(int slot);

private:
    static std::string SlotPath(int slot);
};

// --- Implementation (header-only for simplicity) ---
#include <cstdio>
#include <ctime>

inline std::string SaveSlots::SlotPath(int slot) {
    char buf[32];
    snprintf(buf, 32, "save_slot%d.json", slot + 1);
    return buf;
}

inline std::array<SaveSlot, SaveSlots::SLOT_COUNT> SaveSlots::s_slots = {};

inline void SaveSlots::LoadMeta() {
    for (int i = 0; i < SLOT_COUNT; i++) {
        auto& sl = s_slots[i];
        sl.filepath = SlotPath(i);
        std::ifstream f(sl.filepath);
        if (!f.is_open()) { sl.occupied = false; continue; }
        sl.occupied = true;
        // Read key fields only (fast meta scan — no full parse needed)
        std::string line;
        while (std::getline(f, line)) {
            auto extract = [&](const char* key, auto& out) {
                auto pos = line.find(key);
                if (pos != std::string::npos) {
                    std::istringstream ss(line.substr(pos + strlen(key)));
                    ss >> out;
                }
            };
            if (line.find("agencyName") != std::string::npos) {
                auto a = line.find('"', line.find(':') + 1) + 1;
                auto b = line.find('"', a);
                if (a != std::string::npos && b != std::string::npos)
                    sl.agencyName = line.substr(a, b - a);
            }
            if (line.find("budget")      != std::string::npos && sl.budget == 0)     { float v; std::istringstream ss(line.substr(line.find(':') + 1)); ss >> v; sl.budget = v; }
            if (line.find("market")      != std::string::npos && sl.marketShare == 0){ float v; std::istringstream ss(line.substr(line.find(':') + 1)); ss >> v; sl.marketShare = v; }
            if (line.find("\"month\":")  != std::string::npos && sl.month == 0)      { int v;   std::istringstream ss(line.substr(line.find(':') + 1)); ss >> v; sl.month = v; }
            if (line.find("\"year\":")   != std::string::npos && sl.year == 0)       { int v;   std::istringstream ss(line.substr(line.find(':') + 1)); ss >> v; sl.year = v; }
            if (line.find("savedAt")     != std::string::npos) {
                auto a = line.find('"', line.find(':') + 1) + 1;
                auto b = line.find('"', a);
                if (a != std::string::npos && b != std::string::npos)
                    sl.savedAt = line.substr(a, b - a);
            }
        }
    }
}

inline bool SaveSlots::Save(int slot, const GameState& gs) {
    if (slot < 0 || slot >= SLOT_COUNT) return false;
    std::string path = SlotPath(slot);
    std::ofstream f(path);
    if (!f.is_open()) return false;
    // Get timestamp
    time_t t = time(nullptr);
    char ts[32]; strftime(ts, 32, "%Y-%m-%d %H:%M", localtime(&t));
    int activeClients = 0;
    for (auto& c : gs.clients) if (c.active) activeClients++;
    f << "{\n";
    f << "  \"agencyName\": \"" << gs.agencyName << "\",\n";
    f << "  \"budget\": "       << gs.budget << ",\n";
    f << "  \"month\": "        << gs.month << ",\n";
    f << "  \"year\": "         << gs.year << ",\n";
    f << "  \"playerMarketShare\": " << gs.playerMarketShare << ",\n";
    f << "  \"reputation\": "   << gs.stats.reputation << ",\n";
    f << "  \"totalRevenue\": " << gs.stats.totalRevenue << ",\n";
    f << "  \"monthsPlayed\": " << gs.stats.monthsPlayed << ",\n";
    f << "  \"activeClients\": "<< activeClients << ",\n";
    f << "  \"savedAt\": \""    << ts << "\"\n";
    f << "}\n";
    // Update meta
    s_slots[slot].occupied    = true;
    s_slots[slot].agencyName  = gs.agencyName;
    s_slots[slot].budget      = gs.budget;
    s_slots[slot].marketShare = gs.playerMarketShare;
    s_slots[slot].month       = gs.month;
    s_slots[slot].year        = gs.year;
    s_slots[slot].clients     = activeClients;
    s_slots[slot].savedAt     = ts;
    s_slots[slot].filepath    = path;
    return true;
}

inline bool SaveSlots::Load(int slot, GameState& gs) {
    if (slot < 0 || slot >= SLOT_COUNT) return false;
    if (!s_slots[slot].occupied) return false;
    // Stub: extend with nlohmann/json for full restore
    gs.agencyName        = s_slots[slot].agencyName;
    gs.budget            = s_slots[slot].budget;
    gs.playerMarketShare = s_slots[slot].marketShare;
    gs.month             = s_slots[slot].month;
    gs.year              = s_slots[slot].year;
    return true;
}

inline void SaveSlots::Delete(int slot) {
    if (slot < 0 || slot >= SLOT_COUNT) return;
    remove(SlotPath(slot).c_str());
    s_slots[slot] = {};
    s_slots[slot].filepath = SlotPath(slot);
}

inline SaveSlot SaveSlots::GetInfo(int slot) {
    if (slot < 0 || slot >= SLOT_COUNT) return {};
    return s_slots[slot];
}
