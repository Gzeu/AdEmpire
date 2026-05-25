#pragma once
#include <string>
#include <vector>
#include <ctime>

// ============================================================
//  MarketState — unified struct fed by all market data sources
//  Updated async every 5 minutes. EventSystem reads this.
// ============================================================

struct CryptoTick {
    std::string symbol;          // "BTCUSDT", "ETHUSDT"
    double      price;
    double      change24h;       // percent, e.g. -4.2
    double      volume24h;
};

struct MarketState {
    // ---- Crypto ------------------------------------------------
    CryptoTick  btc;             // from Binance
    CryptoTick  eth;
    int         fearGreedIndex;  // 0-100 (alternative.me/fng)
    std::string fearGreedLabel;  // "Extreme Fear" … "Extreme Greed"
    double      btcDominance;    // from CoinGecko global

    // ---- FX / Macro -------------------------------------------
    double      eurUsd;          // open exchange rates
    double      ronUsd;

    // ---- News sentiment ---------------------------------------
    std::vector<std::string> headlines;     // latest 5 real headlines
    std::string trendingKeyword;            // top Wikipedia/HN keyword
    float       newsSentiment;              // -1.0 bearish → +1.0 bullish
    bool        aiHypeActive;               // HN: "AI" appears > 3 top stories
    bool        cryptoBearActive;           // BTC change24h < -8%
    bool        cryptoBullActive;           // BTC change24h > +8%
    bool        marketPanicActive;          // fearGreed < 20
    bool        techBoomActive;             // HN AI hype + BTC bull

    // ---- Derived game multipliers ----------------------------
    float       adMarketHealth;  // 0.5 – 1.5 — multiplier pe campaign ROI
    float       globalVolatility;// 0.0 – 1.0 — event trigger amplifier

    // ---- Meta -------------------------------------------------
    time_t      fetchedAt;
    bool        isStale;         // true if > 5 min since fetch
    bool        isOffline;       // true if all fetches failed — uses safe defaults

    // Safe defaults when offline
    static MarketState Neutral() {
        MarketState s{};
        s.btc = {"BTCUSDT", 65000.0, 0.0, 0.0};
        s.eth = {"ETHUSDT", 3200.0,  0.0, 0.0};
        s.fearGreedIndex = 50;
        s.fearGreedLabel = "Neutral";
        s.btcDominance   = 55.0;
        s.eurUsd  = 1.08;
        s.ronUsd  = 0.22;
        s.trendingKeyword  = "";
        s.newsSentiment    = 0.0f;
        s.aiHypeActive     = false;
        s.cryptoBearActive = false;
        s.cryptoBullActive = false;
        s.marketPanicActive= false;
        s.techBoomActive   = false;
        s.adMarketHealth   = 1.0f;
        s.globalVolatility = 0.2f;
        s.isOffline        = true;
        s.isStale          = false;
        s.fetchedAt        = time(nullptr);
        return s;
    }
};
