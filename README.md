# 🎮 AdEmpire — Marketing Tycoon

> Build and grow a digital marketing agency from scratch. Manage clients, launch campaigns, beat AI rivals, and become the #1 ad agency in the market.

![C++](https://img.shields.io/badge/C++-17-blue.svg)
![ImGui](https://img.shields.io/badge/Dear%20ImGui-latest-green.svg)
![OpenGL](https://img.shields.io/badge/OpenGL-3.3-orange.svg)
![CMake](https://img.shields.io/badge/CMake-3.16+-red.svg)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows%20%7C%20macOS-lightgrey.svg)
![License](https://img.shields.io/badge/license-MIT-purple.svg)
![Build](https://img.shields.io/badge/build-passing-brightgreen.svg)

---

## 🕹️ Gameplay

You start with **$10,000** and zero clients. Every month you:
- **Pitch** to potential clients across 8 industries
- **Launch campaigns** on 6 channels (Social, SEO, Email, Influencer, PR, Paid Search)
- **Hire staff** that boost campaign performance
- **Survive market events** — algorithm changes, viral surges, recessions, PR crises
- **Outmaneuver** 3 AI rival agencies trying to poach your clients

**Win condition:** Reach **35% market share**  
**Lose condition:** Budget drops below **-$50,000**

---

## 🧠 Core Systems

| System | Description | Status |
|---|---|---|
| Campaign Engine | Math simulation: Reach × CTR × Conv × AOV → Revenue | ✅ |
| AI Competitors | 3 rival agencies (MediaBlaze, PeakBrands, NicheNation) | ✅ |
| Event System | 20 market events with per-channel modifiers | ✅ |
| Client Manager | 30 clients — pitch, satisfy, retain | ✅ |
| Staff System | 6 roles, skill-based campaign bonuses | ✅ |
| Market Map | Real-time market share bar visualization | ✅ |
| Dashboard | Revenue chart (24mo history), live KPIs | ✅ |
| Save System | JSON save/load | ✅ |
| Newsfeed | Active events + history log | ✅ |

---

## 📦 Tech Stack

| Library | Version | Purpose |
|---|---|---|
| [Dear ImGui](https://github.com/ocornut/imgui) | latest (submodule) | All UI — immediate mode |
| GLFW 3 | 3.3+ | Window creation & input |
| OpenGL | 3.3 core | Rendering backend |
| nlohmann/json | auto-downloaded | Save system & JSON data |
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

## 📁 Project Structure

```
AdEmpire/
├── src/
│   ├── main.cpp                   # Entry point + GLFW + ImGui game loop
│   ├── core/
│   │   ├── GameState.h            # All data structs (Campaign, Client, Staff, AI...)
│   │   ├── GameState.cpp
│   │   └── Simulation.cpp         # Monthly economic simulation engine
│   ├── systems/
│   │   ├── CampaignEngine.cpp     # Campaign creation + ROI estimation
│   │   ├── AICompetitor.cpp       # Rival agency AI logic
│   │   ├── EventSystem.cpp        # 20 market events with modifiers
│   │   └── SaveSystem.cpp         # JSON save/load
│   └── ui/
│       ├── Theme.h                # Dark blue marketing theme
│       ├── MainMenu.cpp           # Start screen
│       ├── Dashboard.cpp          # KPIs + 24-month revenue chart
│       ├── CampaignEditor.cpp     # Create campaigns + live ROI preview
│       ├── ClientManager.cpp      # Pitch + manage clients
│       ├── MarketMap.cpp          # Market share visualization
│       ├── Newsfeed.cpp           # Events log
│       └── StaffPanel.cpp         # Hire & manage team
├── assets/
│   ├── data/
│   │   ├── clients.json           # 30 fictional clients
│   │   ├── events.json            # Events reference
│   │   └── channels.json          # 6 channels with base stats
│   └── fonts/                     # Drop custom .ttf fonts here
├── lib/
│   └── imgui/                     # git submodule → ocornut/imgui
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
| `std::hash<ChannelType>` missing | Added template specialization in `GameState.h` with `#include <functional>` |
| `rand() % 8` magic number | Replaced with `STAFF_NAME_COUNT` constant |
| `ClientManager` missing `#include <algorithm>` | Include added |

> ✅ **Zero known compilation blockers** on GCC 11+ / Clang 14+ / Ubuntu 22.04+

---

## 🎯 Roadmap

### v1.0 — Core (Complete ✅)
- [x] Project structure & CMake setup
- [x] GameState core data model
- [x] Campaign Engine math (Reach, CTR, Conv, Revenue)
- [x] Dashboard UI with revenue chart
- [x] Client Manager (pitch + satisfaction system)
- [x] Staff hiring system (6 roles, skill bonuses)
- [x] AI Competitor system (3 agencies, poaching logic)
- [x] Event System (20 market events, per-channel modifiers)
- [x] Market Map visualization
- [x] Newsfeed & event history
- [x] Save/Load JSON system
- [x] Win/Lose conditions

### v1.1 — Polish (Next)
- [ ] Custom fonts (load .ttf from `assets/fonts/`)
- [ ] Animated revenue numbers (lerp)
- [ ] Campaign performance notifications (popup toasts)
- [ ] More clients unlocked progressively (month gating)
- [ ] Difficulty settings (Easy / Normal / Hard)

### v1.2 — Depth
- [ ] Achievements system (15 achievements)
- [ ] Staff leveling (skill grows with months hired)
- [ ] Campaign templates (quick-launch presets)
- [ ] Seasonal events (Q4 holiday boost, summer slump)
- [ ] Client industry-specific channel bonuses

### v2.0 — Extended
- [ ] Sound effects (OpenAL integration)
- [ ] Multiple save slots
- [ ] Leaderboard (local high scores)
- [ ] Moddable events via JSON
- [ ] Steam integration

---

## 📊 Campaign Math

The simulation engine uses this formula per month:

```
Reach           = Budget × ReachPerDollar[channel] × ChannelMod × StaffBonus
CTR             = BaseCTR[channel] × ChannelMod × QualityModifier
Conversions     = Reach × CTR × BaseConversion[channel]
ClientRevenue   = Conversions × AverageOrderValue
AgencyFee       = ClientRevenue × 15%
```

Channel modifiers are multiplied by active market events each month.

---

## 📄 License

MIT © 2026 [George Pricop](https://github.com/Gzeu)
