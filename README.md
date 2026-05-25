# 🎮 AdEmpire — Marketing Tycoon

> Build and grow a digital marketing agency from scratch. Manage clients, launch campaigns, negotiate contracts, unlock specializations, complete quarterly goals, and become the #1 agency in the market.

![C++](https://img.shields.io/badge/C++-17-blue.svg)
![ImGui](https://img.shields.io/badge/Dear%20ImGui-latest-green.svg)
![OpenGL](https://img.shields.io/badge/OpenGL-3.3-orange.svg)
![CMake](https://img.shields.io/badge/CMake-3.16+-red.svg)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows%20%7C%20macOS-lightgrey.svg)
![License](https://img.shields.io/badge/license-MIT-purple.svg)
![Build](https://img.shields.io/badge/build-passing-brightgreen.svg)
![Version](https://img.shields.io/badge/version-v0.4-blue.svg)

---

## 🕹️ Gameplay

You start with **$12,500** (Normal) and zero clients. Every month you:
- **Pitch and negotiate** with potential clients using live fit score, mood bar, and 6 action choices
- **Launch campaigns** on 6 channels: Social, SEO, Email, Influencer, PR, Paid Search
- **Hire and level staff** with role-based bonuses that improve campaign quality scores
- **React to 30 market events** from the moddable JSON event library
- **Unlock 8 agency specializations** for industry-specific revenue multipliers
- **Complete quarterly goals** for cash rewards and reputation boosts
- **Track performance** in the Analytics panel: ROI per channel, win rates, market trend charts
- **Compete with 3 AI agencies** that grow, adapt, and try to poach dissatisfied clients

**Win:** Reach **35% market share** | **Lose:** Budget < **-$50,000**  
**Difficulty:** Easy ($20K) / Normal ($12.5K) / Hard ($5K + max AI aggression)

---

## 📊 Version History

| Version | Commit | What changed |
|---|---|---|
| v0.1 Core | initial | Game loop, all base systems, 6 channels, 30 clients, save/load |
| Audit | [b9ca0247](https://github.com/Gzeu/AdEmpire/commit/b9ca0247d99b072228a43cb0214672746fe69997) | 4 compilation fixes, zero blockers on GCC 11+ |
| v0.2 | [14113c8e](https://github.com/Gzeu/AdEmpire/commit/14113c8eb3b2e2f699e5ae3752c12c8a5416feb7) | Negotiation, Fit Score, Quarterly Goals, Specializations |
| v0.3 | [aa6004fe](https://github.com/Gzeu/AdEmpire/commit/aa6004fef62d05827fdfef34499a98f3adc84f62) | Toast system, Monthly Report Panel, UIStyle, Difficulty, Economy rebalance |
| **v0.4** | **current** | **StatsTracker, Analytics panel, 30-event JSON, Tooltips library** |

---

## ✅ Full Feature Matrix

### Core systems
| System | Description | Status |
|---|---|---|
| Campaign Engine | Reach × CTR × Conv × AOV → Revenue (18% agency fee) | ✅ |
| AI Competitors | MediaBlaze / PeakBrands / NicheNation + poaching | ✅ |
| Event System | 20 hardcoded + 30 JSON-moddable events, per-channel modifiers | ✅ |
| Client Manager | 30 clients, satisfaction bars, contract renewal | ✅ |
| Staff System | 6 roles, skill bonuses, hire from panel | ✅ |
| Dashboard | 4 KPI cards, 24-month revenue sparkline | ✅ |
| Market Map | Market share bars vs. AI rivals | ✅ |
| Save System | JSON save/load | ✅ |
| Newsfeed | Active event log + history | ✅ |

### Progression (v0.2)
| System | Description | Status |
|---|---|---|
| Negotiation | Fit score, mood, 6 actions, probabilistic resolution | ✅ |
| Quarterly Goals | Auto-generated goals, monthly tracking, cash rewards | ✅ |
| Specialization | 8 unlockable types, industry multipliers | ✅ |
| Capacity Tracking | Capacity score component in FitScoreSystem | ✅ |

### Polish (v0.3)
| System | Description | Status |
|---|---|---|
| Toast Notifications | Success/Error/Warning/Info, fade-out, max 6 | ✅ |
| Monthly Report Panel | KPI cards, revenue by channel, market progress | ✅ |
| UIStyle.h | Centralized colors, KPI cards, section headers | ✅ |
| Difficulty System | Easy/Normal/Hard with budget + AI + revenue mods | ✅ |
| Economy Rebalance | 18% fee, scalable overhead, realistic satisfaction decay | ✅ |

### Analytics & Data (v0.4 — this release)
| System | Description | Status |
|---|---|---|
| StatsTracker | Per-channel revenue, ROI, reach; per-industry win rate, satisfaction | ✅ |
| Analytics Panel | 3 tabs: Revenue by Channel, Industry Win Rate, Market Trend | ✅ |
| 30-Event JSON Library | `assets/data/events.json` — fully moddable, schema documented | ✅ |
| Tooltips.h | 7 tooltip types: Channel, FitScore, Satisfaction, MarketShare, ROI, StaffRole, Budget | ✅ |

---

## 📦 Tech Stack

| Library | Purpose |
|---|---|
| [Dear ImGui](https://github.com/ocornut/imgui) | Full UI layer (git submodule) |
| GLFW 3.3+ | Window creation and input |
| OpenGL 3.3 core | Rendering backend |
| nlohmann/json | Save system and data files (auto-downloaded) |
| CMake 3.16+ | Build system |

---

## 🚀 Build & Run

### Ubuntu / Debian
```bash
sudo apt update
sudo apt install cmake build-essential libglfw3-dev libgl1-mesa-dev libglu1-mesa-dev
git clone --recurse-submodules https://github.com/Gzeu/AdEmpire.git
cd AdEmpire && mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
./AdEmpire
```

### Submodule missing?
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
│   ├── main.cpp
│   ├── core/
│   │   ├── GameState.h          # All structs + std::hash<ChannelType>
│   │   ├── Simulation.cpp       # Monthly economic simulation
│   │   └── Difficulty.h         # Easy / Normal / Hard presets
│   ├── systems/
│   │   ├── CampaignEngine.cpp
│   │   ├── AICompetitor.cpp     # Poach toast notifications
│   │   ├── EventSystem.cpp      # 20 hardcoded events
│   │   ├── SaveSystem.cpp
│   │   ├── FitScoreSystem.cpp
│   │   ├── GoalSystem.cpp
│   │   ├── SpecializationSystem.cpp
│   │   ├── ToastSystem.h/.cpp   # v0.3
│   │   └── StatsTracker.h/.cpp  # v0.4 ← NEW
│   └── ui/
│       ├── Theme.h
│       ├── UIStyle.h            # v0.3 — centralized colors + components
│       ├── Tooltips.h           # v0.4 ← NEW — 7 tooltip types
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
│       ├── ReportPanel.cpp      # v0.3
│       └── StatsPanel.cpp       # v0.4 ← NEW — 3-tab analytics
├── assets/
│   └── data/
│       ├── clients.json         # 30 clients
│       ├── channels.json        # 6 channels with base stats
│       └── events.json          # v0.4 ← 30 moddable events
├── lib/
│   └── imgui/                 # git submodule
├── CMakeLists.txt
├── README.md
└── SETUP.md
```

---

## 🔍 Tooltip Coverage (v0.4)

All interactive elements now have `Tooltips.h` coverage:

| Element | Tooltip Content |
|---|---|
| Channel name | Best-for industries, Reach/$, CTR, event vulnerability |
| Fit Score value | 4-component breakdown + strong/moderate/weak assessment |
| Client satisfaction bar | Poach risk thresholds, how to improve |
| Market share % | Growth drivers, win condition reminder |
| ROI value | Formula, grade scale (>50% excellent) |
| Staff role | Role effect on campaigns, salary shown |
| Budget figure | Bankruptcy threshold, revenue flow explanation |

---

## 📊 Simulation Model

```text
Reach         = Budget × ReachPerDollar[ch] × ChannelEventMod × StaffBonus
CTR           = BaseCTR[ch] × QualityModifier(qualityScore)
Conversions   = Reach × CTR × BaseConversion[ch]
ClientRevenue = Conversions × (clientBudget × 0.12)
AgencyFee     = ClientRevenue × 18%
Overhead      = $800 base + $280 × max(0, staffCount - 3)
```

Channel modifiers stack: **market events** × **seasonal events** × **specialization bonuses** × **staff skill**.

---

## 🎨 Moddable Events

Edit `assets/data/events.json` to add your own events at runtime:

```json
{
  "id": "EVT_CUSTOM",
  "title": "My Custom Event",
  "description": "What happens and why.",
  "impact": "Social +50%",
  "modifiers": { "Social": 1.5, "SEO": 1.0, "Email": 1.0, "Influencer": 1.0, "PR": 1.0, "PaidSearch": 1.0 },
  "budgetImpact": 0,
  "durationMonths": 2,
  "probability": 0.07
}
```

Probability is per-month trigger chance (0.0–1.0). The game loads this file at startup automatically.

---

## 🔧 Build Audit

| Issue | Fix |
|---|---|
| `nlohmann/json` missing | Auto-downloaded via `file(DOWNLOAD)` in CMakeLists |
| `std::hash<ChannelType>` | Template specialization in `GameState.h` |
| Hardcoded staff modulo | Safe `STAFF_NAME_COUNT` constant |
| Missing `<algorithm>` | Added to ClientManager |

> ✅ Zero known compilation blockers — GCC 11+ / Clang 14+ / Ubuntu 22.04+

---

## 🔜 What’s next (v0.5)

| Feature | Description |
|---|---|
| Staff leveling | Skill grows +0.8%/month, salary bumps at level thresholds |
| Campaign templates | 15 quick-launch presets sorted by client industry |
| Achievements | 15 milestones with auto-toast on unlock |
| Season bonuses | Nov +60% (Black Friday), Dec +80%, Jul −20% |
| 3 save slots | Named slots with agency snapshot and date |

---

## 📄 License

MIT © 2026 [George Pricop](https://github.com/Gzeu)
