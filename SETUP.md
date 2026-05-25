# AdEmpire — Quick Setup Guide

## 1. Clone with ImGui submodule

```bash
git clone --recurse-submodules https://github.com/Gzeu/AdEmpire.git
cd AdEmpire
```

If you already cloned without submodules:
```bash
git submodule update --init --recursive
```

## 2. Install dependencies (Ubuntu)

```bash
sudo apt update
sudo apt install cmake build-essential libglfw3-dev libgl1-mesa-dev libglu1-mesa-dev
```

## 3. Get nlohmann/json (auto-downloaded by CMake, or manually)

```bash
curl -L https://github.com/nlohmann/json/releases/download/v3.11.3/json.hpp -o lib/json.hpp
```

> CMake will auto-download this if missing during configure step.

## 4. Build

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

## 5. Run

```bash
./AdEmpire
```

## Project Layout

```
src/
├── main.cpp              ← entry point + game loop
├── core/
│   ├── GameState.h       ← all data structs (+ showAchievements flag)
│   └── Simulation.cpp    ← monthly math + Newsfeed auto-push
├── systems/
│   ├── CampaignEngine
│   ├── AICompetitor
│   ├── EventSystem
│   ├── AchievementsSystem ← NEW: 22 achievements
│   └── SaveSystem        ← FIXED: full JSON save/load
└── ui/
    ├── Theme.h
    ├── MainMenu          ← FIXED: all 30 clients
    ├── Dashboard
    ├── CampaignEditor
    ├── ClientManager     ← FIXED: #include <algorithm>
    ├── MarketMap
    ├── Newsfeed          ← FIXED: auto-receives events
    ├── StaffPanel
    ├── AchievementsPanel ← NEW: panel + popup overlay
    └── SettingsPanel     ← NEW: display/audio/gameplay/save
lib/
├── imgui/    ← git submodule
└── json.hpp  ← nlohmann/json (auto-downloaded by CMake)
```

## Troubleshooting

**json.hpp not found:**
```bash
curl -L https://github.com/nlohmann/json/releases/download/v3.11.3/json.hpp -o lib/json.hpp
```

**GLFW not found:**
```bash
sudo apt install libglfw3-dev
```

**Submodule empty:**
```bash
git submodule update --init --recursive
```
