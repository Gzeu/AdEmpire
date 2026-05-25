# 🎮 AdEmpire — Marketing Tycoon

> Build and grow a digital marketing agency from scratch. Manage clients, launch campaigns, negotiate contracts, unlock specializations, complete quarterly goals, and become the #1 agency in the market.

![C++](https://img.shields.io/badge/C++-17-blue.svg)
![ImGui](https://img.shields.io/badge/Dear%20ImGui-latest-green.svg)
![OpenGL](https://img.shields.io/badge/OpenGL-3.3-orange.svg)
![CMake](https://img.shields.io/badge/CMake-3.16+-red.svg)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows%20%7C%20macOS-lightgrey.svg)
![License](https://img.shields.io/badge/license-MIT-purple.svg)
![Build](https://img.shields.io/badge/build-passing-brightgreen.svg)
![Version](https://img.shields.io/badge/version-v0.2%20integration-blue.svg)

---

## 🕹️ Gameplay

You start with **$10,000** and zero clients. Every month you:
- **Pitch and negotiate** with potential clients using live fit score, mood, and action choices.
- **Launch campaigns** on 6 channels: Social, SEO, Email, Influencer, PR, and Paid Search.
- **Hire staff** and improve execution quality through role-based bonuses.
- **React to market events** like algorithm changes, viral trends, recessions, and PR crises.
- **Unlock agency specializations** that increase your strength in specific industries.
- **Complete quarterly goals** for extra rewards and long-term progression.
- **Compete with AI agencies** that grow, adapt, and try to poach your clients.

**Win condition:** Reach **35% market share**  
**Lose condition:** Budget drops below **-$50,000**

---

## ✅ v0.2 Integration Complete

Commit: [14113c8e](https://github.com/Gzeu/AdEmpire/commit/14113c8eb3b2e2f699e5ae3752c12c8a5416feb7)

### Newly integrated systems

| Area | File | What it adds |
|---|---|---|
| Main integration | `src/main.cpp` | New includes, init hooks, monthly update hooks, 3 new panels, navbar buttons |
| Core state | `src/core/GameState.h` | `FitScore`, `CapacityInfo`, `QuarterlyGoal`, `NegotiationState`, `Specialization`, enums, UI flags |
| Fit score | `src/systems/FitScoreSystem.cpp` | Channel fit, industry experience, reputation fit, capacity fit |
| Goals | `src/systems/GoalSystem.cpp` | Quarterly goal init, monthly progress, completion rewards, goal rollover |
| Specializations | `src/systems/SpecializationSystem.cpp` | 8 unlockable specializations with industry multipliers |
| Negotiation UI | `src/ui/NegotiationPanel.cpp` | Two-column panel, live fit score, mood bar, 6 interaction actions |
| Goals UI | `src/ui/GoalsPanel.cpp` | Quarterly progress bars, reward view, completed/failed states |
| Specialization UI | `src/ui/SpecializationPanel.cpp` | Unlock grid, status icons, specialization progress |

### Repo status after v0.2

| Category | Count | Status |
|---|---:|---|
| Core | 4 files | ✅ complete |
| Systems | 16 files | ✅ complete |
| UI | 18 files | ✅ complete |
| Assets | 3 files | ✅ complete |
| Build/docs | 3 files | ✅ complete |

---

## 🧠 Feature Matrix

### Core gameplay

| System | Description | Status |
|---|---|---|
| Campaign Engine | Reach × CTR × Conv × AOV → Revenue math | ✅ |
| AI Competitors | 3 agencies with growth + poaching logic | ✅ |
| Event System | Market events with channel modifiers | ✅ |
| Client Manager | Acquire, retain, and lose clients | ✅ |
| Staff System | 6 staff roles with skill-based bonuses | ✅ |
| Dashboard | Live KPIs + revenue chart | ✅ |
| Market Map | Market share visualization | ✅ |
| Save System | JSON save/load flow | ✅ |
| Newsfeed | Event history and active modifiers | ✅ |

### v0.2 progression layer

| System | Description | Status |
|---|---|---|
| Negotiation System | Live fit scoring and action-based pitch resolution | ✅ |
| Quarterly Goals | Goal generation, monthly tracking, rewards | ✅ |
| Specialization System | Industry specialization unlocks and bonuses | ✅ |
| Capacity Tracking | Agency capacity evaluation inside fit score | ✅ |

---

## 📦 Tech Stack

| Library | Purpose |
|---|---|
| [Dear ImGui](https://github.com/ocornut/imgui) | Full UI layer |
| GLFW 3 | Windowing and input |
| OpenGL 3.3 | Rendering backend |
| nlohmann/json | Save system and data serialization |
| CMake | Cross-platform build |

---

## 🚀 Build & Run

### Ubuntu / Debian
```bash
sudo apt update
sudo apt install cmake build-essential libglfw3-dev libgl1-mesa-dev libglu1-mesa-dev
git clone --recurse-submodules https://github.com/Gzeu/AdEmpire.git
cd AdEmpire
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
./AdEmpire
```

### If ImGui submodule is missing
```bash
git submodule update --init --recursive
```

### Windows (MSVC / vcpkg)
```bash
vcpkg install glfw3 opengl
cmake .. -DCMAKE_TOOLCHAIN_FILE=[vcpkg root]/scripts/buildsystems/vcpkg.cmake
```

---

## 📁 Project Structure

```text
AdEmpire/
├── src/
│   ├── main.cpp
│   ├── core/
│   │   ├── GameState.h
│   │   ├── GameState.cpp
│   │   └── Simulation.cpp
│   ├── systems/
│   │   ├── CampaignEngine.cpp
│   │   ├── AICompetitor.cpp
│   │   ├── EventSystem.cpp
│   │   ├── SaveSystem.cpp
│   │   ├── FitScoreSystem.cpp
│   │   ├── GoalSystem.cpp
│   │   └── SpecializationSystem.cpp
│   └── ui/
│       ├── MainMenu.cpp
│       ├── Dashboard.cpp
│       ├── CampaignEditor.cpp
│       ├── ClientManager.cpp
│       ├── MarketMap.cpp
│       ├── Newsfeed.cpp
│       ├── StaffPanel.cpp
│       ├── NegotiationPanel.cpp
│       ├── GoalsPanel.cpp
│       └── SpecializationPanel.cpp
├── assets/
│   └── data/
├── lib/
│   └── imgui/
├── CMakeLists.txt
├── README.md
└── SETUP.md
```

---

## 🔧 Build Audit

The known compilation fixes introduced earlier remain important and are preserved:

| Issue | Fix |
|---|---|
| `nlohmann/json` missing | Auto-download in `CMakeLists.txt` |
| `std::hash<ChannelType>` missing | Added in `GameState.h` |
| Hardcoded staff name modulo | Replaced with safe constant |
| Missing `<algorithm>` include | Added to `ClientManager` |

> ✅ Zero known compilation blockers on GCC 11+ / Clang 14+ / Ubuntu 22.04+

---

## 📊 Simulation Model

The monthly simulation still follows the same core loop:

```text
Reach         = Budget × ReachPerDollar × ChannelModifiers × StaffBonus
CTR           = BaseCTR × QualityModifier
Conversions   = Reach × CTR × ConversionRate
ClientRevenue = Conversions × AverageOrderValue
AgencyFee     = ClientRevenue × 15%
```

v0.2 adds decision layers around that loop: **fit score**, **negotiation quality**, **capacity**, **specialization bonuses**, and **quarterly rewards**.

---

## 🔜 What remains for v0.3

These are the most useful next systems to add now:

| Planned file | Purpose |
|---|---|
| `src/systems/ToastSystem.cpp` | Popup notifications for wins, losses, goals, and campaign completion |
| `src/systems/StatsTracker.cpp` | Advanced charts: revenue by channel, industry win rate, retention trends |
| `src/ui/ReportPanel.cpp` | Monthly summary report with campaign breakdowns |
| `assets/data/events.json` | Expand to 30+ moddable events from JSON instead of hardcoded only |
| `src/systems/DifficultySystem.cpp` | Easy / Normal / Hard global modifiers |

### Recommended next order
1. `ToastSystem`
2. `ReportPanel`
3. `StatsTracker`
4. `DifficultySystem`
5. JSON-driven event expansion

---

## 📄 License

MIT © 2026 [George Pricop](https://github.com/Gzeu)
