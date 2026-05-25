# AdEmpire — Economy Design

## Purpose
This document defines the business simulation economy for AdEmpire. The goal is to make every month feel like a meaningful management decision under constraints, not a flat spreadsheet optimization problem.

The economy should always create tension between:
- liquidity and expansion,
- client happiness and margins,
- specialization and flexibility,
- safe contracts and explosive opportunities.

## Resource model
The core player resources should be:

| Resource | Meaning | Primary uses |
|---|---|---|
| Cash | Liquid budget | Salaries, tools, campaign allocation, upgrades |
| Reputation | Market trust and perceived quality | Better leads, pricing power, premium access |
| Market Share | Strategic dominance metric | Main campaign victory path |
| Capacity | Operational throughput | Limits number and complexity of campaigns |
| Morale | Team resilience | Protects quality and burnout risk |
| Specialization Score | Vertical/channel mastery | Better fit, better leads, better efficiency |

## Income types
AdEmpire should have multiple income streams so the player can build different business models.

### 1. Retainer income
- fixed monthly payment,
- lower upside,
- stable planning value,
- strongest synergy with conservative clients.

### 2. Performance income
- tied to campaign outcomes,
- volatile,
- potentially highest margin,
- best for aggressive growth builds.

### 3. Launch project income
- short burst of money,
- low retention value,
- useful for survival or transition phases.

### 4. Crisis PR income
- rare,
- high fee,
- high reputation and execution risk,
- highly dependent on timing and event windows.

### 5. Prestige uplift value
Prestige clients may indirectly increase future lead quality, allowing higher-value contracts later even if their immediate margins are not optimal.

## Expense types
### Fixed expenses
- salaries,
- office rent,
- software subscriptions,
- infrastructure/tools,
- passive brand upkeep.

### Variable expenses
- campaign budgets,
- rush delivery costs,
- outsourcing,
- recruitment fees,
- crisis recovery costs,
- client-specific reporting overhead.

### Hidden costs
Some decisions should generate invisible future costs:
- overpromising increases churn exposure,
- overloading the team raises burnout risk,
- under-servicing premium clients hurts brand value,
- excessive discounting weakens pricing power.

## Leads economy
Leads are the top of the opportunity funnel.

### Lead attributes
Each lead should include:
- industry,
- size tier,
- objective,
- budget range,
- urgency,
- risk tolerance,
- prestige value,
- negotiation strictness,
- preferred contract type.

### Lead generation sources
Lead flow should come from:
- baseline market generation,
- referrals from satisfied clients,
- reputation thresholds,
- specialization dominance,
- quarterly goal rewards,
- temporary event surges.

### Lead quality formula
A conceptual formula:

```text
LeadQuality = BaseMarketQuality × ReputationModifier × SpecializationModifier × EventModifier × RivalPressureModifier
```

## Contract economics
### Retainer
- predictable recurring revenue,
- lower churn tolerance,
- strong dependence on steady satisfaction.

### Performance-based
- payout linked to outcomes,
- high upside,
- high variance,
- more sensitive to market modifiers.

### Launch campaign
- front-loaded cash,
- weak renewal baseline,
- useful to patch cashflow holes.

### Crisis PR
- best margins,
- event-dependent,
- failure causes heavy reputation damage.

## Campaign economics
Campaign output should be driven by both business fit and operational readiness.

### Recommended simulation factors
- BaseChannelPower,
- Budget,
- FitScore,
- TeamQuality,
- ToolQuality,
- MarketModifier,
- RiskModifier,
- ClientExpectationModifier.

### Conceptual outcome formula
```text
Outcome = BaseChannelPower × Budget × FitScore × TeamQuality × ToolQuality × MarketModifier × RiskModifier
```

### Agency revenue formulas
Example agency monetization paths:

```text
RetainerRevenue = BaseContractFee
PerformanceRevenue = ClientRevenue × AgencyShare
LaunchRevenue = FixedProjectFee + OptionalSuccessBonus
CrisisPRRevenue = EmergencyFee × ReputationMultiplier
```

## Satisfaction model
Satisfaction should be one of the most important invisible currencies in the game.

### Satisfaction inputs
Client satisfaction should be influenced by:
- result quality,
- reporting quality,
- fit between client objective and channel strategy,
- speed of response,
- overpromising vs delivery gap,
- account management quality,
- competitor contact pressure.

### Satisfaction model
Conceptual model:

```text
SatisfactionDelta = ResultsScore + ReportingScore + ServiceScore - ExpectationGap - DelayPenalty
```

### Design note
A client can be profitable and still unhappy. That distinction is essential for good tycoon gameplay.

## Churn model
Churn should not be purely random. It should be legible and connected to player behavior.

### Churn drivers
- low satisfaction,
- repeated delivery underperformance,
- poor channel fit,
- high competitor pressure,
- contract ending without relationship strength,
- agency reputation damage,
- pricing mismatch.

### Churn risk model
```text
ChurnRisk = BaseRisk + SatisfactionPenalty + RivalPoachPressure + ContractEndPressure - RelationshipStrength
```

### Renewal threshold
Each client should have a renewal threshold, modified by archetype. Conservative and Data-Driven clients should punish inconsistency more than Founder-Led clients.

## Reputation model
Reputation is a strategic multiplier that shapes future opportunity.

### Reputation sources
Positive:
- good retention,
- premium clients served well,
- strong quarterly goal completion,
- successful crisis handling,
- specialization success.

Negative:
- churn streaks,
- failed launches,
- burnout-driven underdelivery,
- public crises,
- over-discounting and inconsistent quality.

### Reputation effects
Higher reputation should improve:
- lead quality,
- renewal tolerance,
- negotiation leverage,
- premium client availability,
- hiring pool quality.

## Market share model
Market share should represent meaningful business influence, not just revenue totals.

### Share sources
The player’s market share should be influenced by:
- total portfolio value,
- number of retained quality clients,
- dominance in a specialization,
- rival weaknesses exploited,
- performance during major events,
- quarter-over-quarter consistency.

### Conceptual model
```text
MarketShareGain = PortfolioStrength + ReputationMomentum + SpecializationBonus + RivalWeaknessExploit - ChurnLoss
```

## Capacity and burnout
Capacity is the main brake on runaway growth.

### Capacity sources
- staff count,
- staff seniority,
- leadership quality,
- office tier,
- tools.

### Burnout triggers
- too many simultaneous campaigns,
- too many high-pressure contracts,
- repeated crisis events,
- poor staffing mix,
- rapid expansion without leadership.

### Burnout effects
- lower delivery quality,
- slower response time,
- reduced satisfaction,
- weaker negotiation outcomes,
- random staff loss or morale penalties.

## Specialization economy
Specialization should create asymmetric strategy paths.

### Example effects
- Performance specialization increases reporting quality and ROI contracts.
- Brand specialization improves premium client attraction and awareness contracts.
- PR specialization reduces damage from crises and boosts emergency opportunities.
- Growth specialization amplifies social volatility and creator-trend exploitation.

### Cost of specialization
The trade-off should be real:
- stronger niche performance,
- weaker flexibility outside chosen strengths,
- better premium fit for some clients,
- worse fit for unrelated leads.

## Balancing principles
The economy should follow these balancing principles:

1. Stable strategies should be viable, not dominant.
2. Aggressive strategies should be rewarding, but fragile.
3. Growth should create pressure before it creates comfort.
4. Premium clients should improve long-term value, not always short-term cash.
5. Player mistakes should be visible, explainable, and recoverable until late collapse.

## Anti-snowball systems
To prevent the game from becoming trivial once the player is ahead, add:
- burnout escalation,
- premium client expectations,
- stronger rival reactions,
- event-based disruption,
- diminishing gains from uncontrolled diversification,
- reputation fragility at high visibility.

## Success criteria for the economy
The economy is working if the player regularly asks:
- Can I afford to grow this month?
- Should I chase this risky opportunity?
- Is this client worth the stress?
- Do I need better staff or better positioning?
- Am I building a stronger agency or just surviving one more month?
