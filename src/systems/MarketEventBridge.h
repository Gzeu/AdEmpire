#pragma once
#include "../network/MarketState.h"
#include "EventSystem.h"
#include <string>
#include <vector>
#include <functional>
#include <chrono>

// MarketEventBridge — connects live MarketState to EventSystem triggers
// Moved to systems/ (v0.8 refactor) — semantic home is here, not network/
// Call MarketEventBridge::Get().Evaluate(state, gameDay) after each FetchAll()
// and once per game "Next Month" tick.

struct MarketRule {
    std::string eventId;        // matches key in custom_events.json
    std::string description;    // human-readable log
    std::function<bool(const MarketState&)> condition;
    float cooldownDays = 30.0f; // minimum real days between same trigger
    float lastTriggeredDay = -999.0f;
};

class MarketEventBridge {
public:
    static MarketEventBridge& Get() {
        static MarketEventBridge instance;
        return instance;
    }

    MarketEventBridge() { BuildRules(); }

    // Call once per game month — returns triggered event IDs
    std::vector<std::string> Evaluate(const MarketState& state, float currentGameDay) {
        std::vector<std::string> triggered;
        for (auto& rule : m_rules) {
            if (currentGameDay - rule.lastTriggeredDay < rule.cooldownDays) continue;
            if (rule.condition(state)) {
                triggered.push_back(rule.eventId);
                rule.lastTriggeredDay = currentGameDay;
                m_log.push_back("[MarketBridge] Triggered: " + rule.eventId + " — " + rule.description);
            }
        }
        return triggered;
    }

    // Global multiplier derived from market health — apply to all campaign revenues
    float GetRevenueMultiplier(const MarketState& state) const {
        float mult = 1.0f;
        if (state.fearGreedIndex > 70)    mult += 0.15f;
        if (state.btcChange24h > 5.0f)    mult += 0.08f;
        if (state.newsSentiment > 0.5f)   mult += 0.10f;
        if (state.eurUsd > 0.0f && state.eurUsd < 1.05f) mult += 0.05f;
        if (state.fearGreedIndex < 25)    mult -= 0.20f;
        if (state.btcChange24h < -8.0f)   mult -= 0.15f;
        if (state.newsSentiment < -0.5f)  mult -= 0.12f;
        if (state.globalVolatility > 0.8f) mult -= 0.10f;
        return std::max(0.3f, std::min(2.0f, mult)); // clamp 30%-200%
    }

    // AI rival aggressiveness — higher when market is fearful
    float GetRivalAggressiveness(const MarketState& state) const {
        float base = 1.0f;
        if (state.fearGreedIndex < 30)    base += 0.25f;
        if (state.fearGreedIndex > 75)    base -= 0.15f;
        if (state.globalVolatility > 0.7f) base += 0.10f;
        return std::max(0.5f, std::min(1.8f, base));
    }

    const std::vector<std::string>& GetLog() const { return m_log; }
    void ClearLog() { m_log.clear(); }

private:
    std::vector<MarketRule> m_rules;
    std::vector<std::string> m_log;

    void BuildRules() {
        // === CRYPTO ===
        m_rules.push_back({"crypto_winter",    "BTC dropped >8% — crypto clients cut budgets",        [](const MarketState& s){ return s.btcChange24h < -8.0f; },  20.0f});
        m_rules.push_back({"crypto_bull_run",  "BTC surged >8% — crypto clients spend aggressively",  [](const MarketState& s){ return s.btcChange24h > 8.0f; },   20.0f});
        m_rules.push_back({"altcoin_season",   "ETH outperforms BTC+5% — altcoin ad spend up",        [](const MarketState& s){ return s.ethChange24h > s.btcChange24h + 5.0f; }, 30.0f});

        // === SENTIMENT ===
        m_rules.push_back({"market_panic",     "Fear & Greed <25 — all budgets -20%",                 [](const MarketState& s){ return s.fearGreedIndex < 25; },   15.0f});
        m_rules.push_back({"market_euphoria",  "Fear & Greed >80 — premium clients open contracts",   [](const MarketState& s){ return s.fearGreedIndex > 80; },   15.0f});
        m_rules.push_back({"market_neutral",   "F&G 45-55 — negotiation bonuses active",              [](const MarketState& s){ return s.fearGreedIndex >= 45 && s.fearGreedIndex <= 55; }, 60.0f});

        // === NEWS / TRENDS ===
        m_rules.push_back({"ai_hype_wave",     "AI/LLM trending — tech clients want AI campaigns",
            [](const MarketState& s){
                const auto& kw = s.trendingKeyword;
                return kw.find("AI") != std::string::npos || kw.find("LLM") != std::string::npos ||
                       kw.find("GPT") != std::string::npos || kw.find("machine learning") != std::string::npos;
            }, 45.0f});
        m_rules.push_back({"economic_downturn","Negative sentiment <-0.5 — recession fears, -15%",    [](const MarketState& s){ return s.newsSentiment < -0.5f; },  30.0f});
        m_rules.push_back({"marketing_boom",   "Marketing/viral trending — bonus contracts up",
            [](const MarketState& s){
                const auto& kw = s.trendingKeyword;
                return kw.find("marketing") != std::string::npos || kw.find("branding") != std::string::npos ||
                       kw.find("viral") != std::string::npos;
            }, 40.0f});

        // === FX / MACRO ===
        m_rules.push_back({"strong_euro_bonus","EUR/USD >1.12 — European clients budget boost",        [](const MarketState& s){ return s.eurUsd > 1.12f && s.eurUsd > 0.0f; }, 60.0f});
        m_rules.push_back({"usd_dominance",    "EUR/USD <0.98 — non-USD contracts penalized",          [](const MarketState& s){ return s.eurUsd < 0.98f && s.eurUsd > 0.0f; }, 60.0f});

        // === VOLATILITY ===
        m_rules.push_back({"market_volatility","Volatility >0.75 — outcomes ±25% random swing",        [](const MarketState& s){ return s.globalVolatility > 0.75f; }, 10.0f});
    }
};
