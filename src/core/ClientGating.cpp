#include "ClientGating.h"

std::vector<GatedClient> ClientGating::BuildPool() {
    return {
        // Month 1 — starter pool (added in MainMenu::Init)
        // Month 2+
        {{7,  "GreenGrove Organic",   ClientIndustry::Food,    2500,  70, 6,  false, true, false, ContractType::Monthly, {}, 0}, 2,  0.f},
        {{8,  "LuxeLook Cosmetics",   ClientIndustry::Fashion, 7000,  70, 6,  false, true, false, ContractType::Monthly, {}, 0}, 2,  10.f},
        // Month 3+
        {{9,  "CloudStack Inc",       ClientIndustry::Tech,    15000, 70, 12, false, true, false, ContractType::Monthly, {}, 0}, 3,  15.f},
        {{10, "AutoDrive Motors",     ClientIndustry::Retail,  20000, 70, 12, false, true, false, ContractType::Monthly, {}, 0}, 3,  20.f},
        {{11, "FreshPress Juices",    ClientIndustry::Food,    1800,  70, 6,  false, true, false, ContractType::Monthly, {}, 0}, 3,  5.f},
        // Month 4+
        {{12, "PixelForge Games",     ClientIndustry::Gaming,  9000,  70, 12, false, true, false, ContractType::Monthly, {}, 0}, 4,  25.f},
        {{13, "BlueSky Airlines",     ClientIndustry::Retail,  25000, 70, 12, false, true, false, ContractType::Monthly, {}, 0}, 5,  30.f},
        {{14, "NovaCure Pharma",      ClientIndustry::Health,  18000, 70, 12, false, true, false, ContractType::Monthly, {}, 0}, 5,  35.f},
        {{15, "ZenMind Wellness",     ClientIndustry::Health,  4000,  70, 6,  false, true, false, ContractType::Monthly, {}, 0}, 4,  10.f},
        // Month 6+
        {{16, "CryptoVault Exchange", ClientIndustry::Finance, 30000, 70, 12, false, true, false, ContractType::Monthly, {}, 0}, 6,  40.f},
        {{17, "TrendBurst Apparel",   ClientIndustry::Fashion, 6000,  70, 6,  false, true, false, ContractType::Monthly, {}, 0}, 6,  20.f},
        {{18, "EcoNest Homes",        ClientIndustry::Retail,  8500,  70, 6,  false, true, false, ContractType::Monthly, {}, 0}, 6,  25.f},
        {{19, "BrainWave EdTech",     ClientIndustry::Education,5500, 70, 9,  false, true, false, ContractType::Monthly, {}, 0}, 6,  20.f},
        // Month 9+
        {{20, "RoboArm Industrial",   ClientIndustry::Tech,    22000, 70, 12, false, true, false, ContractType::Monthly, {}, 0}, 9,  50.f},
        {{21, "SunRise Coffee",       ClientIndustry::Food,    3200,  70, 6,  false, true, false, ContractType::Monthly, {}, 0}, 7,  15.f},
        {{22, "DataMind Analytics",   ClientIndustry::Tech,    11000, 70, 12, false, true, false, ContractType::Monthly, {}, 0}, 8,  40.f},
        {{23, "ActiveLife Sports",    ClientIndustry::Retail,  7500,  70, 6,  false, true, false, ContractType::Monthly, {}, 0}, 7,  20.f},
        {{24, "NatureGlow Skincare",  ClientIndustry::Health,  5800,  70, 6,  false, true, false, ContractType::Monthly, {}, 0}, 8,  30.f},
        // Month 12+ — endgame clients
        {{25, "MetaVerse Studio",     ClientIndustry::Gaming,  14000, 70, 12, false, true, false, ContractType::Monthly, {}, 0}, 12, 60.f},
        {{26, "PocketBank",           ClientIndustry::Finance, 16000, 70, 12, false, true, false, ContractType::Monthly, {}, 0}, 12, 55.f},
        {{27, "UrbanEats Delivery",   ClientIndustry::Food,    6500,  70, 6,  false, true, false, ContractType::Monthly, {}, 0}, 10, 35.f},
        {{28, "SmartHome Devices",    ClientIndustry::Tech,    9500,  70, 12, false, true, false, ContractType::Monthly, {}, 0}, 10, 40.f},
        {{29, "LegalEase Platform",   ClientIndustry::Tech,    7200,  70, 9,  false, true, false, ContractType::Monthly, {}, 0}, 11, 45.f},
        {{30, "GlobeTravel Agency",   ClientIndustry::Retail,  13000, 70, 12, false, true, false, ContractType::Monthly, {}, 0}, 12, 50.f},
    };
}

void ClientGating::UnlockClients(GameState& gs) {
    static std::vector<GatedClient> pool = BuildPool();
    for (auto& gc : pool) {
        // Skip already added
        bool exists = false;
        for (auto& c : gs.clients) if (c.id == gc.client.id) { exists = true; break; }
        if (exists) continue;
        if (gs.stats.monthsPlayed >= gc.unlockMonth &&
            gs.stats.reputation  >= gc.minReputation) {
            gs.clients.push_back(gc.client);
        }
    }
}
