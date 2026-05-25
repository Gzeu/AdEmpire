# Modding Guide — AdEmpire

## Custom Events

Edit `assets/data/custom_events.json` to add your own market events.
They are auto-loaded at game start via `ModLoader`.

### Event Schema

```json
{
  "title": "My Custom Event",
  "description": "What happens in this event.",
  "impact": "Short impact label shown in UI",
  "socialMod": 1.0,
  "seoMod": 1.0,
  "emailMod": 1.0,
  "influencerMod": 1.0,
  "prMod": 1.0,
  "paidMod": 1.0,
  "budgetImpact": 0,
  "durationMonths": 2
}
```

### Modifier Reference
- `1.0` = no change
- `1.5` = +50% effectiveness
- `0.5` = -50% effectiveness
- `budgetImpact` = flat $ added/removed from player budget

## Custom Clients

Edit `assets/data/clients.json` to add clients.
New clients are loaded if `id` is not already in the game pool.

## Custom Channel Bonuses

Edit `src/systems/IndustryBonuses.cpp` to tweak industry multipliers.
Recompile after changes.

## Difficulty Tuning

Edit `src/core/Difficulty.h` to adjust `DIFFICULTIES[]` values.
