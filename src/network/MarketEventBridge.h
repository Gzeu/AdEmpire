#pragma once
#include "MarketState.h"
#include <string>
#include <vector>
#include <functional>

// ============================================================
//  MarketEventBridge
//  Reads fully-populated MarketState and maps signals to
//  AdEmpire game events (string IDs from market_events.json).
//  Call EvaluateTriggers() once per "Next Month" tick.
// ============================================================

struct MarketEventTrigger {
    std::string eventId;
    bool        triggered = false;
};

class MarketEventBridge {
public:
    // Returns list of event IDs to fire this month
    static std::vector<std::string> EvaluateTriggers(const MarketState& s) {
        std::vector<std::string> fired;
        auto fire = [&](bool cond, const std::string& id) {
            if (cond) fired.push_back(id);
        };

        // ---- CRYPTO triggers --------------------------------
        fire(s.btc.change24h < -8.0f,                        "crypto_winter");
        fire(s.btc.change24h >  8.0f,                        "crypto_bull_run");
        fire(s.fearGreedIndex < 20,                          "extreme_fear");
        fire(s.fearGreedIndex > 80,                          "extreme_greed");
        fire(s.cryptoBearActive && s.fearGreedIndex < 30,    "crypto_panic");
        fire(s.btc.volume > 50000000000.0,                   "high_btc_volume");

        // ---- MACRO / FX triggers ----------------------------
        fire(s.currencyShockActive,                          "currency_shock");
        fire(s.strongDollarActive,                           "strong_dollar");
        fire(s.weakDollarActive,                             "weak_dollar");
        fire(s.recessionRiskActive,                          "economic_downturn");
        fire(s.geopoliticalRisk,                             "geopolitical_tension");

        // ---- AI / TECH triggers -----------------------------
        fire(s.aiHypeActive && s.aiHypeScore > 0.65f,        "ai_hype_wave");
        fire(s.aiHypeActive && s.wikiAiScore > 0.5f,         "ai_mainstream");
        fire(s.adtechBoomActive,                             "adtech_boom");
        fire(s.techBoomActive,                               "tech_sector_boom");

        // ---- SOCIAL / SENTIMENT triggers --------------------
        fire(s.newsSentiment > 0.4f,                         "positive_market_mood");
        fire(s.newsSentiment < -0.4f,                        "negative_market_mood");
        fire(s.cryptoSentiment > 0.5f,                       "crypto_community_fomo");
        fire(s.macroSentiment < -0.5f,                       "macro_pessimism");
        fire(s.aiHypeScore > 0.8f,                           "ai_saturation");

        // ---- COMBINED / COMPOUND triggers -------------------
        // Tech boom + crypto bull = goldilocks for ad spend
        fire(s.cryptoBullActive && s.aiHypeActive &&
             s.adMarketHealth > 1.2f,                        "golden_quarter");
        // Bear market + recession fear = budget cuts
        fire(s.cryptoBearActive && s.recessionRiskActive,    "budget_freeze");
        // Strong dollar + geopolitical = international clients pull back
        fire(s.strongDollarActive && s.geopoliticalRisk,     "international_pullback");
        // Extreme greed + adtech boom = premium pricing possible
        fire(s.fearGreedIndex > 75 && s.adtechBoomActive,    "premium_ad_pricing");

        // ---- AD MARKET HEALTH override ----------------------
        fire(s.adMarketHealth > 1.3f,                        "ad_market_hot");
        fire(s.adMarketHealth < 0.7f,                        "ad_market_cold");

        return fired;
    }

    // Human-readable summary for Dashboard "Live Market" tab
    static std::string GetMarketSummary(const MarketState& s) {
        std::string out;
        out += "BTC: " + std::to_string((int)s.btc.price) + " USD";
        out += "  |  24h: " + (s.btc.change24h >= 0 ? "+" : "") +
               std::to_string((int)(s.btc.change24h * 10) / 10.0f) + "%";
        out += "  |  F&G: " + std::to_string(s.fearGreedIndex);
        out += "  |  AdHealth: " +
               std::to_string((int)(s.adMarketHealth * 100)) + "%";
        if (!s.trendingKeyword.empty())
            out += "  |  Trending: " + s.trendingKeyword;
        return out;
    }
};
