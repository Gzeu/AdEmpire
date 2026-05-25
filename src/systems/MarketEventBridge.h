#pragma once
#include "../network/MarketState.h"
#include <string>
#include <functional>
#include <vector>

// ============================================================
//  MarketEventBridge.h  —  AdEmpire v1.0
//  Maps live MarketState → event IDs + campaign multipliers.
//  Called in GameLoop at every "Next Month" tick.
// ============================================================

namespace MarketEventBridge {

// Returns the single most dominant event ID given current state.
// Returns empty string if no event is active.
inline std::string GetDominantEventId(const MarketState& ms) {
    // Priority order: highest impact first
    if (ms.btcChange24h <= -12.0) return "crypto_winter";
    if (ms.btcChange24h >=  12.0) return "crypto_bull_run";
    if (ms.fearGreedIndex <= 20)  return "market_panic";
    if (ms.adMarketHealth <= 0.4f) return "ad_budgets_cut";
    if (ms.adMarketHealth >= 0.85f) return "ad_market_boom";
    if (ms.globalVolatility >= 0.75f) return "market_panic";
    // AI hype: check trending keyword
    if (!ms.trendingKeyword.empty()) {
        std::string kw = ms.trendingKeyword;
        for (char& c : kw) c = (char)::tolower((unsigned char)c);
        if (kw.find("ai") != std::string::npos ||
            kw.find("gpt") != std::string::npos ||
            kw.find("openai") != std::string::npos)
            return "ai_hype_wave";
        if (kw.find("tech") != std::string::npos)
            return "tech_sector_boom";
    }
    return "";
}

// Evaluates ALL applicable events and fires callback for each.
// Call in Next Month tick:
//   MarketEventBridge::EvaluateTriggers(ms, [&](const std::string& id){ eventSystem.TriggerById(id); });
inline void EvaluateTriggers(const MarketState& ms,
                              std::function<void(const std::string&)> trigger) {
    if (ms.btcChange24h <= -8.0)   trigger("crypto_winter");
    if (ms.btcChange24h >=  8.0)   trigger("crypto_bull_run");
    if (ms.fearGreedIndex <= 20)    trigger("market_panic");
    if (ms.fearGreedIndex >= 75)    trigger("crypto_bull_run"); // greed = bull
    if (ms.adMarketHealth <= 0.40f) trigger("ad_budgets_cut");
    if (ms.adMarketHealth >= 0.85f) trigger("ad_market_boom");
    if (ms.globalVolatility >= 0.80f) trigger("market_panic");
    if (!ms.trendingKeyword.empty()) trigger("trending_topic_active");
}

// Campaign ROI multiplier — composite score from all live indicators.
// 1.0 = neutral, >1.0 = bull market boost, <1.0 = bear market penalty.
inline float GetCampaignMultiplier(const MarketState& ms) {
    float mult = 1.0f;

    // BTC momentum ±15%
    mult += (float)(ms.btcChange24h / 100.0) * 1.5f;

    // Fear & Greed: 50=neutral, 0=panic(-20%), 100=greed(+20%)
    mult += ((float)ms.fearGreedIndex - 50.f) / 250.f;

    // Ad market health: 0.5=neutral contribution
    mult += (ms.adMarketHealth - 0.5f) * 0.4f;

    // Volatility penalty
    mult -= ms.globalVolatility * 0.15f;

    // Clamp to reasonable bounds
    if (mult < 0.4f) mult = 0.4f;
    if (mult > 2.5f) mult = 2.5f;
    return mult;
}

// Human-readable summary for Dashboard tooltip / Report panel
inline std::string GetMarketSummary(const MarketState& ms) {
    char buf[256];
    snprintf(buf, sizeof(buf),
        "BTC %+.1f%% | F&G %d | AdHealth %.0f%% | Vol %.0f%% | Trend: %s",
        ms.btcChange24h,
        ms.fearGreedIndex,
        ms.adMarketHealth * 100.f,
        ms.globalVolatility * 100.f,
        ms.trendingKeyword.empty() ? "none" : ms.trendingKeyword.c_str());
    return std::string(buf);
}

} // namespace MarketEventBridge
