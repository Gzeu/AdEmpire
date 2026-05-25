# Contributing to AdEmpire

Thank you for your interest! AdEmpire is a C++ marketing tycoon game built with Dear ImGui. Contributions of any size are welcome.

## Quick Start

```bash
git clone --recurse-submodules https://github.com/Gzeu/AdEmpire.git
cd AdEmpire && mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)   # Linux / macOS
```

**Windows:** install [vcpkg](https://vcpkg.io) and run:
```bash
cmake .. -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake
```

## How to Contribute

### 🐛 Bug Reports
Open an issue with:
- OS and compiler version (`g++ --version`)
- Exact error output or steps to reproduce
- Relevant `GameState` fields if it's a logic bug

### 💡 Feature Requests
Check [open issues](https://github.com/Gzeu/AdEmpire/issues) first. If your idea is new, open an issue describing:
- What system it affects (Campaign / Client / Staff / AI / UI)
- Expected behavior change
- Whether it requires a new file or extends existing ones

### 📦 Pull Requests

1. Fork the repo and create a branch: `git checkout -b feat/my-feature`
2. Follow the code style (see below)
3. Add your feature — every new `.cpp` system needs a matching `.h`
4. Test a full compile: `cmake .. && make -j$(nproc)` must produce zero errors
5. Open a PR with a clear description and reference to the relevant issue

## Code Style

| Rule | Detail |
|---|---|
| Language | C++17 |
| Naming | `PascalCase` for classes, `camelCase` for methods and variables |
| Files | One class per file; header in `core/` or `systems/` or `ui/` |
| ImGui | Immediate-mode only — no retained-mode state outside `GameState` |
| JSON data | Moddable data goes in `assets/data/` as `.json`, not hardcoded |
| No `new`/`delete` | Use `std::vector`, `std::string`, value semantics |

## Adding New Events

The fastest contribution: add events to `assets/data/events.json`. No C++ changes needed.

```json
{
  "id": "your_event_id",
  "title": "Event Title",
  "description": "What happened and why it matters.",
  "impact": "positive",
  "budgetDelta": 5000,
  "reputationDelta": 2,
  "marketShareDelta": 0.5,
  "channelModifiers": {
    "Social": 1.2,
    "SEO": 1.0,
    "Email": 1.0,
    "Influencer": 1.3,
    "PR": 1.1,
    "PaidSearch": 1.0
  },
  "durationMonths": 2
}
```

## Roadmap Areas

Want to pick up something meaty? These are open:

- `v1.0` — screenshots, GitHub Pages live, first binary release
- OpenAL audio polish — procedural SFX for campaign win/loss/achievement
- REST leaderboard server — small Node.js or Go backend
- Steam Greenworks integration stub
- Tutorial / onboarding overlay for new players

## License

By contributing you agree your code is released under the [MIT License](LICENSE).
