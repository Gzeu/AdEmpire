#pragma once
#include "../network/MarketState.h"
#include <string>
#include <vector>
#include <functional>

// ============================================================
//  MarketEventBridge
//  Reads MarketState and maps it to AdEmpire event IDs.
//  Call EvaluateTriggers() at each "Next Month" tick.
//  Returns list of event IDs to fire through EventSystem.
// ============================================================

struct MarketEventBridge {
    using FireFn = std::function<void(const std::string&)>;

    // triggerFn: callback that fires an event by ID string
    // (wrap your EventSystem::TriggerEvent here)
    static void EvaluateTriggers(const MarketState& ms, FireFn triggerFn) {
        // ---- Crypto events ------------------------------------
        if (ms.cryptoBullActive)
            triggerFn("crypto_bull_run");       // in custom_events.json
        if (ms.cryptoBearActive)
            triggerFn("crypto_winter");
        if (ms.marketPanicActive)
            triggerFn("market_panic");

        // ---- Ad market health --------------------------------
        if (ms.adMarketHealth > 1.3f)
            triggerFn("ad_market_boom");
        else if (ms.adMarketHealth < 0.7f)
            triggerFn("ad_budgets_cut");

        // ---- AI hype wave ------------------------------------
        if (ms.aiHypeActive)
            triggerFn("ai_hype_wave");          // tech client bonus

        // ---- Tech boom (AI + crypto bull) --------------------
        if (ms.techBoomActive)
            triggerFn("tech_sector_boom");

        // ---- Trending keyword bonus --------------------------
        if (!ms.trendingKeyword.empty()) {
            // Any campaign targeting the trending keyword gets +25% ROI
            // EventSystem picks this up via trendingKeyword field
            triggerFn("trending_topic_active");
        }

        // ---- Volatility amplifier ----------------------------
        // High volatility: random events fire more often
        // Implemented in EventSystem — read globalVolatility directly
    }

    // Multiplier to apply on campaign ROI at Next Month
    // Usage: finalROI = baseROI * GetCampaignMultiplier(state)
    static float GetCampaignMultiplier(const MarketState& ms) {
        float m = ms.adMarketHealth;              // 0.5 – 1.5
        if (ms.aiHypeActive) m += 0.08f;          // tech ad demand spike
        if (ms.cryptoBullActive) m += 0.05f;      // consumer confidence
        if (ms.cryptoBearActive) m -= 0.08f;      // ad budget cuts
        if (ms.marketPanicActive) m -= 0.15f;     // fear = no spending
        return std::max(0.4f, std::min(1.6f, m));
    }

    // Short human-readable market summary for Dashboard UI
    static std::string GetMarketSummary(const MarketState& ms) {
        if (ms.isOffline) return "Market data offline — using neutral values.";
        std::string s;
        s += "BTC " + std::string(ms.btc.change24h >= 0 ? "+" : "")
           + std::to_string((int)ms.btc.change24h) + "% | ";
        s += "F&G " + std::to_string(ms.fearGreedIndex)
           + " (" + ms.fearGreedLabel + ") | ";
        s += "AdHealth " + std::to_string((int)(ms.adMarketHealth * 100)) + "%";
        if (ms.aiHypeActive)     s += " | AI HYPE";
        if (ms.cryptoBullActive) s += " | BULL";
        if (ms.cryptoBearActive) s += " | BEAR";
        if (ms.marketPanicActive)s += " | PANIC";
        return s;
    }
};
