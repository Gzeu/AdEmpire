# Market Integration Guide

## Overview

AdEmpire v1.0 connects to **live financial and news data** via public APIs — no API key, no account required. The game reacts to real-world market conditions every 5 minutes.

## Architecture

```
src/network/
├── MarketState.h        — unified data struct (all market data lives here)
├── CryptoFeed.h         — Binance + Fear & Greed + CoinGecko
├── NewsFeed.h           — CoinDesk RSS + Reddit RSS + Hacker News
├── TrendFeed.h          — Wikipedia Trending + Exchange Rates
└── MarketFeed.h         — singleton orchestrator, async refresh every 5min

src/systems/
└── MarketEventBridge.h  — maps MarketState → EventSystem triggers

data/
└── market_events.json   — 8 real-world-triggered events (moddable)
```

## Data Sources

| Source | Data | Endpoint | Auth |
|--------|------|----------|------|
| Binance Public | BTC/ETH price, 24h change, volume | `api.binance.com/api/v3/ticker/24hr` | None |
| alternative.me | Fear & Greed Index 0–100 | `api.alternative.me/fng/` | None |
| CoinGecko | BTC dominance, global market cap | `api.coingecko.com/api/v3/global` | None |
| CoinDesk RSS | Crypto news headlines | `coindesk.com/arc/outboundfeeds/rss` | None |
| Reddit RSS | r/marketing, r/digitalmarketing | `reddit.com/r/{sub}/hot.rss` | None |
| Hacker News | Tech top stories, AI hype detection | `hacker-news.firebaseio.com/v0/` | None |
| Wikipedia | Global trending topics | `en.wikipedia.org/w/api.php?list=mostviewed` | None |
| exchangerate.host | EUR/USD/RON live rates | `api.exchangerate.host/latest` | None |

## Integration — 3 lines in main.cpp

```cpp
#include "network/MarketFeed.h"
#include "systems/MarketEventBridge.h"

// After ImGui init:
MarketFeed::Get().StartAsync();

// In your Next Month tick:
const MarketState& ms = MarketFeed::Get().GetState();
MarketEventBridge::EvaluateTriggers(ms, [&](const std::string& id){
    eventSystem.TriggerEventById(id);  // your existing call
});
float roiMult = MarketEventBridge::GetCampaignMultiplier(ms);

// In Dashboard panel:
ImGui::Text("%s", MarketEventBridge::GetMarketSummary(ms).c_str());
```

## Game Effects

| Real World Condition | Game Effect |
|---------------------|-------------|
| BTC +8% in 24h | `crypto_bull_run` — Tech/Fintech clients +20-25% budget |
| BTC -8% in 24h | `crypto_winter` — Fintech freezes, ROI -15% |
| Fear & Greed < 20 | `market_panic` — All budgets -25%, rivals play defensive |
| HN: 3+ AI stories | `ai_hype_wave` — Tech campaigns +30% ROI |
| AI hype + BTC bull | `tech_sector_boom` — Peak conditions, +35% tech ROI |
| Ad health > 130% | `ad_market_boom` — All contracts +15% value |
| Ad health < 70% | `ad_budgets_cut` — 25% fewer new leads |
| Wikipedia trending | `trending_topic_active` — Targeting it = +25% engagement |

## Offline Mode

If all network calls fail, `MarketState::Neutral()` is used automatically — the game runs exactly as before v1.0. No crashes, no degraded experience.

## Modding

All events are in `data/market_events.json`. Add your own triggers, change thresholds, adjust multipliers — no recompilation needed.
