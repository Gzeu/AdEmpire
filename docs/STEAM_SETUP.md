# Steam Integration Setup

## Prerequisites

1. Have a Steam App ID (register at https://partner.steamgames.com/)
2. Download Steamworks SDK
3. Copy `sdk/public/steam/` to `lib/steam/`
4. Copy `sdk/redistributable_bin/` platform libraries to project root

## Enable

1. Open `src/platform/SteamIntegration.h`
2. Uncomment `#define STEAM_ENABLED`
3. Create `steam_appid.txt` in project root with your App ID:
   ```
   480
   ```
   *(480 = Spacewar test app, replace with your real ID)*

4. Update `CMakeLists.txt`:

```cmake
target_include_directories(AdEmpire PRIVATE lib/steam)
if(WIN32)
    target_link_libraries(AdEmpire PRIVATE
        ${CMAKE_SOURCE_DIR}/lib/steam/steam_api.lib)
elseif(UNIX)
    target_link_libraries(AdEmpire PRIVATE
        ${CMAKE_SOURCE_DIR}/lib/steam/libsteam_api.so)
endif()
```

## Steam Achievements Setup

In Steamworks dashboard, create these achievements:

| API Name | Display Name |
|---|---|
| `ACH_FIRST_CLIENT` | First Client |
| `ACH_MARKET_CHALLENGER` | Market Challenger |
| `ACH_MARKET_LEADER` | Market Leader |
| `ACH_EMPIRE_BUILDER` | Empire Builder |
| `ACH_100K_REVENUE` | $100K Revenue |
| `ACH_FULL_TEAM` | Full Team |
| `ACH_SURVIVOR` | Survivor |
| `ACH_VIRAL_HIT` | Viral Hit |

## In-game calls

Achievements auto-sync when `AchievementSystem::Tick()` runs.
Add this in `AchievementSystem.cpp` unlock block:
```cpp
SteamIntegration::Get().UnlockAchievement(("ACH_" + a.title).c_str());
```
