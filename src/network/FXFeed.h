#pragma once
#include "MarketState.h"
#include "httplib.h"
#include "json.hpp"
#include <string>

// ============================================================
//  FXFeed
//  Source: Open Exchange Rates (HTTPS, free tier, no API key
//          for latest.json when app_id is omitted — returns
//          USD-base rates only; good enough for EUR/USD/RON)
//  Fallback: hardcoded EUR/USD ~1.08, RON/USD ~0.22
// ============================================================

class FXFeed {
public:
    static void Fetch(MarketState& state) {
        try {
            // Frankfurter.app — 100% free, no key, ECB data, HTTPS
            httplib::SSLClient cli("api.frankfurter.app", 443);
            cli.set_connection_timeout(5);
            cli.set_read_timeout(5);
            cli.enable_server_certificate_verification(true);
            // Latest USD rates with EUR and RON
            auto res = cli.Get("/latest?from=USD&to=EUR,RON");
            if (!res || res->status != 200) { SetFallback(state); return; }
            auto j = nlohmann::json::parse(res->body);
            if (j.contains("rates")) {
                auto& r = j["rates"];
                if (r.contains("EUR")) state.eurUsd = 1.0 / r["EUR"].get<double>();
                if (r.contains("RON")) state.ronUsd = 1.0 / r["RON"].get<double>();
            }
        } catch (...) { SetFallback(state); }
    }

private:
    static void SetFallback(MarketState& state) {
        state.eurUsd = 1.085;
        state.ronUsd = 0.220;
    }
};
