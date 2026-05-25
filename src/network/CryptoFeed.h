#pragma once
#include "MarketState.h"
#include "httplib.h"      // cpp-httplib
#include "json.hpp"       // nlohmann already in repo
#include <string>
#include <sstream>

// ============================================================
//  CryptoFeed
//  Sources:
//    Binance Public   — BTC/ETH price, 24h change, volume
//    alternative.me   — Fear & Greed Index
//    CoinGecko Free   — BTC dominance, global market data
//  Zero API key required.
// ============================================================

class CryptoFeed {
public:
    // Fill btc, eth, fearGreedIndex, btcDominance into state
    // Returns true if at least BTC was fetched successfully
    static bool Fetch(MarketState& state) {
        bool ok = false;
        ok |= FetchBinanceTicker(state, "BTCUSDT");
        ok |= FetchBinanceTicker(state, "ETHUSDT");
        FetchFearGreed(state);
        FetchCoinGeckoGlobal(state);
        DeriveFlags(state);
        return ok;
    }

private:
    // ----------------------------------------------------------
    static bool FetchBinanceTicker(MarketState& state, const std::string& symbol) {
        try {
            httplib::Client cli("https://api.binance.com");
            cli.set_connection_timeout(4);
            cli.set_read_timeout(4);
            auto res = cli.Get(("/api/v3/ticker/24hr?symbol=" + symbol).c_str());
            if (!res || res->status != 200) return false;
            auto j = nlohmann::json::parse(res->body);
            
            if (symbol == "BTCUSDT") {
                state.btcPrice = std::stod(j["lastPrice"].get<std::string>());
                state.btcChange24h = std::stod(j["priceChangePercent"].get<std::string>());
                state.btcVolume = std::stod(j["quoteVolume"].get<std::string>());
            } else if (symbol == "ETHUSDT") {
                state.ethPrice = std::stod(j["lastPrice"].get<std::string>());
                state.ethChange24h = std::stod(j["priceChangePercent"].get<std::string>());
            }
            return true;
        } catch (...) { return false; }
    }

    // ----------------------------------------------------------
    static void FetchFearGreed(MarketState& state) {
        try {
            httplib::Client cli("https://api.alternative.me");
            cli.set_connection_timeout(4);
            cli.set_read_timeout(4);
            auto res = cli.Get("/fng/?limit=1&format=json");
            if (!res || res->status != 200) return;
            auto j = nlohmann::json::parse(res->body);
            auto& d = j["data"][0];
            state.fearGreedIndex = std::stoi(d["value"].get<std::string>());
            // state.fearGreedLabel = d["value_classification"].get<std::string>(); // fearGreedLabel doesn't exist in MarketState
        } catch (...) {
            state.fearGreedIndex = 50;
            // state.fearGreedLabel = "Neutral"; // fearGreedLabel doesn't exist in MarketState
        }
    }

    // ----------------------------------------------------------
    static void FetchCoinGeckoGlobal(MarketState& state) {
        try {
            httplib::Client cli("https://api.coingecko.com");
            cli.set_connection_timeout(5);
            cli.set_read_timeout(5);
            auto res = cli.Get("/api/v3/global");
            if (!res || res->status != 200) return;
            auto j = nlohmann::json::parse(res->body);
            auto& pct = j["data"]["market_cap_percentage"];
            if (pct.contains("btc"))
                state.btcDominance = pct["btc"].get<double>();
        } catch (...) { state.btcDominance = 55.0; }
    }

    // ----------------------------------------------------------
    static void DeriveFlags(MarketState& state) {
        // Note: These flags don't exist in MarketState, commenting out for now
        // state.cryptoBullActive  = (state.btcChange24h >  8.0);
        // state.cryptoBearActive  = (state.btcChange24h < -8.0);
        // state.marketPanicActive = (state.fearGreedIndex < 20);
    }
};
