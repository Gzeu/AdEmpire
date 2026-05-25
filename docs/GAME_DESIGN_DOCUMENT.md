# AdEmpire — Game Design Document

## High concept
AdEmpire is a business simulation and strategy game about building, specializing, and scaling a digital marketing agency from a fragile boutique startup into a market-dominating firm. The player sells outcomes, manages risk, balances client happiness against profitability, reacts to macro market shifts, and tries to capture market share before collapsing under financial or operational pressure.

The product identity is:

**Digital Agency Tycoon with a strong Viral Growth Simulator layer.**

That identity keeps the game commercially readable as a tycoon while differentiating it through modern systems: algorithms, creator economy, trend spikes, PR crises, performance pressure, and short windows of opportunity.

## Player fantasy
The player fantasy is not "designing UI windows" or "clicking dashboards." It is:
- Building a respected agency brand.
- Winning important clients before rivals do.
- Turning unstable trends into profit.
- Choosing what kind of agency to become.
- Surviving periods of cashflow stress through smarter strategy.
- Reaching a point where the market reacts to the player, not the other way around.

The fantasy should feel like a blend of:
- operator,
- strategist,
- negotiator,
- brand architect,
- crisis manager.

## Core pillars
The design should be centered around four pillars:

### 1. Cashflow vs growth
The player should constantly choose between safe contracts and aggressive expansion. Fast growth should create payroll pressure, execution risk, and future churn exposure.

### 2. Client happiness vs profitability
A highly profitable deal that hurts satisfaction should threaten renewals and reputation. Over-serving clients should improve retention but reduce margins.

### 3. Specialization vs diversification
Focused agencies should gain better efficiency and premium positioning in certain verticals, but lose flexibility. Diversified agencies should be safer but less dominant in any niche.

### 4. Short-term wins vs long-term brand
The player should be able to chase quick revenue spikes, but repeated short-term behavior should weaken brand trust, premium access, and long-term lead quality.

## Game loop

### Monthly core loop
The economy runs on a monthly turn. Every month the player:
1. Reviews agency dashboard: cash, reputation, pipeline, team load, churn risk, quarterly goals.
2. Evaluates incoming leads and negotiates contracts.
3. Allocates budget, channels, and strategic intent for campaigns.
4. Hires, reorganizes, or upgrades staff and tools.
5. Ends the month and processes outcomes, renewals, churn, rival actions, and market events.

### Weekly presentation loop
Weeks should exist as a presentation and feedback layer, not as a second full simulation layer. Weekly beats can show:
- alerts,
- campaign milestones,
- client feedback,
- team burnout warnings,
- trend spikes,
- poaching attempts,
- headline events.

This creates rhythm and storytelling without multiplying simulation complexity.

## System map
The business model should be built from five layers:

| Layer | Role | Produces |
|---|---|---|
| Leads | Source of opportunity | New pipeline |
| Contracts | Monetization structure | Stable or volatile income |
| Campaigns | Performance engine | Results and satisfaction shifts |
| Staff & tools | Operational multipliers | Efficiency, quality, capacity |
| Reputation & specialization | Strategic metaprogression | Better pricing, lead quality, prestige |

## Leads system
Leads should replace the simple idea of a static available client. A lead is an opportunity package with business context.

Each lead should include:
- industry,
- company size,
- urgency,
- price sensitivity,
- risk tolerance,
- dominant objective,
- budget band,
- prestige value,
- expected reporting strictness,
- potential contract type.

### Lead objectives
Every lead should have a primary objective:
- Awareness,
- Leads,
- Sales,
- Retention,
- PR Repair,
- Product Launch.

This lets the same channel perform differently depending on context. A fashion client seeking awareness behaves differently from a finance client demanding efficient acquisition.

### Lead quality
Lead quality should be influenced by:
- agency reputation,
- industry specialization,
- previous client retention,
- recent market events,
- rival dominance in that vertical.

High lead quality means better contract terms, lower negotiation friction, and higher renewal potential.

## Contract system
Contracts should become a dedicated gameplay system instead of a single binary pitch result.

### Contract types
Four recommended contract types:

| Type | Description | Risk | Reward |
|---|---|---|---|
| Retainer | Stable recurring monthly contract | Low | Medium |
| Performance-based | Revenue depends on outcomes | High | High |
| Launch campaign | Short project, fast cash | Medium | Medium |
| Crisis PR | Rare high-margin emergency contract | High | Very High |

### Contract variables
Each contract should define:
- duration,
- base fee,
- performance bonus or penalty,
- reporting requirements,
- renewal threshold,
- scope complexity,
- reputation impact,
- termination clause.

### Negotiation system
Negotiation should let the player trade between:
- price,
- duration,
- expected results,
- reporting intensity,
- exclusivity,
- risk-sharing.

The core decision is not just “take client or not,” but “under what terms does this client become worth taking?”

## Campaign simulation
The campaign layer is the center of the game’s business depth.

### Campaign inputs
Every campaign should be built from three decision groups:
- strategic setup: channel, budget, message type, target objective,
- delivery quality: team quality, tools, specialization, fit,
- execution pressure: capacity load, burnout, number of concurrent campaigns.

### Recommended model
The current formula can remain the base, but it should be extended with two key factors:
- **FitScore** — how well the selected channel matches the client’s industry and objective,
- **ExecutionRisk** — penalty when the agency is overloaded or lacks expertise.

Recommended outcome formula:

```text
Outcome = BaseChannelPower × Budget × FitScore × TeamQuality × MarketModifier × RiskModifier
```

### Campaign outputs
Each campaign should produce at least:
- reach,
- CTR,
- conversion rate,
- client-perceived value,
- revenue generated for client,
- agency revenue,
- satisfaction delta,
- reputation delta,
- staff stress delta.

### Strategic consequences
A campaign should not be judged only by money. It should also influence:
- renewal chance,
- industry reputation,
- future lead quality,
- staff burnout,
- rival behavior.

## Client model
Clients should feel like economic personalities, not table rows.

Each client should have:
- archetype,
- objective priority,
- renewal threshold,
- churn sensitivity,
- negotiation style,
- preferred channels,
- price tolerance,
- brand value to agency.

### Client archetypes

| Archetype | Wants | Hates | Best fit |
|---|---|---|---|
| Conservative | Predictability | Volatility | Retainers |
| Aggressive | Fast growth | Slow ramp-up | Performance deals |
| Prestige | Premium brand image | Cheap-looking tactics | Branding / PR |
| Data-Driven | Reporting and ROI clarity | Vague creative claims | SEO / Paid / Analytics |
| Founder-Led | Speed and intuition | Bureaucracy | Startups / Viral plays |

### Why archetypes matter
Archetypes create emergent gameplay:
- some clients are high maintenance but boost prestige,
- some are easy to satisfy but low-margin,
- some are risky but can accelerate specialization.

That makes the portfolio itself part of the strategy.

## Staff and operations
Staff should evolve from static bonuses into an operational structure.

### Staff axes
Recommended four-axis structure:
- Delivery: Social, SEO, Paid, PR, Content.
- Client Service: Account Manager.
- Intelligence: Data Analyst.
- Leadership: Creative Director, Strategy Lead.

### Operational stats
The agency should gain the following new operational stats:
- **Capacity score** — how many campaigns can be handled without efficiency loss,
- **Burnout risk** — increased by overwork and poor staffing balance,
- **Seniority** — junior, mid, senior,
- **Morale** — improved by stable months and office upgrades,
- **Hiring market quality** — determines who is available to recruit.

### Capacity model
Capacity should be one of the most important anti-snowball systems in the game. Growth without structure should create penalties.

Examples:
- too many campaigns lowers quality score,
- lack of account management hurts satisfaction,
- too many performance contracts increases volatility,
- too many simultaneous crises spike burnout.

### Office progression
Office progression can be a secondary progression layer:
- Home Office,
- Small Studio,
- City Office,
- Premium HQ.

Office upgrades can increase:
- hiring slots,
- morale,
- premium lead attraction,
- client meeting quality,
- passive brand prestige.

## Tools and agency infrastructure
Tools should act as modifiers with upkeep, not cosmetic upgrades.

Suggested categories:
- analytics suite,
- ad automation stack,
- influencer CRM,
- reporting dashboard,
- creative collaboration tools,
- social listening tools.

Tool upgrades should improve:
- reporting quality,
- execution speed,
- campaign fit visibility,
- event response speed,
- premium client compatibility.

## AI rivals
Rivals should be distinct strategic actors, not generic share holders.

Recommended rivals:

| Rival | Identity | Strength | Weakness |
|---|---|---|---|
| MediaBlaze | Volume player | Cheap pricing, fast scaling | High churn |
| PeakBrands | Premium branding agency | Prestige and large clients | Slower growth |
| GrowthLab | Performance specialists | High ROI execution | Vulnerable to privacy shocks |
| Signal PR | Reputation and crisis experts | Excels in scandals and media events | Weak on stable growth channels |

### Rival variables
Each rival should have:
- specialization,
- pricing style,
- poaching behavior,
- event affinity,
- risk tolerance,
- weak spots,
- favorite industries.

### Rival reaction design
Rivals should react to the player by:
- targeting the same vertical,
- undercutting prices,
- poaching unhappy clients,
- grabbing specific opportunity events,
- dominating categories where the player is weak.

This creates narrative pressure and strategic identity on every run.

## Progression structure
The campaign should be divided into four phases.

### Phase 1 — Survival
- very limited cash,
- low team capacity,
- early retainers matter,
- simple decisions with visible consequences.

### Phase 2 — Expansion
- staff and tools enter the equation,
- more leads appear,
- rivals begin to matter,
- specialization starts becoming visible.

### Phase 3 — Positioning
- the player can refuse clients,
- vertical reputation matters,
- premium clients appear,
- strategic focus beats raw volume.

### Phase 4 — Domination
- enterprise contracts and market wars,
- quarter goals become harder,
- rivals react directly to the player,
- market share control becomes the central victory path.

## Specialization tree
The specialization tree should define build identity.

### Branches

| Branch | Focus | Typical clients |
|---|---|---|
| Performance | ROI, paid, analytics, efficiency | Fintech, ecommerce, SaaS |
| Brand | Awareness, image, premium positioning | Fashion, consumer brands |
| PR | Crisis management, reputation insulation, media leverage | Finance, health, enterprise |
| Growth | Social, creator economy, viral spikes | Startups, gaming, creator brands |

### Design goal
The player should be allowed to go:
- full specialist,
- hybrid,
- adaptive generalist.

But every path must have trade-offs. A strong specialization should unlock better fit, better leads, and better pricing power, while reducing flexibility elsewhere.

## Reputation and market share
Reputation should not be only a cosmetic progress bar. It should directly affect:
- lead quality,
- premium contract access,
- negotiation leverage,
- churn tolerance,
- hiring quality,
- market share acceleration.

Market share should be influenced by:
- active client portfolio value,
- retention rate,
- specialization dominance,
- rival pressure,
- event exploitation,
- campaign consistency.

## Quarterly goals
Quarterly goals should provide pacing and mid-term objectives.

Examples:
- hit a revenue target,
- retain 90% of clients,
- sign 2 premium brands,
- dominate one vertical,
- survive a recession quarter,
- complete a quarter with zero burnout events.

Quarter goals improve structure and create mini-arcs inside a long campaign.

## Scenario mode
Scenario mode can greatly improve replayability.

Recommended scenarios:
- Startup Mode,
- Recession Mode,
- Crypto Boom Mode,
- Election Season,
- Creator Economy Rush.

Each scenario should shift:
- available leads,
- event frequency,
- contract mix,
- dominant channels,
- rival strengths.

## Win and lose states
### Win states
Recommended win conditions:
- reach target market share,
- dominate a chosen specialization vertical,
- build a premium reputation threshold,
- complete a long campaign score challenge.

### Lose states
Recommended fail states:
- deep insolvency,
- catastrophic churn spiral,
- reputation collapse,
- sustained burnout and delivery failure.

## Positioning and monetization
AdEmpire is best positioned as a premium single-purchase indie PC simulation game. The concept is strong because the subject is rare and commercially understandable.

Recommended commercial structure:
- premium full game,
- demo with 12 months of gameplay,
- optional supporter pack or soundtrack.

The game should not be designed around live-service expectations. Its strength is depth, replayability, and theme novelty.

## Immediate implementation priorities
Recommended next sprint order:
1. Client archetypes.
2. Contract negotiation system.
3. Quarterly goals.
4. Industry bonuses tied to simulation.
5. Agency specialization tree.
6. Capacity and burnout.

## Content roadmap summary
### v0.2 — Depth systems
- client archetypes,
- contract negotiation,
- FitScore and ExecutionRisk,
- capacity and burnout,
- industry bonuses.

### v0.3 — Scenarios
- scenario mode,
- economy presets,
- event weighting per scenario,
- scenario-specific rewards.

### v0.4 — Rival personalities
- 4 full rivals,
- distinct AI behaviors,
- vertical competition,
- targeted poaching logic.

### v0.5 — Polish and retention
- better alerts and feedback,
- clearer outcome explanations,
- achievement layer,
- onboarding and tutorial beats,
- end-of-quarter summaries.
