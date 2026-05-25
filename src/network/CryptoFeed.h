#pragma once
#include "MarketState.h"
#include "httplib.h"
#include "json.hpp"
#include <string>
#include <sstream>

// ============================================================
//  CryptoFeed
//  Sources (all HTTPS, zero API key):
//    Binance Public   — BTC/ETH price, 24h change, volume
//    alternative.me   — Fear & Greed Index
//    CoinGecko Free   — BTC dominance, global market data
// ============================================================

class CryptoFeed {
public:
    static bool Fetch(MarketState& state) {
        bool ok = false;
        ok |= FetchBinanceTicker(state, "BTCUSDT");
        ok |= FetchBinanceTicker(state, "ETHUSDT");
        FetchFearGreed(state);
        FetchCoinGeckoGlobal(state);
        return ok;
    }

private:
    static bool FetchBinanceTicker(MarketState& state, const std::string& symbol) {
        try {
            httplib::SSLClient cli("api.binance.com", 443);
            cli.set_connection_timeout(5);
            cli.set_read_timeout(5);
            cli.enable_server_certificate_verification(true);
            auto res = cli.Get(("/api/v3/ticker/24hr?symbol=" + symbol).c_str());
            if (!res || res->status != 200) return false;
            auto j = nlohmann::json::parse(res->body);
            if (symbol == "BTCUSDT") {
                state.btcPrice     = std::stod(j["lastPrice"].get<std::string>());
                state.btcChange24h = std::stod(j["priceChangePercent"].get<std::string>());
                state.btcVolume    = std::stod(j["quoteVolume"].get<std::string>());
            } else if (symbol == "ETHUSDT") {
                state.ethPrice     = std::stod(j["lastPrice"].get<std::string>());
                state.ethChange24h = std::stod(j["priceChangePercent"].get<std::string>());
            }
            return true;
        } catch (...) { return false; }
    }

    static void FetchFearGreed(MarketState& state) {
        try {
            httplib::SSLClient cli("api.alternative.me", 443);
            cli.set_connection_timeout(5);
            cli.set_read_timeout(5);
            cli.enable_server_certificate_verification(true);
            auto res = cli.Get("/fng/?limit=1&format=json");
            if (!res || res->status != 200) return;
            auto j = nlohmann::json::parse(res->body);
            state.fearGreedIndex = std::stoi(j["data"][0]["value"].get<std::string>());
        } catch (...) {
            state.fearGreedIndex = 50;
        }
    }

    static void FetchCoinGeckoGlobal(MarketState& state) {
        try {
            httplib::SSLClient cli("api.coingecko.com", 443);
            cli.set_connection_timeout(6);
            cli.set_read_timeout(6);
            cli.enable_server_certificate_verification(true);
            auto res = cli.Get("/api/v3/global");
            if (!res || res->status != 200) return;
            auto j = nlohmann::json::parse(res->body);
            auto& pct = j["data"]["market_cap_percentage"];
            if (pct.contains("btc"))
                state.btcDominance = pct["btc"].get<double>();
        } catch (...) { state.btcDominance = 55.0; }
    }
};
