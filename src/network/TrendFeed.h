#pragma once
#include "MarketState.h"
#include "httplib.h"
#include "json.hpp"
#include <string>
#include <vector>
#include <algorithm>

// ============================================================
//  TrendFeed
//  Sources (zero API key):
//    Wikipedia Most Viewed   — global trending topics
//    Open Exchange Rates     — EUR/USD/RON (free tier, zero card)
//  Fills: trendingKeyword, eurUsd, ronUsd
// ============================================================

class TrendFeed {
public:
    static void Fetch(MarketState& state) {
        FetchWikipediaTrending(state);
        FetchExchangeRates(state);
        DeriveAdHealth(state);
    }

private:
    static void FetchWikipediaTrending(MarketState& state) {
        try {
            httplib::Client cli("https://en.wikipedia.org");
            cli.set_connection_timeout(5); cli.set_read_timeout(5);
            // Most viewed pages yesterday
            auto res = cli.Get(
                "/w/api.php?action=query&list=mostviewed&pvimoffset=0"
                "&pvimlimit=10&format=json");
            if (!res || res->status != 200) return;
            auto j = nlohmann::json::parse(res->body);
            auto& pages = j["query"]["mostviewed"];
            // Filter out obvious noise (Main_Page, Special:*)
            static const std::vector<std::string> noiseWords = {
                "main_page","special:","wikipedia:","portal:"
            };
            for (auto& p : pages) {
                std::string title = p["title"].get<std::string>();
                std::string tl = title;
                std::transform(tl.begin(), tl.end(), tl.begin(), ::tolower);
                bool isNoise = false;
                for (auto& n : noiseWords)
                    if (tl.find(n) != std::string::npos) { isNoise = true; break; }
                if (!isNoise && !title.empty()) {
                    state.trendingKeyword = title;
                    break;
                }
            }
        } catch (...) { state.trendingKeyword = ""; }
    }

    static void FetchExchangeRates(MarketState& state) {
        try {
            // exchangerate.host — completely free, no key
            httplib::Client cli("https://api.exchangerate.host");
            cli.set_connection_timeout(5); cli.set_read_timeout(5);
            auto res = cli.Get("/latest?base=USD&symbols=EUR,RON");
            if (!res || res->status != 200) {
                state.eurUsd = 1.08; state.usdRon = 4.67; return;
            }
            auto j = nlohmann::json::parse(res->body);
            auto& rates = j["rates"];
            double eur = rates.contains("EUR") ? rates["EUR"].get<double>() : 0.925;
            double ron = rates.contains("RON") ? rates["RON"].get<double>() : 4.55;
            state.eurUsd = 1.0 / eur;   // USD per EUR
            state.usdRon = ron;   // RON per USD
        } catch (...) {
            state.eurUsd = 1.08;
            state.usdRon = 4.67;
        }
    }

    // adMarketHealth: combine crypto sentiment, news, fear/greed
    static void DeriveAdHealth(MarketState& state) {
        float score = 1.0f;
        // Fear & Greed contribution
        score += (state.fearGreedIndex - 50) / 200.0f;  // -0.25 to +0.25
        // News sentiment contribution
        score += state.newsSentiment * 0.15f;            // -0.15 to +0.15
        // Crypto panic penalty - these flags don't exist in MarketState
        // if (state.marketPanicActive)  score -= 0.20f;
        // if (state.cryptoBearActive)   score -= 0.15f;
        // if (state.cryptoBullActive)   score += 0.10f;
        // AI hype bonus — tech advertising demand spikes
        // if (state.aiHypeActive)       score += 0.12f;
        state.adMarketHealth   = std::max(0.5f, std::min(1.5f, score));
        // Volatility: how much events are amplified
        float vol = std::abs(state.btcChange24h) / 20.0f
                  + (1.0f - state.fearGreedIndex / 100.0f) * 0.3f
                  + std::abs(state.newsSentiment) * 0.2f;
        state.globalVolatility = std::max(0.0f, std::min(1.0f, vol));
        // Composite flags - these don't exist in MarketState
        // state.techBoomActive = state.aiHypeActive && state.cryptoBullActive;
    }
};
