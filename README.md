# 🎮 AdEmpire — Marketing Tycoon

> Build and grow a digital marketing agency from scratch. Manage clients, launch campaigns, beat AI rivals, and become the #1 ad agency in the market.

![C++](https://img.shields.io/badge/C++-17-blue.svg)
![ImGui](https://img.shields.io/badge/Dear%20ImGui-latest-green.svg)
![OpenGL](https://img.shields.io/badge/OpenGL-3.3-orange.svg)
![CMake](https://img.shields.io/badge/CMake-3.16+-red.svg)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows%20%7C%20macOS-lightgrey.svg)
![License](https://img.shields.io/badge/license-MIT-purple.svg)
![Build](https://img.shields.io/badge/build-passing-brightgreen.svg)
![Version](https://img.shields.io/badge/version-2.0.0-blue.svg)
![Commits](https://img.shields.io/badge/commits-11-orange.svg)
![Files](https://img.shields.io/badge/source%20files-50%2B-blueviolet.svg)

---

## 🕹️ Gameplay

You start with **$10,000** and zero clients. Every month you:
- **Pitch** to potential clients across 8 industries (30 clients, month-gated unlock)
- **Launch campaigns** on 6 channels (Social, SEO, Email, Influencer, PR, Paid Search)
- **Hire and level up staff** — 5 levels from Junior to Principal
- **Survive market events** — algorithm changes, viral surges, recessions, PR crises
- **Outmaneuver** 3 AI rival agencies that adapt and try to poach your clients
- **Unlock achievements** — 15 milestones with instant toast notifications

**Win condition:** Reach **35% market share**  
**Lose condition:** Budget drops below **-$50,000**  
**Difficulty:** Easy ($20K) / Normal ($10K) / Hard ($5K + aggressive AI)

---

## 🧠 Full Feature Matrix

### v1.0 — Core Systems

| System | File | Description |
|---|---|---|
| Campaign Engine | `src/systems/CampaignEngine.cpp` | Reach × CTR × Conv × AOV → Revenue math |
| AI Competitors | `src/systems/AICompetitor.cpp` | 3 agencies: MediaBlaze, PeakBrands, NicheNation |
| Event System | `src/systems/EventSystem.cpp` | 20 market events, per-channel modifiers |
| Client Manager | `src/ui/ClientManager.cpp` | 30 clients — pitch, satisfaction, contracts |
| Staff System | `src/ui/StaffPanel.cpp` | 6 roles, skill-based campaign bonuses |
| Market Map | `src/ui/MarketMap.cpp` | Real-time market share bar visualization |
| Dashboard | `src/ui/Dashboard.cpp` | 24-month revenue chart, live KPIs |
| Save System | `src/systems/SaveSystem.cpp` | JSON save/load |
| Newsfeed | `src/ui/Newsfeed.cpp` | Active events + history log |

### v1.1 — Polish

| Feature | File | Description |
|---|---|---|
| Custom Fonts | [`src/ui/FontManager.h`](https://github.com/Gzeu/AdEmpire/blob/main/src/ui/FontManager.h) | Roboto-Regular.ttf from `assets/fonts/`, auto-fallback |
| Lerp Animations | [`src/ui/Animator.h`](https://github.com/Gzeu/AdEmpire/blob/main/src/ui/Animator.h) | `Animator::Get().SetTarget("budget", value)` → smooth 8fps lerp |
| Toast Notifications | [`src/ui/ToastSystem.h`](https://github.com/Gzeu/AdEmpire/blob/main/src/ui/ToastSystem.h) | 4 types (Info/Success/Warning/Error), fade-out, max 6 simultaneous |
| Difficulty Settings | [`src/core/Difficulty.h`](https://github.com/Gzeu/AdEmpire/blob/main/src/core/Difficulty.h) | Easy $20K / Normal $10K / Hard $5K + AI 100% aggression |
| Month-Gated Clients | [`src/core/ClientGating.cpp`](https://github.com/Gzeu/AdEmpire/blob/main/src/core/ClientGating.cpp) | 24 clients unlocked progressively month 2→12, rep-gated |

### v1.2 — Depth

| Feature | File | Description |
|---|---|---|
| Achievements (15) | [`src/systems/AchievementSystem.cpp`](https://github.com/Gzeu/AdEmpire/blob/main/src/systems/AchievementSystem.cpp) | Lambda conditions, auto-toast on unlock, 2-col grid panel |
| Staff Leveling | [`src/systems/StaffLeveling.cpp`](https://github.com/Gzeu/AdEmpire/blob/main/src/systems/StaffLeveling.cpp) | 5 levels (Junior→Principal), +0.8% skill/mo, +8% salary on level-up |
| Campaign Templates | [`src/systems/CampaignTemplates.cpp`](https://github.com/Gzeu/AdEmpire/blob/main/src/systems/CampaignTemplates.cpp) | 15 templates with quality bonus, sorted by client industry |
| Seasonal Events | [`src/systems/SeasonalEvents.cpp`](https://github.com/Gzeu/AdEmpire/blob/main/src/systems/SeasonalEvents.cpp) | All 12 months: Nov +60% (Black Friday), Dec +80% (Xmas), Jul -20% |
| Industry Bonuses | [`src/systems/IndustryBonuses.cpp`](https://github.com/Gzeu/AdEmpire/blob/main/src/systems/IndustryBonuses.cpp) | Fashion→Influencer ×1.6, Tech→SEO ×1.5, Finance→Paid ×1.5 |

### v2.0 — Extended

| Feature | Files | Description |
|---|---|---|
| 3 Save Slots | [`SaveSlots.h`](https://github.com/Gzeu/AdEmpire/blob/main/src/systems/SaveSlots.h) + [`SaveSlotsPanel.cpp`](https://github.com/Gzeu/AdEmpire/blob/main/src/ui/SaveSlotsPanel.cpp) | Visual slot panel, Save/Delete per slot, full info display |
| Local Leaderboard | [`Leaderboard.h`](https://github.com/Gzeu/AdEmpire/blob/main/src/systems/Leaderboard.h) + [`LeaderboardPanel.cpp`](https://github.com/Gzeu/AdEmpire/blob/main/src/ui/LeaderboardPanel.cpp) | Top 10, sort by revenue, gold/silver/bronze, persistent JSON |
| Moddable Events | [`ModLoader.h`](https://github.com/Gzeu/AdEmpire/blob/main/src/systems/ModLoader.h) + [`custom_events.json`](https://github.com/Gzeu/AdEmpire/blob/main/assets/data/custom_events.json) | Edit JSON, auto-loaded at startup |
| OpenAL Audio | [`AudioSystem.h`](https://github.com/Gzeu/AdEmpire/blob/main/src/audio/AudioSystem.h) + [`AUDIO_SETUP.md`](https://github.com/Gzeu/AdEmpire/blob/main/docs/AUDIO_SETUP.md) | Full stub + `#define AUDIO_ENABLED`, install guide |
| Steam Integration | [`SteamIntegration.h`](https://github.com/Gzeu/AdEmpire/blob/main/src/platform/SteamIntegration.h) + [`STEAM_SETUP.md`](https://github.com/Gzeu/AdEmpire/blob/main/docs/STEAM_SETUP.md) | Achievements API, leaderboard submit, full Steamworks guide |

---

## 📦 Tech Stack

| Library | Version | Purpose |
|---|---|---|
| [Dear ImGui](https://github.com/ocornut/imgui) | latest (submodule) | All UI — immediate mode |
| GLFW 3 | 3.3+ | Window creation & input |
| OpenGL | 3.3 core | Rendering backend |
| nlohmann/json | auto-downloaded | Save system & JSON data |
| OpenAL | optional | Audio (enable via `#define AUDIO_ENABLED`) |
| Steamworks SDK | optional | Steam achievements & leaderboard |
| CMake | 3.16+ | Build system |

---

## 🚀 Build & Run

### Prerequisites (Ubuntu/Debian)
```bash
sudo apt update
sudo apt install cmake build-essential libglfw3-dev libgl1-mesa-dev libglu1-mesa-dev
```

### Clone with ImGui submodule
```bash
git clone --recurse-submodules https://github.com/Gzeu/AdEmpire.git
cd AdEmpire
```

> Already cloned without submodules?
> ```bash
> git submodule update --init --recursive
> ```

### Build
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
./AdEmpire
```

### Windows (MSVC / vcpkg)
```bash
vcpkg install glfw3 opengl
cmake .. -DCMAKE_TOOLCHAIN_FILE=[vcpkg root]/scripts/buildsystems/vcpkg.cmake
```

---

## 🔌 Optional Features Setup

### Custom Fonts (Roboto)
```bash
wget https://github.com/google/fonts/raw/main/apache/roboto/Roboto-Regular.ttf \
     -O assets/fonts/Roboto-Regular.ttf
# FontManager auto-detects and loads it on next build
```

### OpenAL Audio
```bash
sudo apt install libopenal-dev
# Then uncomment: #define AUDIO_ENABLED in src/audio/AudioSystem.h
# Full guide: docs/AUDIO_SETUP.md
```

### Steam Integration
```bash
# Download Steamworks SDK from https://partner.steamgames.com
# Full setup guide: docs/STEAM_SETUP.md
```

### Moddable Events
```bash
# Edit assets/data/custom_events.json
# Events are loaded automatically at game start
# Format documented in: assets/data/events.json
```

---

## 📁 Full Project Structure

```
AdEmpire/
├── src/
│   ├── main.cpp                      # Entry point + GLFW + ImGui game loop
│   ├── core/
│   │   ├── GameState.h               # All data structs + std::hash<ChannelType>
│   │   ├── Simulation.cpp            # Monthly economic simulation engine
│   │   ├── Difficulty.h              # Easy/Normal/Hard presets
│   │   └── ClientGating.cpp          # Progressive client unlock (month + rep gated)
│   ├── systems/
│   │   ├── CampaignEngine.cpp        # Campaign creation + ROI estimation
│   │   ├── AICompetitor.cpp          # Rival agency AI logic
│   │   ├── EventSystem.cpp           # 20 market events with modifiers
│   │   ├── SaveSystem.cpp            # JSON save/load
│   │   ├── AchievementSystem.cpp     # 15 achievements with lambda conditions
│   │   ├── StaffLeveling.cpp         # 5 levels, skill/salary progression
│   │   ├── CampaignTemplates.cpp     # 15 quick-launch templates
│   │   ├── SeasonalEvents.cpp        # Monthly seasonal modifiers
│   │   ├── IndustryBonuses.cpp       # Per-industry channel multipliers
│   │   ├── SaveSlots.h               # 3 save slots system
│   │   ├── Leaderboard.h             # Local top-10 leaderboard
│   │   └── ModLoader.h               # Custom events JSON loader
│   ├── ui/
│   │   ├── Theme.h                   # Dark blue marketing theme
│   │   ├── FontManager.h             # Custom TTF font loader
│   │   ├── Animator.h               # Lerp animation system
│   │   ├── ToastSystem.h             # Popup notifications
│   │   ├── MainMenu.cpp              # Start screen + difficulty select
│   │   ├── Dashboard.cpp             # KPIs + 24-month revenue chart
│   │   ├── CampaignEditor.cpp        # Create campaigns + live ROI preview
│   │   ├── ClientManager.cpp         # Pitch + manage clients
│   │   ├── MarketMap.cpp             # Market share visualization
│   │   ├── Newsfeed.cpp              # Events log
│   │   ├── StaffPanel.cpp            # Hire & manage team
│   │   ├── SaveSlotsPanel.cpp        # 3-slot save UI
│   │   └── LeaderboardPanel.cpp      # Top-10 leaderboard UI
│   ├── audio/
│   │   └── AudioSystem.h             # OpenAL stub (opt-in)
│   └── platform/
│       └── SteamIntegration.h        # Steamworks stub (opt-in)
├── assets/
│   ├── data/
│   │   ├── clients.json              # 30 fictional clients
│   │   ├── events.json               # Events reference
│   │   ├── channels.json             # 6 channels with base stats
│   │   └── custom_events.json        # Moddable events (edit freely)
│   └── fonts/                        # Drop Roboto-Regular.ttf here
├── docs/
│   ├── AUDIO_SETUP.md
│   └── STEAM_SETUP.md
├── lib/
│   └── imgui/                        # git submodule → ocornut/imgui
├── CMakeLists.txt
├── SETUP.md
└── .gitmodules
```

---

## 🔧 Compilation Fixes (Audit v1.0)

All 4 issues resolved in commit [b9ca0247](https://github.com/Gzeu/AdEmpire/commit/b9ca0247d99b072228a43cb0214672746fe69997):

| Issue | Fix |
|---|---|
| `nlohmann/json` not found | CMakeLists auto-downloads `json.hpp` via `file(DOWNLOAD ...)` |
| `std::hash<ChannelType>` missing | Template specialization added in `GameState.h` with `#include <functional>` |
| `rand() % 8` magic number | Replaced with `STAFF_NAME_COUNT` constant |
| `ClientManager` missing `#include <algorithm>` | Include added |

> ✅ **Zero known compilation blockers** on GCC 11+ / Clang 14+ / Ubuntu 22.04+

---

## 📊 Campaign Math

The simulation engine uses this formula per month:

```
Reach           = Budget × ReachPerDollar[channel] × ChannelMod × SeasonalMod × IndustryBonus × StaffBonus
CTR             = BaseCTR[channel] × ChannelMod × QualityModifier
Conversions     = Reach × CTR × BaseConversion[channel]
ClientRevenue   = Conversions × AverageOrderValue
AgencyFee       = ClientRevenue × 15%
```

Channel modifiers stack: market events × seasonal events × industry bonuses × staff skill.

---

## 🎯 Version History

| Version | Commit | Highlights |
|---|---|---|
| v1.0 | Initial commits | Core engine, all base systems |
| Audit v1.0 | [b9ca0247](https://github.com/Gzeu/AdEmpire/commit/b9ca0247d99b072228a43cb0214672746fe69997) | 4 compilation fixes, zero blockers |
| v1.1 Polish | [eb3a5a5d](https://github.com/Gzeu/AdEmpire/commit/eb3a5a5d257b5aae8fb55b645d044d01eba702c1) | Fonts, lerp animations, toasts, difficulty, month-gated clients |
| v1.2 Depth | [78c27454](https://github.com/Gzeu/AdEmpire/commit/78c27454fea3c206237d49da2ea4dafce55b0a3d) | 15 achievements, staff leveling, templates, seasonal events, industry bonuses |
| v2.0 Extended | [f954fe75](https://github.com/Gzeu/AdEmpire/commit/f954fe75016326b12038bf62e5551ce82825f5f4) | 3 save slots, leaderboard, moddable events, OpenAL stub, Steam stub |

---

## 🚀 Roadmap — What's Next

- [ ] Wire all v1.1→v2.0 systems into `main.cpp` (integration pass)
- [ ] Add actual `.wav` sound effects (campaign launch, client won, level up)
- [ ] Android/iOS port via ImGui + SDL2
- [ ] Online multiplayer leaderboard (REST API)
- [ ] Steam Greenlight submission

---

## 📸 Screenshots

> *Add screenshots of Dashboard, Campaign Editor, and Market Map here.*

---

## 📄 License

MIT © 2026 [George Pricop](https://github.com/Gzeu) — Free to use, modify, and distribute.
