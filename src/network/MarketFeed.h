#pragma once
#include "MarketState.h"
#include "CryptoFeed.h"
#include "NewsFeed.h"
#include "TrendFeed.h"
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>

// ============================================================
//  MarketFeed — singleton orchestrator
//  Usage:
//    // After game init:
//    MarketFeed::Get().StartAsync();
//
//    // Each frame / Next Month:
//    const MarketState& ms = MarketFeed::Get().GetState();
//    float health = ms.adMarketHealth;  // use in campaign ROI
//
//    // Manual refresh:
//    MarketFeed::Get().RefreshNow();
// ============================================================

class MarketFeed {
public:
    static MarketFeed& Get() {
        static MarketFeed instance;
        return instance;
    }

    // Start background refresh every 5 minutes
    void StartAsync() {
        if (running_) return;
        running_ = true;
        state_   = MarketState::Neutral();
        worker_  = std::thread([this]() {
            while (running_) {
                FetchAll();
                // Sleep 5 minutes, checking for stop every second
                for (int i = 0; i < 300 && running_; ++i)
                    std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        });
        worker_.detach();
    }

    void Stop() { running_ = false; }

    // Force immediate refresh (call from UI "Refresh Market" button)
    void RefreshNow() {
        std::thread([this](){ FetchAll(); }).detach();
    }

    const MarketState& GetState() const {
        std::lock_guard<std::mutex> lock(mx_);
        return state_;
    }

    // Convenience — safe to call every frame
    float GetAdMarketHealth()   const { return GetState().adMarketHealth; }
    float GetGlobalVolatility() const { return GetState().globalVolatility; }
    bool  IsOffline()           const { return GetState().isOffline; }

private:
    MarketFeed() = default;

    void FetchAll() {
        MarketState fresh = MarketState::Neutral();
        fresh.isOffline = false;

        bool cryptoOk = CryptoFeed::Fetch(fresh);
        NewsFeed::Fetch(fresh);
        TrendFeed::Fetch(fresh);

        fresh.isOffline  = !cryptoOk;
        fresh.isStale    = false;
        fresh.fetchedAt  = time(nullptr);

        std::lock_guard<std::mutex> lock(mx_);
        state_ = fresh;
    }

    MarketState       state_;
    mutable std::mutex mx_;
    std::atomic<bool>  running_{false};
    std::thread        worker_;
};
