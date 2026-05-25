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

## 3. Build

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

## 4. Run

```bash
./AdEmpire
```

## Troubleshooting

**GLFW not found:**
```bash
sudo apt install libglfw3-dev
# or build from source: https://www.glfw.org/download.html
```

**OpenGL not found:**
```bash
sudo apt install libgl1-mesa-dev
```

**Submodule empty (no imgui files in lib/imgui):**
```bash
git submodule update --init --recursive
```

## Project Layout

```
src/
├── main.cpp           ← entry point, GLFW + ImGui init + game loop
├── core/
│   ├── GameState.h    ← all data structures
│   ├── GameState.cpp
│   └── Simulation.cpp ← monthly simulation math
├── systems/
│   ├── CampaignEngine ← campaign creation + ROI estimation
│   ├── AICompetitor   ← rival agency logic
│   ├── EventSystem    ← 20 market events
│   └── SaveSystem     ← JSON save/load
└── ui/
    ├── Theme.h        ← dark blue marketing theme
    ├── MainMenu       ← start screen
    ├── Dashboard      ← KPIs + revenue chart
    ├── CampaignEditor ← create + view campaigns
    ├── ClientManager  ← pitch + manage clients
    ├── MarketMap      ← share visualization
    ├── Newsfeed       ← events log
    └── StaffPanel     ← hire + manage team
```
