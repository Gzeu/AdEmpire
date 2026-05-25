#pragma once
#include <string>
#include <ctime>

// ============================================================
//  MarketState.h  —  AdEmpire v1.0
//  Central struct populated by MarketFeed (async).
//  Read by MarketEventBridge + Dashboard + Newsfeed.
// ============================================================

struct MarketState {
    // ── Crypto ──────────────────────────────────────────────
    double btcPrice       = 0.0;
    double btcChange24h   = 0.0;   // percent, e.g. +5.2 or -3.1
    double ethPrice       = 0.0;
    double ethChange24h   = 0.0;
    double btcVolume      = 0.0;
    int    fearGreedIndex = 50;    // 0 (extreme fear) → 100 (extreme greed)
    double btcDominance   = 0.0;  // CoinGecko market cap dominance %

    // ── FX / Macro ──────────────────────────────────────────
    float  eurUsd  = 1.0850f;
    float  usdRon  = 4.6700f;

    // ── Sentiment ───────────────────────────────────────────
    std::string trendingKeyword;   // Wikipedia #1 trending
    float  newsSentiment = 0.0f;   // -1 bearish → +1 bullish

    // ── Derived (calculated by MarketFeed) ──────────────────
    float  globalVolatility = 0.0f;  // 0.0 – 1.0
    float  adMarketHealth   = 0.5f;  // 0.0 – 1.0 → campaign multiplier base

    // ── Status flags ────────────────────────────────────────
    bool   isOffline  = true;        // true = all fetches failed, using Neutral()
    bool   isStale    = true;        // true = fetchedAt > 5 min ago
    time_t fetchedAt  = 0;           // epoch of last successful FetchAll()

    // Neutral safe defaults — used when all sources are offline
    static MarketState Neutral() {
        MarketState s;
        s.btcPrice         = 95000.0;
        s.btcChange24h     = 0.0;
        s.ethPrice         = 3200.0;
        s.ethChange24h     = 0.0;
        s.fearGreedIndex   = 50;
        s.btcDominance     = 55.0;
        s.eurUsd           = 1.0850f;
        s.usdRon           = 4.6700f;
        s.globalVolatility = 0.0f;
        s.adMarketHealth   = 0.5f;
        s.trendingKeyword  = "";
        s.newsSentiment    = 0.0f;
        s.isOffline        = true;
        s.isStale          = true;
        s.fetchedAt        = 0;
        return s;
    }

    // Returns seconds since last successful fetch (or -1 if never fetched)
    double AgeSecs() const {
        if (fetchedAt == 0) return -1.0;
        return difftime(time(nullptr), fetchedAt);
    }
};
