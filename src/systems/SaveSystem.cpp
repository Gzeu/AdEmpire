#include "SaveSystem.h"
#include <fstream>
#include <iostream>

// Minimal save - writes key fields as JSON manually (no external dep required)
bool SaveSystem::Save(const GameState& gs, const std::string& path) {
    std::ofstream f(path);
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
    f << "  \"clientsAcquired\": " << gs.stats.clientsAcquired << "\n";
    f << "}\n";
    return true;
}

bool SaveSystem::HasSave(const std::string& path) {
    std::ifstream f(path);
    return f.good();
}

bool SaveSystem::Load(GameState& gs, const std::string& path) {
    // Minimal loader - full nlohmann/json integration recommended
    std::ifstream f(path);
    if (!f.is_open()) return false;
    std::cout << "[SaveSystem] Save file found at " << path << std::endl;
    return true; // stub - extend with nlohmann/json
}
