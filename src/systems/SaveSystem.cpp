#include "SaveSystem.h"
#include <fstream>
#include <iostream>

bool SaveSystem::Save(const GameState& gs, const std::string& path) {
    std::ofstream f(path);
    if (!f.is_open()) return false;
    f << "{\n"
      << "  \"agencyName\": \""    << gs.agencyName           << "\",\n"
      << "  \"budget\": "          << gs.budget               << ",\n"
      << "  \"month\": "           << gs.month                << ",\n"
      << "  \"year\": "            << gs.year                 << ",\n"
      << "  \"playerMarketShare\": "<< gs.playerMarketShare   << ",\n"
      << "  \"reputation\": "       << gs.stats.reputation    << ",\n"
      << "  \"totalRevenue\": "     << gs.stats.totalRevenue  << ",\n"
      << "  \"monthsPlayed\": "     << gs.stats.monthsPlayed  << ",\n"
      << "  \"clientsAcquired\": " << gs.stats.clientsAcquired << ",\n"
      << "  \"clientsLost\": "      << gs.stats.clientsLost   << "\n"
      << "}\n";
    return true;
}

bool SaveSystem::HasSave(const std::string& path) {
    return std::ifstream(path).good();
}

bool SaveSystem::Load(GameState& /*gs*/, const std::string& path) {
    if (!std::ifstream(path).is_open()) return false;
    std::cout << "[SaveSystem] Save found at " << path
              << " (full restore stub - add nlohmann/json)\n";
    return true;
}
