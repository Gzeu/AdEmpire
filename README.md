# 🎮 AdEmpire — Marketing Tycoon

> Build and grow a digital marketing agency from scratch. Negotiate contracts, launch campaigns, unlock specializations, hit quarterly goals, and become the market leader.

![C++](https://img.shields.io/badge/C++-17-blue.svg)
![ImGui](https://img.shields.io/badge/Dear%20ImGui-latest-green.svg)
![OpenGL](https://img.shields.io/badge/OpenGL-3.3-orange.svg)
![CMake](https://img.shields.io/badge/CMake-3.16+-red.svg)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows%20%7C%20macOS-lightgrey.svg)
![License](https://img.shields.io/badge/license-MIT-purple.svg)
![Build](https://img.shields.io/badge/build-passing-brightgreen.svg)
![Version](https://img.shields.io/badge/version-v0.5-blue.svg)

---

## 🕹️ Gameplay

You start with **$10,000** and zero clients. Every month you:
- **Negotiate** — Live fit score, mood bar, and 6 action choices per pitch
- **Launch campaigns** — 6 channels × 15 quick-launch templates with quality bonuses
- **Hire & level up staff** — Skill grows +0.8%/month, salary bumps on level-up
- **React to seasonal events** — July slump −20%, Black Friday +60%, Christmas +80%
- **Unlock specializations** — 8 industry multipliers that stack with channel mods
- **Hit quarterly goals** — Cash rewards + reputation boosts every 3 months
- **Track analytics** — ROI per channel, industry win rates, 36-month market trend
- **Earn achievements** — 15 milestones with auto-toast notifications
- **Beat 3 AI rivals** — MediaBlaze, PeakBrands, NicheNation grow and poach clients

**Win:** 35% market share | **Lose:** Budget < −$50,000

---

## 📊 Version History

| Version | Commit | Highlights |
|---|---|---|
| v0.1 | — | Game loop, 6 channels, 30 clients, campaign engine, AI, events, save |
| Audit | [b9ca0247](https://github.com/Gzeu/AdEmpire/commit/b9ca0247d99b072228a43cb0214672746fe69997) | 4 compilation fixes, zero blockers on GCC 11+ |
| v0.2 | [14113c8e](https://github.com/Gzeu/AdEmpire/commit/14113c8eb3b2e2f699e5ae3752c12c8a5416feb7) | Negotiation, Fit Score, Quarterly Goals, Specializations |
| v0.3 | [aa6004fe](https://github.com/Gzeu/AdEmpire/commit/aa6004fef62d05827fdfef34499a98f3adc84f62) | Toast system, Monthly Report, UIStyle, Difficulty |
| v0.4 | [bc0ac0a2](https://github.com/Gzeu/AdEmpire/commit/bc0ac0a2c21a0f49d51a84bd52d11f1450af5306) | StatsTracker, Analytics panel, 30-event JSON, Tooltips |
| **v0.5** | [6c8220df](https://github.com/Gzeu/AdEmpire/commit/6c8220df108b1218f6f066dbd1e98b0957366fc6) | **Staff leveling, 15 templates, 15 achievements, seasonal bonuses, 3 save slots** |

---

## ✅ Full Feature Matrix

### Core (v0.1)
| System | Description | Status |
|---|---|---|
| Campaign Engine | Reach × CTR × Conv × AOV → Revenue (18% agency fee) | ✅ |
| AI Competitors | MediaBlaze / PeakBrands / NicheNation with poaching | ✅ |
| Event System | 20 hardcoded + 30 JSON-moddable events | ✅ |
| Client Manager | 30 clients, satisfaction, contract renewal | ✅ |
| Staff System | 6 roles with skill bonuses | ✅ |
| Dashboard | KPI cards, 24-month sparkline | ✅ |
| Market Map | Market share bars vs AI rivals | ✅ |
| Save System | JSON save/load | ✅ |
| Newsfeed | Event log + history | ✅ |

### Progression (v0.2)
| System | Description | Status |
|---|---|---|
| Negotiation Panel | Fit score, mood, 6 actions, probabilistic resolve | ✅ |
| Quarterly Goals | Auto-generated, monthly tracking, cash rewards | ✅ |
| Specialization System | 8 unlockable industry specializations | ✅ |
| Capacity Tracking | FitScore capacity component | ✅ |

### Polish (v0.3)
| System | Description | Status |
|---|---|---|
| Toast Notifications | 4 types, fade-out, max 6 simultaneous | ✅ |
| Monthly Report Panel | KPI cards, channel breakdown, market progress | ✅ |
| UIStyle.h | Centralized colors, KPI cards, section headers | ✅ |
| Difficulty System | Easy / Normal / Hard modifiers | ✅ |

### Analytics (v0.4)
| System | Description | Status |
|---|---|---|
| StatsTracker | Per-channel revenue/ROI; per-industry win rate | ✅ |
| Analytics Panel | 3 tabs: Revenue by Channel, Industry Win Rate, Market Trend | ✅ |
| 30-Event JSON Library | Fully moddable `assets/data/events.json` | ✅ |
| Tooltips.h | 7 types: Channel, FitScore, Satisfaction, Market, ROI, Staff, Budget | ✅ |

### Depth (v0.5 — this release)
| System | Description | Status |
|---|---|---|
| Staff Leveling | +0.8% skill/month, 5 levels (Junior→Principal), salary bumps | ✅ |
| 15 Campaign Templates | Quick-launch presets per channel with quality bonus | ✅ |
| 15 Achievements | Lambda conditions, auto-toast, 2-col grid panel | ✅ |
| Seasonal Events | All 12 months modeled — Jul −20%, Nov +60%, Dec +80% | ✅ |
| 3 Save Slots | Named slots with agency snapshot + progress bar | ✅ |

---

## 📦 Tech Stack

| Library | Purpose |
|---|---|
| [Dear ImGui](https://github.com/ocornut/imgui) | Full UI (git submodule) |
| GLFW 3.3+ | Window + input |
| OpenGL 3.3 core | Rendering backend |
| nlohmann/json | Save system + data files (auto-downloaded) |
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

### Submodule empty?
```bash
git submodule update --init --recursive
```

### Windows (vcpkg)
```bash
vcpkg install glfw3 opengl
cmake .. -DCMAKE_TOOLCHAIN_FILE=[vcpkg]/scripts/buildsystems/vcpkg.cmake
```

---

## 📁 Full Project Structure

```text
AdEmpire/
├── src/
│   ├── main.cpp
│   ├── core/
│   │   ├── GameState.h          # All structs + std::hash<ChannelType>
│   │   ├── Simulation.cpp
│   │   └── Difficulty.h
│   ├── systems/
│   │   ├── CampaignEngine.cpp
│   │   ├── AICompetitor.cpp
│   │   ├── EventSystem.cpp
│   │   ├── SaveSystem.cpp
│   │   ├── FitScoreSystem.cpp
│   │   ├── GoalSystem.cpp
│   │   ├── SpecializationSystem.cpp
│   │   ├── ToastSystem.h/.cpp
│   │   ├── StatsTracker.h/.cpp
│   │   ├── StaffLeveling.h/.cpp    ← v0.5
│   │   ├── CampaignTemplates.h/.cpp ← v0.5
│   │   ├── AchievementSystem.h/.cpp ← v0.5
│   │   ├── SeasonalEvents.h/.cpp    ← v0.5
│   │   └── SaveSlots.h              ← v0.5
│   └── ui/
│       ├── Theme.h
│       ├── UIStyle.h
│       ├── Tooltips.h
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
│       ├── ReportPanel.cpp
│       ├── StatsPanel.cpp
│       ├── AchievementsPanel.h/.cpp ← v0.5
│       ├── SaveSlotsPanel.h/.cpp    ← v0.5
│       └── TemplatesPanel.h/.cpp    ← v0.5
├── assets/
│   └── data/
│       ├── clients.json
│       ├── channels.json
│       └── events.json             # 30 moddable events
├── lib/
│   └── imgui/                    # git submodule
├── CMakeLists.txt
├── README.md
└── SETUP.md
```

---

## 🎄 Seasonal Calendar

| Month | Season | Effect |
|---|---|---|
| January | Slowdown | Social −15%, Influencer −20% |
| March | Spring Push | SEO +10%, PR +10% |
| July | **Summer Slump** | **All channels −20%** |
| October | Q4 Warmup | All channels +10% |
| November | **Black Friday** | **Social +60%, PaidSearch +70%** |
| December | **Christmas** | **Social +80%, Email +60%, PaidSearch +90%** |

---

## 🏆 Achievements (15)

| Icon | Title | Condition |
|---|---|---|
| 🤝 | First Contract | Win first client |
| ⭐ | Popular Agency | 5 active clients at once |
| 🏆 | Power Player | 10 active clients at once |
| 🚀 | Campaign Manager | Launch first campaign |
| 📈 | Campaign Veteran | Complete 10 campaigns |
| 💰 | Flush With Cash | Reach $50K budget |
| 🤑 | Six Figures | Reach $100K budget |
| 🗺️ | On the Map | 10% market share |
| 📅 | Major Player | 20% market share |
| 🌟 | Market Leader | 35% market share (win!) |
| 💯 | Trusted Brand | 80% reputation |
| 👥 | Growing Team | Hire first staff member |
| 🏢 | Full Agency | 5 staff members |
| 📆 | Year One | Survive 12 months |
| 📉 | Best Month | Earn $5K in a single month |

---

## 📈 Simulation Model

```text
Reach         = Budget × ReachPerDollar[ch] × EventMod × SeasonalMod × StaffBonus × SpecBonus
CTR           = BaseCTR[ch] × QualityModifier(score)
Conversions   = Reach × CTR × BaseConversion[ch]
ClientRevenue = Conversions × (clientBudget × 0.12)
AgencyFee     = ClientRevenue × 18%
Overhead      = $800 + $280 × max(0, staffCount − 3)
```

Template quality bonus stacks on top of the base quality score (capped at 10.0).

---

## 🎮 Moddable Events

Add or edit events in `assets/data/events.json` without recompiling:

```json
{
  "id": "EVT_MY_EVENT",
  "title": "My Event",
  "description": "Something happening in the market.",
  "impact": "Social +50%",
  "modifiers": { "Social": 1.5, "SEO": 1.0, "Email": 1.0, "Influencer": 1.0, "PR": 1.0, "PaidSearch": 1.0 },
  "budgetImpact": 0,
  "durationMonths": 2,
  "probability": 0.07
}
```

---

## 🔧 Build Audit

| Issue | Fix |
|---|---|
| `nlohmann/json` missing | Auto-downloaded in CMakeLists |
| `std::hash<ChannelType>` | Specialization in `GameState.h` |
| Staff name modulo | Safe `STAFF_NAME_COUNT` constant |
| Missing `<algorithm>` | Added to ClientManager |

> ✅ Zero known blockers — GCC 11+ / Clang 14+ / Ubuntu 22.04+

---

## 🔜 Integration Checklist for v0.5

After pulling, add these hooks to `main.cpp`:

```cpp
// In includes:
#include "systems/StaffLeveling.h"
#include "systems/AchievementSystem.h"
#include "systems/SeasonalEvents.h"
#include "systems/SaveSlots.h"
#include "ui/AchievementsPanel.h"
#include "ui/SaveSlotsPanel.h"
#include "ui/TemplatesPanel.h"

// In new game init:
AchievementSystem::Init();
SaveSlots::LoadMeta();

// In pendingNextMonth block (after AdvanceMonth):
StaffLeveling::AdvanceMonth(gs);
SeasonalEvents::Apply(gs);    // must be after channelModifiers are reset
AchievementSystem::Check(gs);

// In RenderGame():
AchievementsPanel::Render(gs);
SaveSlotsPanel::Render(gs);
TemplatesPanel::Render(gs);

// Navbar buttons:
if (ImGui::Button(" Achievements ")) gs.showAchievements = true;
if (ImGui::Button("  Templates   ")) gs.showTemplates    = true;
if (ImGui::Button("   Save       ")) gs.showSaveSlots    = true;
```

---

## 🔜 What's Next (v0.6)

| Feature | Description |
|---|---|
| Full `main.cpp` integration pass | Wire all v0.5 systems into game loop |
| GameState v0.5 flags | Add `showAchievements`, `showTemplates`, `showSaveSlots` |
| Staff level display in StaffPanel | Show Junior/Mid/Senior/Lead/Principal badge |
| Template filter by active client industry | Auto-filter in TemplatesPanel |
| Leaderboard (local top 10) | Persistent JSON, sort by total revenue |

---

## 📄 License

MIT © 2026 [George Pricop](https://github.com/Gzeu)
