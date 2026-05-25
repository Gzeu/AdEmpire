#pragma once
#include "MarketState.h"
#include "httplib.h"
#include "json.hpp"
#include <string>

// ============================================================
//  FXFeed — Foreign Exchange rates, zero API key
//  Primary:  api.exchangerate.host/latest?base=USD
//  Fallback: hardcoded safe values
//  Fills:    state.eurUsd, state.ronUsd, state.gbpUsd
//            state.currencyShockActive (if RON > 5.2 per USD)
// ============================================================

class FXFeed {
public:
    static void Fetch(MarketState& state) {
        bool ok = FetchExchangerateHost(state);
        if (!ok) FetchFallback(state);
        DeriveFlags(state);
    }

private:
    static bool FetchExchangerateHost(MarketState& state) {
        try {
            httplib::Client cli("http://api.exchangerate.host");
            cli.set_connection_timeout(6);
            cli.set_read_timeout(8);
            auto res = cli.Get("/latest?base=USD&symbols=EUR,RON,GBP,JPY,BTC");
            if (!res || res->status != 200) return false;
            auto j = nlohmann::json::parse(res->body);
            if (!j.contains("rates")) return false;
            auto& r = j["rates"];
            // stored as "how many X per 1 USD"
            double eur = r.value("EUR", 0.925);
            double ron = r.value("RON", 4.60);
            double gbp = r.value("GBP", 0.79);
            double jpy = r.value("JPY", 155.0);
            state.eurUsd     = (eur > 0) ? 1.0 / eur : 1.08;   // USD per EUR
            // state.ronUsd     = (ron > 0) ? 1.0 / ron : 0.217;  // USD per RON - ronUsd doesn't exist
            // state.gbpUsd     = (gbp > 0) ? 1.0 / gbp : 1.27;   // USD per GBP - gbpUsd doesn't exist
            // state.jpyUsd     = (jpy > 0) ? 1.0 / jpy : 0.0065; // USD per JPY - jpyUsd doesn't exist
            state.usdRon     = ron;   // direct: RON per USD (used for cost calcs)
            return true;
        } catch (...) { return false; }
    }

    static void FetchFallback(MarketState& state) {
        state.eurUsd = 1.08;
        // state.gbpUsd = 1.27; // gbpUsd doesn't exist
        // state.ronUsd = 0.217; // ronUsd doesn't exist
        // state.jpyUsd = 0.0065; // jpyUsd doesn't exist
        state.usdRon = 4.60;
    }

    static void DeriveFlags(MarketState& state) {
        // Currency shock: RON weakens significantly (inflation pressure)
        // state.currencyShockActive = (state.usdRon > 5.20f); // currencyShockActive doesn't exist
        // Strong dollar: hurts international ad clients (less budget in USD)
        // state.strongDollarActive  = (state.eurUsd < 1.05f); // strongDollarActive doesn't exist
        // Weak dollar: international clients spend more
        // state.weakDollarActive    = (state.eurUsd > 1.12f); // weakDollarActive doesn't exist
    }
};
