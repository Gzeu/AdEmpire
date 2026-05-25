# AdEmpire — Polish & Integration Log

## v1.0-rc  (commit: push masiv polish)

### New files
| File | What it does |
|---|---|
| `src/ui/LiveMarketPanel.h` | Real-time panel: BTC/ETH prices, Fear & Greed gauge, FX rates, ad health bar, live headlines, campaign ROI modifier |
| `src/network/NewsFeed.h` | Pulls CoinDesk RSS, Reddit RSS (r/marketing, r/digitalmarketing), Hacker News top 10 + AI hype score |
| `src/network/MarketState.h` | Central struct for all market indicators with `Neutral()` offline fallback |
| `src/systems/MarketEventBridge.h` | `GetDominantEventId()`, `EvaluateTriggers()`, `GetCampaignMultiplier()`, `GetMarketSummary()` |

### Updated files
| File | Changes |
|---|---|
| `src/ui/Dashboard.cpp` | Full rewrite with `BeginTabBar`: Overview / Channels / Live Market. KPI cards, Revenue chart, Market Share chart. Channel ROI adjusted by live market multiplier. |
| `src/ui/Newsfeed.cpp` | Full rewrite: live event banner, 3 tabs (Headlines / Reddit / HN), ticker strip at bottom. |

### Integration checklist

Add to `main.cpp`:
```cpp
// includes
#include "network/MarketFeed.h"
#include "network/NewsFeed.h"
#include "systems/MarketEventBridge.h"

// after ImGui::CreateContext()
MarketFeed::Get().StartAsync();
NewsFeed::Get().StartAsync();

// in Next Month tick
const MarketState& ms = MarketFeed::Get().GetState();
MarketEventBridge::EvaluateTriggers(ms, [&](const std::string& id){
    eventSystem.TriggerEventById(id);
});
float roiMult = MarketEventBridge::GetCampaignMultiplier(ms);
```

### Live data sources (zero API keys)

| Source | Endpoint | Data |
|---|---|---|
| Binance | `api.binance.com/api/v3/ticker/24hr?symbol=BTCUSDT` | BTC/ETH price + 24h change |
| CoinGecko | `api.coingecko.com/api/v3/global` | BTC dominance, market cap |
| Fear & Greed | `api.alternative.me/fng/` | 0–100 index |
| CoinDesk RSS | `www.coindesk.com/arc/outboundfeeds/rss/` | Crypto headlines |
| Reddit RSS | `reddit.com/r/marketing.rss` + `reddit.com/r/digitalmarketing.rss` | Marketing trends |
| Hacker News | `hacker-news.firebaseio.com/v0/topstories.json` | Tech + AI sentiment |
| Open Exchange Rates | `open.er-api.com/v6/latest/USD` | EUR/USD, USD/RON |
| Wikipedia Trending | `en.wikipedia.org/w/api.php?action=query&list=mostviewed` | Global trending keyword |
