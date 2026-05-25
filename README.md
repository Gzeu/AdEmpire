# 🎮 AdEmpire — Marketing Tycoon

> Build and grow a digital marketing agency from scratch. Pitch clients, run campaigns, manage staff, navigate market events, and dominate the competition.

![C++](https://img.shields.io/badge/C++-17-blue.svg)
![ImGui](https://img.shields.io/badge/Dear%20ImGui-latest-green.svg)
![OpenGL](https://img.shields.io/badge/OpenGL-3.3-orange.svg)
![CMake](https://img.shields.io/badge/CMake-3.16+-red.svg)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows%20%7C%20macOS-lightgrey.svg)
![License](https://img.shields.io/badge/license-MIT-purple.svg)
![Build](https://img.shields.io/badge/build-passing-brightgreen.svg)
![Version](https://img.shields.io/badge/version-v0.6-blue.svg)

---

## 🕹️ Gameplay

Start with **$10,000**, zero clients, and zero reputation.  
Every month you pitch, launch campaigns, manage staff, and react to market events.

**Win:** Reach **35% market share**  
**Lose:** Budget drops below **-$50,000**

---

## ✅ Version History

| Version | What was added |
|---|---|
| v0.1 | Core engine: Campaign, Client, Staff, AI, Events, Dashboard, Save |
| v0.2 | Negotiation system, FitScore, Quarterly Goals, Specializations |
| v0.3 | Toast notifications, Report panel, UIStyle helpers |
| v0.4 | Difficulty system, Client gating, Staff leveling, Seasonal events |
| v0.5 | Achievements (15), Campaign Templates (15), 3 Save Slots, Leaderboard stubs |
| **v0.6** | **Full integration of v0.5 systems into `main.cpp` + `GameState.h` flags** |

---

## 🧠 Feature Matrix

| System | File | Status |
|---|---|---|
| Campaign Engine | `systems/CampaignEngine.cpp` | ✅ |
| AI Competitors | `systems/AICompetitor.cpp` | ✅ |
| Event System | `systems/EventSystem.cpp` | ✅ |
| Seasonal Events | `systems/SeasonalEvents.cpp` | ✅ |
| Client Manager | `ui/ClientManager.cpp` | ✅ |
| Staff + Leveling | `systems/StaffLeveling.cpp` | ✅ |
| Negotiation | `ui/NegotiationPanel.cpp` | ✅ |
| Quarterly Goals | `systems/GoalSystem.cpp` | ✅ |
| Specializations | `systems/SpecializationSystem.cpp` | ✅ |
| Achievements (15) | `systems/AchievementSystem.cpp` | ✅ |
| Campaign Templates (15) | `ui/TemplatesPanel.cpp` | ✅ |
| 3 Save Slots | `ui/SaveSlotsPanel.cpp` | ✅ |
| Dashboard + Chart | `ui/Dashboard.cpp` | ✅ |
| Market Map | `ui/MarketMap.cpp` | ✅ |
| Toast Notifications | `systems/ToastSystem.h` | ✅ |
| Monthly Report | `ui/ReportPanel.cpp` | ✅ |
| Fit Score | `systems/FitScoreSystem.cpp` | ✅ |
| Save System | `systems/SaveSystem.cpp` | ✅ |

---

## 📅 Seasonal Calendar

| Month | Effect |
|---|---|
| January | Steady baseline |
| February | Valentine +5% |
| July | Summer Slump − 15–20% all channels |
| September | Back-to-school +5% |
| October | Q4 ramp-up +10% |
| November | Black Friday: Social +60%, Paid +70% |
| December | Holiday Peak: Social +80%, Paid +90% |

---

## 📦 Tech Stack

| Library | Purpose |
|---|---|
| [Dear ImGui](https://github.com/ocornut/imgui) | All UI |
| GLFW 3 | Window & input |
| OpenGL 3.3 | Rendering backend |
| nlohmann/json | Save system (auto-downloaded by CMake) |
| CMake 3.16+ | Build system |

---

## 🚀 Build & Run

### Ubuntu / Debian
```bash
sudo apt install cmake build-essential libglfw3-dev libgl1-mesa-dev
git clone --recurse-submodules https://github.com/Gzeu/AdEmpire.git
cd AdEmpire && mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
./AdEmpire
```

### If ImGui submodule is empty
```bash
git submodule update --init --recursive
```

### Windows (vcpkg)
```bash
vcpkg install glfw3 opengl
cmake .. -DCMAKE_TOOLCHAIN_FILE=[vcpkg]/scripts/buildsystems/vcpkg.cmake
```

---

## 📁 Project Structure

```text
AdEmpire/
├── src/
│   ├── main.cpp                    # v0.6: all systems wired
│   ├── core/
│   │   ├── GameState.h             # v0.6: showTemplates, showSaveSlots, Achievement struct
│   │   └── Simulation.cpp
│   ├── systems/
│   │   ├── AchievementSystem.cpp   # 15 achievements with lambda conditions
│   │   ├── StaffLeveling.cpp       # +0.8%/mo skill, 5-tier promotion
│   │   ├── SeasonalEvents.cpp      # 12-month seasonal multipliers
│   │   ├── CampaignEngine.cpp
│   │   ├── AICompetitor.cpp
│   │   ├── EventSystem.cpp
│   │   ├── GoalSystem.cpp
│   │   ├── SpecializationSystem.cpp
│   │   ├── FitScoreSystem.cpp
│   │   └── SaveSystem.cpp
│   └── ui/
│       ├── AchievementsPanel.cpp   # 2-col grid, progress bar
│       ├── TemplatesPanel.cpp      # 15 templates, launch to client
│       ├── SaveSlotsPanel.cpp      # 3 slots: Save/Load/Delete
│       ├── NegotiationPanel.cpp
│       ├── GoalsPanel.cpp
│       ├── SpecializationPanel.cpp
│       ├── Dashboard.cpp
│       ├── CampaignEditor.cpp
│       ├── ClientManager.cpp
│       ├── MarketMap.cpp
│       ├── Newsfeed.cpp
│       ├── StaffPanel.cpp
│       └── ReportPanel.cpp
├── assets/data/
├── lib/imgui/                      # git submodule
├── CMakeLists.txt
└── README.md
```

---

## 🔧 Build Audit

| Issue | Fix |
|---|---|
| `nlohmann/json` missing | CMakeLists `file(DOWNLOAD ...)` |
| `std::hash<ChannelType>` | Template specialization in `GameState.h` |
| Magic `rand() % 8` | Replaced with `STAFF_NAME_COUNT` constant |
| Missing `<algorithm>` | Added to `ClientManager` |

> ✅ Zero known compilation blockers — GCC 11+ / Clang 14+ / Ubuntu 22.04+

---

## 📊 Simulation Formula

```text
Reach       = Budget × BaseReach[ch] × EventMod × SeasonalMod × StaffBonus
CTR         = BaseCTR[ch] × QualityModifier
Revenue     = Reach × CTR × ConvRate × AOV
AgencyFee   = Revenue × 15%
```

---

## 🛣️ Roadmap v0.7

Next update targets:

- [ ] `StatsTracker.cpp` — per-channel revenue breakdown + win rate charts
- [ ] `DifficultySystem.cpp` — Easy / Normal / Hard global multipliers at game start
- [ ] JSON-driven events — load extra events from `assets/data/custom_events.json`
- [ ] Leaderboard — local top-10 persistent JSON scores
- [ ] Screenshots section in README after first playtest

---

## 📄 License

MIT © 2026 [George Pricop](https://github.com/Gzeu)
