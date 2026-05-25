# 🎮 AdEmpire — Marketing Tycoon

> Build and grow a digital marketing agency from scratch. Manage clients, launch campaigns, beat AI rivals, and become the #1 ad agency in the market.

![C++](https://img.shields.io/badge/C++-17-blue.svg)
![ImGui](https://img.shields.io/badge/Dear%20ImGui-latest-green.svg)
![OpenGL](https://img.shields.io/badge/OpenGL-3.3-orange.svg)
![License](https://img.shields.io/badge/license-MIT-purple.svg)

## 🕹️ Gameplay

- **Month 1**: Start with $10,000 budget, 0 clients. Pitch to small businesses.
- **Month 3**: Grow to 3 clients. Hire specialists. Navigate market events.
- **Month 12**: Compete for #1 market share against AI agencies.

## 🧠 Core Systems

| System | Description |
|---|---|
| Campaign Engine | Math simulation: Reach, CTR, ROI, conversions |
| AI Competitors | 3 rival agencies that adapt and steal clients |
| Event System | 50+ random events (algorithm changes, viral trends, PR crises) |
| Client Manager | 30 clients with satisfaction scores & contract renewal |
| Market Map | Real-time market share visualization |
| Save System | Full JSON save/load |

## 📦 Tech Stack

- **Dear ImGui** — all UI (immediate mode)
- **GLFW 3** — window & input
- **OpenGL 3.3** — rendering backend
- **GLM** — math
- **nlohmann/json** — save system & data files
- **CMake** — build system

## 🚀 Build & Run

### Prerequisites (Ubuntu/Debian)
```bash
sudo apt install cmake libglfw3-dev libgl1-mesa-dev
```

### Clone with submodules
```bash
git clone --recurse-submodules https://github.com/Gzeu/AdEmpire.git
cd AdEmpire
```

### Build
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
./AdEmpire
```

## 📁 Project Structure

```
AdEmpire/
├── src/
│   ├── main.cpp              # Entry point + game loop
│   ├── core/
│   │   ├── GameState.h       # All game data structs
│   │   ├── GameState.cpp
│   │   └── Simulation.cpp    # Economic simulation engine
│   ├── systems/
│   │   ├── CampaignEngine.cpp
│   │   ├── AICompetitor.cpp
│   │   ├── EventSystem.cpp
│   │   └── SaveSystem.cpp
│   └── ui/
│       ├── MainMenu.cpp
│       ├── Dashboard.cpp
│       ├── CampaignEditor.cpp
│       ├── ClientManager.cpp
│       ├── MarketMap.cpp
│       └── Newsfeed.cpp
├── assets/
│   ├── data/
│   │   ├── clients.json
│   │   ├── events.json
│   │   └── channels.json
│   └── fonts/
├── lib/
│   └── imgui/                # git submodule
└── CMakeLists.txt
```

## 🎯 Roadmap

- [x] Project structure & CMake setup
- [x] GameState core data model
- [x] Campaign Engine math
- [x] Dashboard UI
- [x] Client Manager
- [x] AI Competitor system
- [x] Event System (50 events)
- [x] Save/Load system
- [ ] Sound effects (OpenAL)
- [ ] Achievements system
- [ ] Steam integration

## 📄 License

MIT © 2026 George Pricop
