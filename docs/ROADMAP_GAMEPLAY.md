# AdEmpire — Gameplay Roadmap

## Goal
This roadmap focuses on deepening the business gameplay, strategic identity, and replayability of AdEmpire. The objective is not to add more windows first, but to increase meaningful decisions, pressure, differentiation, and retention.

## v0.2 — Depth systems
Priority: highest.

### 1. Client archetypes
Add archetypes to clients:
- Conservative,
- Aggressive,
- Prestige,
- Data-Driven,
- Founder-Led.

Implementation goals:
- new fields in client data,
- archetype-based satisfaction modifiers,
- different negotiation tolerances,
- different preferred channels.

Expected outcome:
- clients become strategic picks instead of flat entries.

### 2. Contract negotiation system
Replace one-click pitch success with negotiation.

Implementation goals:
- contract type selection,
- fee negotiation,
- duration negotiation,
- reporting expectations,
- risk sharing clauses.

Expected outcome:
- pitching becomes real gameplay with trade-offs.

### 3. Industry bonuses
Connect industries directly to channels and objectives.

Implementation goals:
- industry-channel matrix,
- objective-channel matrix,
- FitScore calculation in simulation.

Expected outcome:
- strategic setup matters more than raw budget.

### 4. Capacity and burnout
Introduce growth pressure.

Implementation goals:
- agency capacity stat,
- team load calculation,
- burnout warnings,
- risk modifier in campaign outcome.

Expected outcome:
- overexpansion becomes dangerous and legible.

### 5. Quarterly goals
Add pacing and mid-term agency targets.

Implementation goals:
- quarter tracker,
- random or curated goal selection,
- quarter-end reward/penalty screen.

Expected outcome:
- stronger campaign structure and retention.

### 6. Specialization tree
Create build identity.

Implementation goals:
- 4 branches: Performance, Brand, PR, Growth,
- unlock nodes,
- branch-specific perks and lead quality bonuses.

Expected outcome:
- higher replayability and clearer agency fantasy.

## v0.3 — Scenarios
Priority: high.

### Scenario mode
Add replayable starts with different economic conditions.

Scenarios:
- Startup Mode,
- Recession Mode,
- Crypto Boom Mode,
- Election Season,
- Creator Economy Rush.

Implementation goals:
- scenario selection on new game,
- event weight overrides,
- contract mix overrides,
- rival strength modifiers.

Expected outcome:
- major replayability increase.

## v0.4 — Rival personalities
Priority: high.

### Rival expansion
Move from generic AI competitors to authored rivals.

Target rivals:
- MediaBlaze,
- PeakBrands,
- GrowthLab,
- Signal PR.

Implementation goals:
- rival-specific strengths and weaknesses,
- event affinity,
- poaching patterns,
- preferred industries,
- pricing identities.

Expected outcome:
- stronger market storytelling and strategic response.

## v0.5 — Polish and retention
Priority: medium.

### UX and feedback improvements
Implementation goals:
- better end-of-month recap,
- clearer explanation for satisfaction and churn,
- campaign result breakdowns,
- event banners and toast notifications,
- onboarding hints for first 12 months.

Expected outcome:
- lower confusion and stronger player retention.

### Achievement layer
Implementation goals:
- wealth goals,
- efficiency goals,
- brand goals,
- risk goals,
- rivalry goals,
- specialization goals.

Expected outcome:
- more meta-progression and session goals.

## Suggested implementation order
Recommended next practical order for repo work:
1. Extend `GameState` and client data with archetypes, objectives, contract types, capacity fields.
2. Update `Simulation.cpp` to support FitScore and RiskModifier.
3. Add `NegotiationSystem` for contract generation and acceptance.
4. Add quarterly goals data and quarter processing.
5. Add specialization progression data and UI.
6. Expand AI rivals into authored identities.

## Tech tasks per milestone
### v0.2
- update structs in `src/core/GameState.h`,
- add economy helper functions,
- expand JSON client definitions,
- update dashboard with capacity, quarter, and specialization indicators,
- add negotiation UI screen.

### v0.3
- add scenario config JSON,
- add scenario modifiers at new game start,
- expose scenario effects in dashboard and event system.

### v0.4
- split rival logic into authored profiles,
- add rival event reactions,
- visualize rival strengths in market map.

### v0.5
- improve month-end summary,
- add better charts,
- add achievements and tutorial messaging.

## Definition of success
The roadmap succeeds if AdEmpire evolves from:
- a functional marketing sim prototype,

to:
- a replayable tycoon with strong strategic identity,
- clear agency-building fantasy,
- meaningful month-to-month trade-offs,
- distinct runs based on specialization and scenario.
