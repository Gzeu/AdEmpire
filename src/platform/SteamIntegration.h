#pragma once

// Steam SDK Integration Stub
// To activate:
// 1. Download Steamworks SDK from https://partner.steamgames.com/
// 2. Copy sdk/public/steam/ to lib/steam/
// 3. Uncomment #define STEAM_ENABLED
// 4. Add to CMakeLists.txt:
//    target_include_directories(AdEmpire PRIVATE lib/steam)
//    target_link_libraries(AdEmpire PRIVATE steam_api)

// #define STEAM_ENABLED

#ifdef STEAM_ENABLED
#include "steam/steam_api.h"
#endif

class SteamIntegration {
public:
    static SteamIntegration& Get() { static SteamIntegration s; return s; }

    bool Init() {
#ifdef STEAM_ENABLED
        if (!SteamAPI_Init()) return false;
        initialized = true;
        return true;
#else
        return false;
#endif
    }

    void Tick() {
#ifdef STEAM_ENABLED
        if (initialized) SteamAPI_RunCallbacks();
#endif
    }

    void Shutdown() {
#ifdef STEAM_ENABLED
        if (initialized) SteamAPI_Shutdown();
#endif
    }

    // Unlock a Steam Achievement
    void UnlockAchievement(const char* apiName) {
#ifdef STEAM_ENABLED
        if (!initialized) return;
        SteamUserStats()->SetAchievement(apiName);
        SteamUserStats()->StoreStats();
#else
        (void)apiName;
#endif
    }

    // Set a leaderboard score
    void SubmitScore(const char* boardName, int score) {
#ifdef STEAM_ENABLED
        if (!initialized) return;
        SteamUserStats()->FindOrCreateLeaderboard(boardName,
            k_ELeaderboardSortMethodDescending,
            k_ELeaderboardDisplayTypeNumeric);
        // UploadLeaderboardScore called in callback
#else
        (void)boardName; (void)score;
#endif
    }

    bool initialized = false;
};

// Steam Achievement API names (configure in Steamworks dashboard)
// FIRST_CLIENT       -> achievement_first_client
// MARKET_LEADER      -> achievement_market_leader
// EMPIRE_BUILDER     -> achievement_empire_builder
// HUNDRED_K_REVENUE  -> achievement_100k
// FULL_TEAM          -> achievement_full_team
