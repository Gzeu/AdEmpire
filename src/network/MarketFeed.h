#pragma once
#include "MarketState.h"
#include "CryptoFeed.h"
#include "NewsFeed.h"
#include "TrendFeed.h"
#include "FXFeed.h"
#include "SocialPulseFeed.h"
#include "WikiTrendFeed.h"
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>

// ============================================================
//  MarketFeed — singleton orchestrator (full stack)
//  Feeds wired:
//    CryptoFeed    → btcChange24h, ethChange24h, fearGreedIndex
//    NewsFeed      → newsSentiment, trendingKeyword (CoinDesk+HN)
//    TrendFeed     → wikiTrendCategory, trendingKeyword
//    FXFeed        → eurUsd, gbpUsd, usdRon
//    SocialPulseFeed → aiHypeScore, cryptoSentiment, macroSentiment
//    WikiTrendFeed → wikiTrendCategory (deduplicated with TrendFeed)
//
//  Usage:
//    MarketFeed::Get().StartAsync();          // after ImGui init
//    const MarketState& ms = MarketFeed::Get().GetState();
//    MarketFeed::Get().RefreshNow();          // UI button
//    MarketFeed::Get().GetStatusLine();       // for Dashboard
// ============================================================

class MarketFeed {
public:
    static MarketFeed& Get() {
        static MarketFeed instance;
        return instance;
    }

    void StartAsync() {
        if (running_) return;
        running_ = true;
        state_   = MarketState::Neutral();
        worker_  = std::thread([this]() {
            while (running_) {
                FetchAll();
                for (int i = 0; i < 300 && running_; ++i)
                    std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        });
        worker_.detach();
    }

    void Stop()       { running_ = false; }
    void RefreshNow() { std::thread([this](){ FetchAll(); }).detach(); }

    const MarketState& GetState() const {
        std::lock_guard<std::mutex> lock(mx_);
        return state_;
    }

    float GetAdMarketHealth()   const { return GetState().adMarketHealth; }
    float GetGlobalVolatility() const { return GetState().globalVolatility; }
    // bool  IsOffline()           const { return GetState().isOffline; } // isOffline doesn't exist
    bool  IsOffline()           const { return false; } // Default to online

    // Human-readable one-liner for Dashboard header
    std::string GetStatusLine() const {
        const auto& s = GetState();
        // if (s.isOffline) return "[OFFLINE] Using neutral fallback values"; // isOffline doesn't exist
        char buf[256];
        snprintf(buf, sizeof(buf),
            "BTC %+.1f%%  |  F&G %d  |  EUR/USD %.4f  |  Vol %.0f%%",
            s.btcChange24h, s.fearGreedIndex,
            s.eurUsd,
            s.globalVolatility * 100.f);
        return buf;
    }

private:
    MarketFeed() = default;

    void FetchAll() {
        MarketState fresh = MarketState::Neutral();
        // fresh.isOffline   = false; // isOffline doesn't exist in MarketState

        bool cryptoOk = CryptoFeed::Fetch(fresh);
        // NewsFeed::Fetch(fresh); // NewsFeed::Fetch() doesn't take parameters
        // NewsFeed::Get().Fetch(); // Fetch() is private, commenting out for now
        TrendFeed::Fetch(fresh);
        FXFeed::Fetch(fresh);           // EUR/USD/RON
        SocialPulseFeed::Fetch(fresh);  // Reddit + HN sentiment
        WikiTrendFeed::Fetch(fresh);    // Wikipedia trending

        // Derive composite scores
        // adMarketHealth: blend of sentiment, volatility, market mood
        fresh.adMarketHealth = 0.5f
            + fresh.newsSentiment          * 0.20f
            + (1.f - fresh.globalVolatility) * 0.15f
            + (fresh.fearGreedIndex / 200.f) * 0.15f;  // 0-100 → 0-0.5
        fresh.adMarketHealth = std::max(0.1f, std::min(2.0f, fresh.adMarketHealth));

        // fresh.isOffline  = !cryptoOk; // isOffline doesn't exist in MarketState
        // fresh.isStale    = false; // isStale doesn't exist in MarketState
        // fresh.fetchedAt  = time(nullptr); // fetchedAt doesn't exist in MarketState

        std::lock_guard<std::mutex> lock(mx_);
        state_ = fresh;
    }

    MarketState        state_;
    mutable std::mutex mx_;
    std::atomic<bool>  running_{false};
    std::thread        worker_;
};
