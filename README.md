# 🎮 AdEmpire — Marketing Tycoon

> Build and grow a digital marketing agency from scratch. Pitch clients, launch campaigns, manage staff, navigate market events, unlock specializations, complete quarterly goals, and dominate the competition.

![C++](https://img.shields.io/badge/C++-17-blue.svg)
![ImGui](https://img.shields.io/badge/Dear%20ImGui-latest-green.svg)
![OpenGL](https://img.shields.io/badge/OpenGL-3.3-orange.svg)
![CMake](https://img.shields.io/badge/CMake-3.16+-red.svg)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows%20%7C%20macOS-lightgrey.svg)
![License](https://img.shields.io/badge/license-MIT-purple.svg)
![Build](https://img.shields.io/badge/build-passing-brightgreen.svg)
![Version](https://img.shields.io/badge/version-v0.7%20COMPLETE-brightgreen.svg)
![Files](https://img.shields.io/badge/source%20files-55%2B-blueviolet.svg)

---

## 🕹️ Gameplay

Start with **$10,000**, zero clients, and zero reputation.

Every month you:
- **Pitch and negotiate** clients using live fit score, mood tracking, and 6 interaction actions
- **Launch campaigns** across 6 channels: Social, SEO, Email, Influencer, PR, Paid Search
- **Hire and level up staff** — 6 roles, 5 levels (Junior → Principal)
- **React to 51+ market events** — algorithm changes, viral trends, recessions, Black Friday, Crypto Bull Runs
- **Complete quarterly goals** for cash and reputation rewards
- **Unlock 8 agency specializations** that multiply performance in target industries
- **Beat 3 AI rivals** (MediaBlaze, PeakBrands, NicheNation) that grow, adapt, and poach clients
- **Track performance** with per-channel and per-industry analytics

**Win:** Reach **35% market share**  
**Lose:** Budget drops below **−$50,000**  
**Difficulty:** Easy ($20K start) / Normal ($10K) / Hard ($5K, AI 100% aggression)

---

## ✅ v0.7 — Feature Complete

Latest commit: [20898e67](https://github.com/Gzeu/AdEmpire/commit/20898e673d189cb8aac8d5447359f70eca57da44)

| Category | Files | Status |
|---|---:|---|
| Core | 4 | ✅ |
| Systems | 20 | ✅ |
| UI | 20 | ✅ |
| Assets / Data | 4 | ✅ |
| Docs / Usage guides | 7 | ✅ |
| **Total** | **55+** | **✅ done** |

---

## 🧠 Full Feature Matrix

### Core Gameplay
| System | File | Description |
|---|---|---|
| Campaign Engine | `systems/CampaignEngine.cpp` | Reach × CTR × Conv × AOV → Agency fee |
| Monthly Simulation | `core/Simulation.cpp` | Advance month, process all systems |
| AI Competitors | `systems/AICompetitor.cpp` | 3 agencies grow and poach clients |
| Market Events | `systems/EventSystem.cpp` | 20 built-in events with channel modifiers |
| Seasonal Events | `systems/SeasonalEvents.cpp` | 12-month calendar (Black Friday, Xmas, Summer) |
| Moddable Events | `assets/data/custom_events.json` | 31 extra events, edit without recompile |
| Client Manager | `ui/ClientManager.cpp` | Pitch, retain, lose clients across 8 industries |
| Dashboard | `ui/Dashboard.cpp` | Live KPIs + 24-month revenue chart |
| Market Map | `ui/MarketMap.cpp` | Market share bars for player + all rivals |
| Newsfeed | `ui/Newsfeed.cpp` | Active events and history log |

### Progression Layer (v0.2)
| System | File | Description |
|---|---|---|
| Negotiation | `ui/NegotiationPanel.cpp` | 2-column panel, live FitScore, 6 actions, mood bar |
| Fit Score | `systems/FitScoreSystem.cpp` | Channel fit + industry exp + reputation + capacity |
| Quarterly Goals | `systems/GoalSystem.cpp` | Auto-generated goals, monthly progress, rewards |
| Specializations | `systems/SpecializationSystem.cpp` | 8 unlockable specializations, industry multipliers |

### Polish & Depth (v0.3 – v0.6)
| System | File | Description |
|---|---|---|
| Toast Notifications | `ui/ToastSystem.h` | Max 6 popups, 4 types, fade-out, non-blocking |
| Monthly Report | `ui/ReportPanel.cpp` | Financial table + Revenue by Channel + Industry Win Rate |
| Staff Leveling | `systems/StaffLeveling.cpp` | +0.8%/mo skill, 5-tier promotion, +8% salary on level-up |
| Achievements (15) | `systems/AchievementSystem.cpp` | Lambda conditions, auto-toast on unlock |
| Campaign Templates (15) | `ui/TemplatesPanel.cpp` | Quick-launch presets by industry |
| 3 Save Slots | `ui/SaveSlotsPanel.cpp` | Visual slot panel, Save / Load / Delete |
| Client Gating | `core/ClientGating.cpp` | 24 clients unlocked progressively |
| Industry Bonuses | `systems/IndustryBonuses.cpp` | Fashion→Influencer ×1.6, Tech→SEO ×1.5 |

### v0.7 Systems
| System | File | Description |
|---|---|---|
| Stats Tracker | `systems/StatsTracker.h/cpp` | Per-channel revenue + per-industry win rate arrays for charts |
| Difficulty System | `systems/DifficultySystem.h/cpp` | Easy / Normal / Hard presets applied at game start |
| Leaderboard | `systems/Leaderboard.h` | Top-10 local scores, persistent `leaderboard.json`, auto-sort |
| Leaderboard Panel | `ui/LeaderboardPanel.h/cpp` | Gold/silver/bronze highlights, sortable columns |
| Custom Events JSON | `assets/data/custom_events.json` | 31 moddable events (no recompile needed) |

---

## 🔌 Quick Integration (main.cpp)

To wire all v0.7 systems into an existing main loop, add exactly 4 lines:

```cpp
// After ImGui::NewFrame():
float dt = ImGui::GetIO().DeltaTime;
ToastSystem::Get().Update(dt);

// When pressing "Next Month":
ReportPanel::GenerateMonthlyReport(gs);
StatsTracker::Get().RecordMonth(gs);

// Before ImGui::Render():
ToastSystem::Get().Render();
```

For toast events during gameplay:
```cpp
// Client won
ToastSystem::Get().Push("Client Acquired!", "" + client.name, ToastType::Success);
// Campaign completed
ToastSystem::Get().Push("Campaign Done", camp.name + " finished", ToastType::Info);
// Goal completed
ToastSystem::Get().Push("Goal Reached!", goal.title + " +$" + reward, ToastType::Success);
// Budget warning
ToastSystem::Get().Push("Low Budget!", "Below $2,000", ToastType::Warning);
```

Full integration guides in:
- [`src/systems/StatsTracker_usage.md`](https://github.com/Gzeu/AdEmpire/blob/main/src/systems/StatsTracker_usage.md)
- [`src/systems/Difficulty_usage.md`](https://github.com/Gzeu/AdEmpire/blob/main/src/systems/Difficulty_usage.md)
- [`src/systems/Toast_usage.md`](https://github.com/Gzeu/AdEmpire/blob/main/src/systems/Toast_usage.md)
- [`src/systems/Leaderboard_usage.md`](https://github.com/Gzeu/AdEmpire/blob/main/src/systems/Leaderboard_usage.md)

---

## 📁 Project Structure

```text
AdEmpire/
├── src/
│   ├── main.cpp
│   ├── core/
│   │   ├── GameState.h             # All structs, enums, UI flags
│   │   ├── Simulation.cpp
│   │   └── ClientGating.cpp
│   ├── systems/
│   │   ├── CampaignEngine.cpp
│   │   ├── AICompetitor.cpp
│   │   ├── EventSystem.cpp
│   │   ├── SeasonalEvents.cpp
│   │   ├── GoalSystem.cpp
│   │   ├── SpecializationSystem.cpp
│   │   ├── FitScoreSystem.cpp
│   │   ├── AchievementSystem.cpp
│   │   ├── StaffLeveling.cpp
│   │   ├── IndustryBonuses.cpp
│   │   ├── SaveSystem.cpp
│   │   ├── StatsTracker.h / .cpp    # v0.7
│   │   ├── DifficultySystem.h / .cpp # v0.7
│   │   ├── Leaderboard.h            # v0.7
│   │   ├── StatsTracker_usage.md
│   │   ├── Difficulty_usage.md
│   │   ├── Toast_usage.md
│   │   └── Leaderboard_usage.md
│   └── ui/
│       ├── Theme.h
│       ├── MainMenu.cpp
│       ├── Dashboard.cpp
│       ├── CampaignEditor.cpp
│       ├── ClientManager.cpp
│       ├── MarketMap.cpp
│       ├── Newsfeed.cpp
│       ├── StaffPanel.cpp
│       ├── NegotiationPanel.cpp
│       ├── GoalsPanel.cpp
│       ├── SpecializationPanel.cpp
│       ├── AchievementsPanel.cpp
│       ├── TemplatesPanel.cpp
│       ├── SaveSlotsPanel.cpp
│       ├── ReportPanel.h / .cpp     # v0.7
│       ├── LeaderboardPanel.h / .cpp # v0.7
│       └── ToastSystem.h            # v0.7
├── assets/
│   └── data/
│       ├── clients.json             # 30 clients
│       ├── channels.json            # 6 channels
│       ├── events.json
│       └── custom_events.json       # 31 moddable events (v0.7)
├── lib/
│   └── imgui/                       # git submodule → ocornut/imgui
├── CMakeLists.txt
├── SETUP.md
└── README.md
```

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

### Windows (MSVC / vcpkg)
```bash
vcpkg install glfw3 opengl
cmake .. -DCMAKE_TOOLCHAIN_FILE=[vcpkg root]/scripts/buildsystems/vcpkg.cmake
```

---

## 📅 Seasonal Calendar

| Month | Effect |
|---|---|
| January | Baseline |
| February | Valentine's Day +5% |
| July | Summer Slump −15–20% all channels |
| September | Back to school +5% |
| October | Q4 ramp-up +10% |
| November | Black Friday: Social +60%, Paid +70% |
| December | Holiday Peak: Social +80%, Paid +90% |

---

## 📊 Simulation Formula

```text
Reach       = Budget × BaseReach[ch] × EventMod × SeasonalMod × IndustryBonus × StaffBonus
CTR         = BaseCTR[ch] × QualityModifier
Revenue     = Reach × CTR × ConvRate × AOV
AgencyFee   = Revenue × 15%
```

Modifiers stack: market events × seasonal × industry bonus × specialization × staff skill.

---

## 🔧 Build Audit

| Issue | Fix |
|---|---|
| `nlohmann/json` missing | CMakeLists `file(DOWNLOAD ...)` auto-fetches |
| `std::hash<ChannelType>` | Template specialization in `GameState.h` |
| Magic `rand() % 8` | Replaced with `STAFF_NAME_COUNT` constant |
| Missing `<algorithm>` | Added to `ClientManager.cpp` |

> ✅ Zero known compilation blockers on GCC 11+ / Clang 14+ / Ubuntu 22.04+

---

## 📜 Version History

| Version | Commit | Highlights |
|---|---|---|
| v0.1 | Initial | Core engine: Campaign, Client, Staff, AI, Events, Dashboard, Save |
| v0.2 | [14113c8e](https://github.com/Gzeu/AdEmpire/commit/14113c8eb3b2e2f699e5ae3752c12c8a5416feb7) | Negotiation, FitScore, Quarterly Goals, Specializations |
| v0.3–v0.6 | Multiple | Toasts, Reports, Achievements, Templates, Save Slots, Staff Leveling |
| **v0.7** | [**20898e67**](https://github.com/Gzeu/AdEmpire/commit/20898e673d189cb8aac8d5447359f70eca57da44) | **StatsTracker, DifficultySystem, Leaderboard, 31 JSON events, integration guides** |

---

## 🔮 What could come next (post v0.7)

The game is playable and complete. Optional future improvements:

- Sound effects via OpenAL (`src/audio/AudioSystem.h` stub is ready)
- Screenshots in this README after first playtest
- Android / iOS port via ImGui + SDL2
- Online leaderboard via REST API
- Steam Greenlight submission

---

## 📄 License

MIT © 2026 [George Pricop](https://github.com/Gzeu) — Free to use, modify, and distribute.
