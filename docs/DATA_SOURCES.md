# AdEmpire — Data Sources Reference

> Every market signal that powers AdEmpire's live simulation.
> All sources are **zero API key, zero account required** unless noted.

---

## Architecture Overview

```
Live Internet
    │
    ├── CryptoFeed.h    → Binance + alternative.me/fng
    ├── FXFeed.h        → exchangerate.host
    ├── NewsFeed.h      → CoinDesk RSS + Reuters RSS
    ├── SocialPulseFeed.h → Reddit RSS + HackerNews Firebase
    ├── WikiTrendFeed.h → Wikipedia Most Viewed API
    └── TrendFeed.h     → orchestrator + adMarketHealth derivation
            │
            ▼
        MarketState (unified struct)
            │
            ▼
    MarketEventBridge.h → evaluates triggers
            │
            ▼
    EventSystem.cpp → fires events from data/market_events.json
```

**Update cadence:** All feeds fetched once per real-time session start, then cached for the session. Optionally re-fetch every 30 real-minutes if the game window stays open.

---

## Source 1 — Binance Public API

| Field | Value |
|---|---|
| **Endpoint** | `https://api.binance.com/api/v3/ticker/24hr?symbol=BTCUSDT` |
| **Auth** | None |
| **Rate limit** | 1200 requests/minute (weight 1) |
| **File** | `src/network/CryptoFeed.h` |
| **Fills** | `btc.price`, `btc.change24h`, `btc.volume` |

```json
{ "lastPrice": "67420.00", "priceChangePercent": "-3.21", "volume": "28340.5" }
```

**Fallback:** If unreachable, `btc.change24h = 0.0`, `btc.price = 65000.0` (neutral).

---

## Source 2 — Fear & Greed Index

| Field | Value |
|---|---|
| **Endpoint** | `https://api.alternative.me/fng/?limit=1` |
| **Auth** | None |
| **Rate limit** | Unlimited public |
| **File** | `src/network/CryptoFeed.h` |
| **Fills** | `fearGreedIndex` (0 = Extreme Fear, 100 = Extreme Greed) |

```json
{ "data": [{ "value": "42", "value_classification": "Fear" }] }
```

**Fallback:** `fearGreedIndex = 50` (neutral).

---

## Source 3 — Exchange Rates (FX)

| Field | Value |
|---|---|
| **Endpoint** | `https://api.exchangerate.host/latest?base=USD&symbols=EUR,RON,GBP,JPY` |
| **Auth** | None |
| **Rate limit** | ~250 requests/month free tier |
| **File** | `src/network/FXFeed.h` |
| **Fills** | `eurUsd`, `gbpUsd`, `ronUsd`, `usdRon` |

**Game impact:**
- `usdRon > 5.20` → triggers `currency_shock` event
- `eurUsd < 1.05` → triggers `strong_dollar` event
- `eurUsd > 1.12` → triggers `weak_dollar` event

**Fallback:** `eurUsd=1.08`, `usdRon=4.60`, `gbpUsd=1.27`

---

## Source 4 — Reddit RSS Feeds

| Field | Value |
|---|---|
| **Endpoints** | `reddit.com/r/marketing.rss`, `/r/digitalmarketing.rss`, `/r/cryptocurrency.rss`, `/r/economics.rss` |
| **Auth** | None (public RSS) |
| **Rate limit** | ~60 req/min per IP |
| **File** | `src/network/SocialPulseFeed.h` |
| **Fills** | `newsSentiment`, `cryptoSentiment`, `macroSentiment`, `aiHypeScore`, `redditTrendingTopic` |

**Keyword scoring:**
- Bullish: `growth, surge, rally, boom, profit, launch, innovation, hiring`
- Bearish: `crash, recession, layoff, bankrupt, fraud, collapse, crisis`
- AI detector: `ai, gpt, llm, openai, claude, automation, agent`

---

## Source 5 — HackerNews Firebase API

| Field | Value |
|---|---|
| **Endpoint** | `https://hacker-news.firebaseio.com/v0/topstories.json` then `/v0/item/{id}.json` |
| **Auth** | None |
| **Rate limit** | Generous public API |
| **File** | `src/network/SocialPulseFeed.h` |
| **Fills** | `aiHypeScore` (tech sentiment component) |

**Logic:** Scans titles of top 5 stories for AI/tech keywords. Weighted 40% in final `newsSentiment`.

---

## Source 6 — Wikipedia Most Viewed

| Field | Value |
|---|---|
| **Endpoint** | `https://en.wikipedia.org/w/api.php?action=query&list=mostviewed&pvimlimit=20&format=json` |
| **Auth** | None |
| **Rate limit** | 200 req/sec aggregate (very generous) |
| **File** | `src/network/WikiTrendFeed.h` |
| **Fills** | `trendingKeyword`, `wikiAiScore`, `wikiCryptoScore`, `wikiMacroBearScore`, `wikiAdtechScore`, `wikiGeoScore` |

**Categories mapped:**

| Wikipedia topic | Signal | Game trigger |
|---|---|---|
| `artificial intelligence`, `OpenAI`, `ChatGPT` | `wikiAiScore` | `ai_hype_wave`, `ai_mainstream` |
| `Bitcoin`, `Ethereum`, `cryptocurrency` | `wikiCryptoScore` | combined with CryptoFeed |
| `recession`, `inflation`, `financial crisis` | `wikiMacroBearScore` | `economic_downturn` |
| `advertising`, `TikTok`, `social media` | `wikiAdtechScore` | `adtech_boom` |
| `war`, `sanctions`, `trade war` | `wikiGeoScore` | `geopolitical_tension` |

---

## Source 7 — News RSS Feeds

| Field | Value |
|---|---|
| **Endpoints** | CoinDesk, Reuters Tech RSS |
| **Auth** | None |
| **File** | `src/network/NewsFeed.h` |
| **Fills** | `latestHeadline`, `newsSentiment` (partial) |

---

## MarketState — Complete Field Reference

```cpp
struct MarketState {
    // CryptoFeed
    struct { double price; float change24h; double volume; } btc;
    struct { double price; float change24h; } eth;
    int   fearGreedIndex;       // 0-100
    bool  cryptoBullActive;     // btcChange24h > +5%
    bool  cryptoBearActive;     // btcChange24h < -5%
    bool  marketPanicActive;    // fearGreed < 25

    // FXFeed
    double eurUsd, gbpUsd, ronUsd, jpyUsd, usdRon;
    bool   currencyShockActive; // usdRon > 5.20
    bool   strongDollarActive;  // eurUsd < 1.05
    bool   weakDollarActive;    // eurUsd > 1.12

    // SocialPulseFeed
    float  newsSentiment;       // -1.0 to +1.0
    float  aiHypeScore;         // 0.0 to 1.0
    float  cryptoSentiment;     // -1.0 to +1.0
    float  macroSentiment;      // -1.0 to +1.0
    std::string redditTrendingTopic;

    // WikiTrendFeed
    std::string trendingKeyword;
    float  wikiAiScore, wikiCryptoScore, wikiMacroBearScore;
    float  wikiMacroBullScore, wikiAdtechScore, wikiGeoScore;

    // Derived (TrendFeed)
    float  adMarketHealth;      // 0.5 to 1.5, multiplicator direct
    float  globalVolatility;    // 0.0 to 1.0
    bool   aiHypeActive;        // aiHypeScore > 0.45
    bool   recessionRiskActive; // wikiMacroBearScore > 0.35
    bool   adtechBoomActive;
    bool   geopoliticalRisk;
    bool   techBoomActive;      // aiHype && cryptoBull

    // NewsFeed
    std::string latestHeadline;
};
```

---

## Adding a New Data Source

1. Create `src/network/YourFeed.h` — static `Fetch(MarketState& state)` method
2. Add new fields to `MarketState.h`
3. Call `YourFeed::Fetch(state)` in `MarketFeed.h` `FetchAll()`
4. Add triggers in `MarketEventBridge.h` `EvaluateTriggers()`
5. Add event definitions in `data/market_events.json`
6. Document the source in this file

---

## Offline / Fallback Behavior

Every feed has a `catch(...)` block that sets safe neutral values:
- BTC change = 0%, Fear&Greed = 50, EUR/USD = 1.08
- All sentiment scores = 0.0 (neutral)
- Trending keyword = empty string
- `adMarketHealth` falls back to `1.0` (no multiplier)

The game is fully playable offline — real data is an enhancement, not a requirement.
