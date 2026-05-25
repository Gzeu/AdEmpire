#pragma once
#include "../network/MarketFeed.h"
#include "../network/MarketEventBridge.h"
#include "imgui.h"
#include <string>
#include <vector>

// ============================================================
//  LiveMarketPanel — Dashboard tab "Live Market"
//  Call inside an existing ImGui window / tab bar:
//    LiveMarketPanel::Render();
// ============================================================

namespace LiveMarketPanel {

static inline void RenderGauge(const char* label, float value, float min, float max,
                                const char* fmt,
                                ImVec4 colorLow, ImVec4 colorHigh) {
    float t = (max > min) ? (value - min) / (max - min) : 0.5f;
    t = std::max(0.f, std::min(1.f, t));
    ImVec4 col = ImVec4(
        colorLow.x + t * (colorHigh.x - colorLow.x),
        colorLow.y + t * (colorHigh.y - colorLow.y),
        colorLow.z + t * (colorHigh.z - colorLow.z), 1.f);
    ImGui::TextColored(col, fmt, value);
    ImGui::SameLine();
    ImGui::TextDisabled("%s", label);
}

void Render() {
    const MarketState& ms = MarketFeed::Get().GetState();

    // ── Status bar ──────────────────────────────────────────
    // Note: isOffline doesn't exist in MarketState, using default
    ImGui::TextColored(ImVec4(0.4f,0.9f,0.4f,1), "[LIVE]");
    ImGui::SameLine();
    ImGui::TextDisabled("Market data");
    ImGui::Separator();

    // ── KPI grid ────────────────────────────────────────────
    ImGui::Columns(3, "market_kpis", false);

    // Col 1 — Crypto
    ImGui::TextDisabled("CRYPTO");
    RenderGauge("BTC 24h", ms.btcChange24h, -20.f, 20.f, "%+.2f%%",
        ImVec4(1,0.3f,0.3f,1), ImVec4(0.3f,1,0.3f,1));
    RenderGauge("ETH 24h", ms.ethChange24h, -20.f, 20.f, "%+.2f%%",
        ImVec4(1,0.3f,0.3f,1), ImVec4(0.3f,1,0.3f,1));
    ImGui::Text("Fear & Greed: %d / 100", ms.fearGreedIndex);
    {
        const char* label = ms.fearGreedIndex < 25  ? "Extreme Fear" :
                            ms.fearGreedIndex < 45  ? "Fear" :
                            ms.fearGreedIndex < 55  ? "Neutral" :
                            ms.fearGreedIndex < 75  ? "Greed" : "Extreme Greed";
        ImGui::TextDisabled("  %s", label);
    }

    ImGui::NextColumn();

    // Col 2 — FX & Macro
    ImGui::TextDisabled("FX / MACRO");
    ImGui::Text("EUR/USD  %.4f", ms.eurUsd);
    // ImGui::Text("GBP/USD  %.4f", ms.gbpUsd); // gbpUsd doesn't exist in MarketState
    ImGui::Text("USD/RON  %.4f", ms.usdRon);
    ImGui::Spacing();
    RenderGauge("Ad Market Health", ms.adMarketHealth, 0.1f, 2.0f, "%.2fx",
        ImVec4(1,0.3f,0.3f,1), ImVec4(0.3f,1,0.3f,1));
    RenderGauge("Global Volatility", ms.globalVolatility * 100.f, 0.f, 100.f, "%.0f%%",
        ImVec4(0.3f,1,0.3f,1), ImVec4(1,0.3f,0.3f,1));

    ImGui::NextColumn();

    // Col 3 — Sentiment
    ImGui::TextDisabled("SENTIMENT");
    RenderGauge("News",        ms.newsSentiment,   -1.f, 1.f, "%+.2f",
        ImVec4(1,0.3f,0.3f,1), ImVec4(0.3f,1,0.3f,1));
    // RenderGauge("AI Hype",     ms.aiHypeScore,      0.f, 1.f, "%.0f%%", // aiHypeScore doesn't exist
    //     ImVec4(0.5f,0.5f,0.5f,1), ImVec4(0.6f,0.4f,1,1));
    // RenderGauge("Crypto Buzz", ms.cryptoSentiment, -1.f, 1.f, "%+.2f", // cryptoSentiment doesn't exist
    //     ImVec4(1,0.3f,0.3f,1), ImVec4(0.3f,1,0.3f,1));
    if (!ms.trendingKeyword.empty())
        ImGui::Text("Trending: #%s", ms.trendingKeyword.c_str());
    // if (!ms.wikiTrendCategory.empty()) // wikiTrendCategory doesn't exist
    //     ImGui::TextDisabled("Wiki: %s", ms.wikiTrendCategory.c_str());

    ImGui::Columns(1);
    ImGui::Separator();

    // ── Active market events ─────────────────────────────────
    std::vector<std::string> triggered;
    MarketEventBridge::EvaluateTriggers(ms, [&](const std::string& id) {
        triggered.push_back(id);
    });
    if (!triggered.empty()) {
        ImGui::TextColored(ImVec4(1,0.85f,0,1), "ACTIVE MARKET TRIGGERS (%zu)", triggered.size());
        for (const auto& evId : triggered) {
            ImGui::BulletText("%s", evId.c_str());
        }
    } else {
        ImGui::TextDisabled("No active market triggers — stable conditions");
    }

    ImGui::Spacing();

    // ── Refresh button ───────────────────────────────────────
    if (ImGui::Button("Refresh Market Data")) {
        MarketFeed::Get().RefreshNow();
    }
    ImGui::SameLine();
    // Note: fetchedAt doesn't exist in MarketState
    ImGui::TextDisabled("Last fetch: N/A");
}

} // namespace LiveMarketPanel
