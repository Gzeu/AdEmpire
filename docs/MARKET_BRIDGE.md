# MarketEventBridge — Live Market Integration

AdEmpire connects to real-world financial data and translates it into in-game events and modifiers.

## Architecture

```
Internet APIs
  Binance       → btcChange24h, ethChange24h
  alternative.me → fearGreedIndex
  Open FX       → eurUsd
  CoinDesk RSS  → headlines, newsSentiment
  Reddit RSS    → trendingKeyword (r/marketing, r/cryptocurrency)
  Hacker News   → trendingKeyword (AI/tech hype)
  Wikipedia     → trendingKeyword (global virals)
        ↓
  MarketFeed::FetchAll()   (async, every 5 minutes)
        ↓
  MarketState              (single unified struct)
        ↓
  MarketEventBridge::Evaluate()   (called each game month)
        ↓
  EventSystem::TriggerEvent()     → in-game consequences
```

## Triggered Events

| Event ID | Condition | Effect |
|---|---|---|  
| `crypto_winter` | BTC 24h < -8% | Crypto clients cut budgets 25% |
| `crypto_bull_run` | BTC 24h > +8% | Crypto clients spend aggressively |
| `altcoin_season` | ETH outperforms BTC by 5% | Altcoin projects open new contracts |
| `market_panic` | Fear & Greed < 25 | All clients reduce budgets 20% |
| `market_euphoria` | Fear & Greed > 80 | Premium clients open new contracts |
| `ai_hype_wave` | Trending: AI/LLM/GPT | Tech clients want AI-focused campaigns |
| `economic_downturn` | News sentiment < -0.5 | Recession fears, 15% budget cuts |
| `marketing_boom` | Trending: marketing/viral | Bonus contracts available |
| `strong_euro_bonus` | EUR/USD > 1.12 | European clients get budget boost |
| `usd_dominance` | EUR/USD < 0.98 | Non-USD contracts penalized |
| `market_volatility` | Global volatility > 0.75 | Campaign outcomes swing ±25% |
| `market_neutral` | Fear & Greed 45-55 | Negotiation bonuses active |

## Revenue Multiplier

`MarketEventBridge::GetRevenueMultiplier()` returns a float (0.3–2.0) applied to all campaign revenues:

- Fear & Greed > 70 → +15%
- BTC 24h > +5% → +8%
- Positive news → +10%
- Fear & Greed < 25 → -20%
- BTC 24h < -8% → -15%
- High volatility → -10%

## AI Rival Aggressiveness

`GetRivalAggressiveness()` modifies MediaBlaze/PeakBrands pitch rate:
- Extreme fear → rivals cut prices (+25% aggressiveness)
- Extreme greed → rivals raise prices (-15% aggressiveness)

## Integration in main.cpp

```cpp
// After ImGui::NewFrame() — refresh timer
static auto lastFetch = std::chrono::steady_clock::now();
auto now = std::chrono::steady_clock::now();
if (std::chrono::duration_cast<std::chrono::minutes>(now - lastFetch).count() >= 5) {
    MarketFeed::Get().FetchAsync();
    lastFetch = now;
}

// In GameEngine::NextMonth()
auto triggered = MarketEventBridge::Get().Evaluate(
    MarketFeed::Get().GetLatest(), m_currentGameDay);
for (auto& id : triggered)
    m_eventSystem.TriggerEvent(id);

float revMult = MarketEventBridge::Get().GetRevenueMultiplier(
    MarketFeed::Get().GetLatest());
m_campaignEngine.SetRevenueMultiplier(revMult);
```

## Dashboard Integration

In `Dashboard.cpp`, replace the current scroll panel with:

```cpp
if (ImGui::BeginTabBar("##dashboard_tabs")) {
    if (ImGui::BeginTabItem("Overview")) {
        // existing dashboard content
        ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Live Market")) {
        LiveMarketPanel::RenderLiveMarket();
        ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Newsfeed")) {
        LiveMarketPanel::RenderNewsfeed();
        ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
}
```
