#pragma once
#include "MarketState.h"
#include "../vendor/httplib.h"      // cpp-httplib already in repo
#include "../vendor/json.hpp"       // nlohmann already in repo
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
        ok |= FetchBinanceTicker(state, "BTCUSDT", state.btc);
        ok |= FetchBinanceTicker(state, "ETHUSDT", state.eth);
        FetchFearGreed(state);
        FetchCoinGeckoGlobal(state);
        DeriveFlags(state);
        return ok;
    }

private:
    // ----------------------------------------------------------
    static bool FetchBinanceTicker(MarketState& /*s*/, const std::string& symbol, CryptoTick& tick) {
        try {
            httplib::Client cli("https://api.binance.com");
            cli.set_connection_timeout(4);
            cli.set_read_timeout(4);
            auto res = cli.Get(("/api/v3/ticker/24hr?symbol=" + symbol).c_str());
            if (!res || res->status != 200) return false;
            auto j = nlohmann::json::parse(res->body);
            tick.symbol    = symbol;
            tick.price     = std::stod(j["lastPrice"].get<std::string>());
            tick.change24h = std::stod(j["priceChangePercent"].get<std::string>());
            tick.volume24h = std::stod(j["quoteVolume"].get<std::string>());
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
            state.fearGreedLabel = d["value_classification"].get<std::string>();
        } catch (...) {
            state.fearGreedIndex = 50;
            state.fearGreedLabel = "Neutral";
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
        state.cryptoBullActive  = (state.btc.change24h >  8.0);
        state.cryptoBearActive  = (state.btc.change24h < -8.0);
        state.marketPanicActive = (state.fearGreedIndex < 20);
    }
};
