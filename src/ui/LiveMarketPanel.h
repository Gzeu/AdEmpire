#pragma once
#include "imgui.h"
#include "../network/MarketFeed.h"
#include "../network/NewsFeed.h"
#include "../systems/MarketEventBridge.h"
#include <vector>
#include <string>
#include <cmath>
#include <ctime>

// ============================================================
//  LiveMarketPanel.h  —  AdEmpire v1.0
//  Real-time market data panel: BTC/ETH, Fear & Greed,
//  FX rates, live headlines, ad market health bar.
//  Zero dependencies beyond MarketFeed + NewsFeed.
// ============================================================

namespace LiveMarketPanel {

// ── internal helpers ────────────────────────────────────────
static void RenderGauge(const char* label, float value, float min, float max,
                         ImVec4 colorLow, ImVec4 colorHigh) {
    float t = (value - min) / (max - min);
    t = t < 0.f ? 0.f : (t > 1.f ? 1.f : t);
    ImVec4 col = ImVec4(
        colorLow.x + (colorHigh.x - colorLow.x) * t,
        colorLow.y + (colorHigh.y - colorLow.y) * t,
        colorLow.z + (colorHigh.z - colorLow.z) * t, 1.f);
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, col);
    char buf[32]; snprintf(buf, sizeof(buf), "%.1f", value);
    ImGui::ProgressBar(t, ImVec2(-1.f, 14.f), buf);
    ImGui::PopStyleColor();
    ImGui::SameLine(); ImGui::TextUnformatted(label);
}

static ImVec4 ChangeColor(float pct) {
    if (pct >  0.5f) return ImVec4(0.20f, 0.80f, 0.40f, 1.f);
    if (pct < -0.5f) return ImVec4(0.90f, 0.30f, 0.25f, 1.f);
    return ImVec4(0.75f, 0.75f, 0.75f, 1.f);
}

// ── main render ─────────────────────────────────────────────
static void Render() {
    const MarketState& ms = MarketFeed::Get().GetState();
    bool stale = MarketFeed::Get().IsStale();

    // ── header status bar
    ImGui::Spacing();
    if (stale) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.65f, 0.1f, 1.f));
        ImGui::TextUnformatted("  LIVE DATA  [offline — cached values]");
        ImGui::PopStyleColor();
    } else {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.20f, 0.80f, 0.40f, 1.f));
        ImGui::TextUnformatted("  LIVE DATA  [synced]");
        ImGui::PopStyleColor();
    }
    ImGui::SameLine(ImGui::GetContentRegionAvail().x - 120.f);
    if (ImGui::SmallButton("Refresh")) MarketFeed::Get().FetchAsync();
    ImGui::Separator();

    // ── two columns: crypto left, macro right
    float colW = (ImGui::GetContentRegionAvail().x - 12.f) * 0.5f;
    ImGui::BeginChild("##lm_left", ImVec2(colW, 0), false);

    // Crypto prices
    ImGui::TextUnformatted("CRYPTO");
    ImGui::Separator();

    auto Row = [&](const char* sym, double price, double chg) {
        ImGui::Text("%-6s", sym);
        ImGui::SameLine(70.f);
        ImGui::Text("$%,.0f", price);
        ImGui::SameLine(160.f);
        ImGui::PushStyleColor(ImGuiCol_Text, ChangeColor((float)chg));
        ImGui::Text("%+.2f%%", chg);
        ImGui::PopStyleColor();
    };
    Row("BTC", ms.btcPrice,  ms.btcChange24h);
    Row("ETH", ms.ethPrice,  ms.ethChange24h);

    ImGui::Spacing();

    // Fear & Greed
    const char* fgLabel =
        ms.fearGreedIndex >= 75 ? "Extreme Greed" :
        ms.fearGreedIndex >= 55 ? "Greed" :
        ms.fearGreedIndex >= 45 ? "Neutral" :
        ms.fearGreedIndex >= 25 ? "Fear" : "Extreme Fear";
    ImGui::Text("Fear & Greed: %d  %s", ms.fearGreedIndex, fgLabel);
    RenderGauge("##fg", (float)ms.fearGreedIndex, 0.f, 100.f,
                ImVec4(0.9f,0.3f,0.25f,1.f), ImVec4(0.2f,0.8f,0.4f,1.f));

    ImGui::Spacing();

    // HN AI hype
    ImGui::Text("HN AI Hype: %d pts", NewsFeed::Get().GetAiHypeScore());
    RenderGauge("##aihype", (float)NewsFeed::Get().GetAiHypeScore(),
                0.f, 300.f,
                ImVec4(0.5f,0.5f,0.9f,1.f), ImVec4(0.9f,0.7f,0.1f,1.f));

    ImGui::EndChild();
    ImGui::SameLine();

    ImGui::BeginChild("##lm_right", ImVec2(0, 0), false);

    // FX + macro
    ImGui::TextUnformatted("MACRO / FX");
    ImGui::Separator();
    ImGui::Text("EUR/USD  %.4f", ms.eurUsd > 0 ? ms.eurUsd : 1.0850);
    ImGui::Text("USD/RON  %.4f", ms.usdRon > 0 ? ms.usdRon : 4.6700);
    ImGui::Spacing();

    // Ad market health
    ImGui::TextUnformatted("Ad Market Health");
    float health = ms.adMarketHealth;
    RenderGauge("##health", health * 100.f, 0.f, 100.f,
                ImVec4(0.9f,0.3f,0.25f,1.f), ImVec4(0.2f,0.8f,0.4f,1.f));

    ImGui::Spacing();

    // Global volatility
    ImGui::TextUnformatted("Global Volatility");
    RenderGauge("##vol", ms.globalVolatility * 100.f, 0.f, 100.f,
                ImVec4(0.2f,0.8f,0.4f,1.f), ImVec4(0.9f,0.3f,0.25f,1.f));

    ImGui::Spacing();

    // Trending keyword
    if (!ms.trendingKeyword.empty()) {
        ImGui::Text("Trending: \"%s\"", ms.trendingKeyword.c_str());
    }

    // Active market event badge
    std::string evtId = MarketEventBridge::GetDominantEventId(ms);
    if (!evtId.empty() && evtId != "none") {
        ImGui::Spacing();
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.85f, 0.1f, 1.f));
        ImGui::Text("Active Event: %s", evtId.c_str());
        ImGui::PopStyleColor();
    }

    ImGui::EndChild();

    // ── live headlines ───────────────────────────────────────
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::TextUnformatted("LIVE HEADLINES");
    ImGui::Separator();

    const auto& headlines = NewsFeed::Get().GetHeadlines();
    if (headlines.empty()) {
        ImGui::TextDisabled("  No headlines yet — fetching...");
    } else {
        ImGui::BeginChild("##headlines", ImVec2(0, 140), false);
        for (const auto& h : headlines) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.82f, 0.82f, 0.82f, 1.f));
            ImGui::TextWrapped("%s", h.c_str());
            ImGui::PopStyleColor();
            ImGui::Spacing();
        }
        ImGui::EndChild();
    }

    // Campaign multiplier hint
    float mult = MarketEventBridge::GetCampaignMultiplier(ms);
    ImGui::Spacing();
    ImGui::PushStyleColor(ImGuiCol_Text,
        mult >= 1.f ? ImVec4(0.2f,0.8f,0.4f,1.f) : ImVec4(0.9f,0.4f,0.3f,1.f));
    ImGui::Text("Campaign ROI modifier this month: x%.2f", mult);
    ImGui::PopStyleColor();
}

} // namespace LiveMarketPanel
