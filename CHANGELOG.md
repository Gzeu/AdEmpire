# Changelog

All notable changes to **AdEmpire** are documented here.  
Format follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/). Versioning follows [Semantic Versioning](https://semver.org/).

---

## [Unreleased] — v1.0.0

### Planned
- First public binary release (Linux / Windows / macOS via GitHub Actions)
- Screenshots in README
- GitHub Pages landing page live

---

## [0.9.0] — 2026-05-25

### Added
- **AgencyBrandingPanel** — custom agency color (ImVec4) + logo selector (Diamond, Globe, Rocket, Star, Crown) persisted in `GameState`
- **ChartRenderer** — real-time revenue bar charts rendered with ImGui DrawList, no external library
- **SplashScreen** — animated intro with logo morph and tagline fade
- **VictoryScreen** — full-screen celebration panel on 35% market share, shows final stats
- **EventPopup** — modal popup gate for market events; blocks month advance until dismissed
- `GameEvent` struct with full field set: `id`, `title`, `description`, `impact`, `budgetDelta`, `reputationDelta`, `marketShareDelta`, 6 channel modifiers, `durationMonths`
- `pendingEventPopup` + `currentEvent` fields added to `GameState`
- **CMakeLists.txt** cross-platform overhaul: Linux pkg-config fallback + apt hint, macOS Cocoa/IOKit/CoreVideo auto-link, Windows MSVC `/W3 /O2` + vcpkg hint, FetchContent for nlohmann/json, CPack TGZ/ZIP/NSIS, post-build asset copy
- **GitHub Actions** `release.yml`: matrix build on `ubuntu-22.04`, `windows-2022`, `macos-14` arm64; auto-publishes GitHub Release with download table on any `v*.*.*` tag

### Changed
- `main.cpp` updated to integrate all five v0.9 UI modules

---

## [0.8.0] — 2026-05-25

### Added
- **OpenAL audio system** — procedural WAV synthesis (sine tone generator, no external audio files required)
- **REST leaderboard** via `cpp-httplib` — optional online score submission with local fallback
- Audio mute toggle in settings panel

---

## [0.7.0] — 2026-05-25

### Added
- **StatsTracker** — revenue per channel, win rates by industry, 24-month retention history, monthly breakdowns
- **DifficultySystem** — Easy / Normal / Hard / Nightmare presets affecting AI aggression, event frequency, economy multipliers
- **Leaderboard** — local top-10 stored in JSON, sorted by total revenue
- **31 moddable JSON events** in `assets/data/events.json`; custom events load at startup with zero recompilation
- Integration guide in README (4-line `main.cpp` snippet)
- Version history with direct commit links
- Post-v0.7 optional roadmap: OpenAL stub, Steam achievements, REST API

### Changed
- README v0.7: badge v0.7 COMPLETE, full feature matrix on 3 layers (Core / Progression / Polish), exact file counts

---

## [0.6.0] — 2026-05-25

### Changed
- Integrated all v0.5 systems into `main.cpp` and `GameState`
- README updated with v0.5 feature list and v0.6 status

---

## [0.5.0] — 2026-05-25

### Added
- **Staff leveling** — 5 levels (Junior → Principal), skill +0.8%/month, salary scales with level
- **15 campaign templates** — pre-configured channel allocations per industry
- **15 achievements** — unlocked by milestones (first campaign, 10 clients, $1M revenue, etc.)
- **Seasonal bonuses** — Q4 holiday multiplier, summer slump penalty, etc.
- **3 save slots** — independent JSON save/load with slot picker UI

---

## [0.4.0] — 2026-05-25

### Added
- **StatsTracker** early version — 30 moddable events in JSON, tooltip system
- README with full feature matrix and build instructions

---

## [0.3.0] — 2026-05-25

### Added
- **ToastSystem** — bottom-right notification queue with auto-dismiss
- **ReportPanel** — end-of-month breakdown of revenue, campaign ROI, client satisfaction deltas
- **UIStyle** — global ImGui theme (dark, gold accent, custom window rounding)
- Economy rebalance: campaign fees adjusted, AI aggression tuned
- Difficulty selector in main menu

### Fixed
- AI competitors no longer double-fire market-share steal events

---

## [0.2.0] — 2026-05-25

### Added
- **NegotiationSystem** — 6 actions (Pitch, Discount, Case Study, Urgency, Partner, Walk Away), mood bar, probability resolver
- **FitScore** — composite score from channel fit, industry XP, reputation, capacity utilization
- **ClientArchetype** + **ContractType** enums with rate multipliers
- **QuarterlyGoals** — 3-month objective cycle with reward/penalty
- **8 Specializations** — Fashion/Influencer ×1.6, Tech/SEO ×1.5, Finance/Paid ×1.5, etc.
- Capacity / burnout system tied to staff headcount

---

## [0.1.0] — 2026-05-25

### Added
- Initial project structure and build system (CMake, ImGui submodule, GLFW, OpenGL 3.3)
- `GameState` core struct
- **CampaignManager** — 6 channels (Social, SEO, Email, Influencer, PR, Paid Search), ROI formula: Reach × CTR × Conv × AOV → Agency fee
- **ClientManager** — 30 clients across 8 industries, satisfaction decay, churn logic
- **StaffManager** — hire/fire, salary, skill bonus on campaign output
- **AIManager** — 3 rivals (MediaBlaze, PeakBrands, NicheNation) with monthly market-share competition
- **EventSystem** — random event dispatcher with budget/reputation/market-share deltas
- **Dashboard** — ImGui main loop with KPI cards, client list, campaign panel
- **SaveSystem** — JSON serialization via nlohmann/json
- Win condition: reach 35% market share

---

[Unreleased]: https://github.com/Gzeu/AdEmpire/compare/v0.9.0...HEAD
[0.9.0]: https://github.com/Gzeu/AdEmpire/compare/v0.8.0...v0.9.0
[0.8.0]: https://github.com/Gzeu/AdEmpire/compare/v0.7.0...v0.8.0
[0.7.0]: https://github.com/Gzeu/AdEmpire/compare/v0.6.0...v0.7.0
[0.6.0]: https://github.com/Gzeu/AdEmpire/compare/v0.5.0...v0.6.0
[0.5.0]: https://github.com/Gzeu/AdEmpire/compare/v0.4.0...v0.5.0
[0.4.0]: https://github.com/Gzeu/AdEmpire/compare/v0.3.0...v0.4.0
[0.3.0]: https://github.com/Gzeu/AdEmpire/compare/v0.2.0...v0.3.0
[0.2.0]: https://github.com/Gzeu/AdEmpire/compare/v0.1.0...v0.2.0
[0.1.0]: https://github.com/Gzeu/AdEmpire/releases/tag/v0.1.0
