#pragma once
#include "imgui.h"
#include "../network/MarketState.h"
#include "../network/MarketFeed.h"
#include "../network/NewsFeed.h"
#include <string>
#include <vector>
#include <cmath>

// LiveMarketPanel — renders the "Live Market" and "Newsfeed" tabs in Dashboard
// Usage: call LiveMarketPanel::RenderLiveMarket() and LiveMarketPanel::RenderNewsfeed()

class LiveMarketPanel {
public:

    // ── Live Market tab ──────────────────────────────────────────────────────
    static void RenderLiveMarket() {
        const MarketState& s = MarketFeed::Get().GetState();
        bool stale = MarketFeed::Get().IsStale();

        // Status bar
        if (stale) {
            ImGui::TextColored(ImVec4(1,0.6f,0,1), "(!) Data may be stale (>5 min)");
        } else {
            ImGui::TextColored(ImVec4(0.3f,0.9f,0.4f,1), "(*) Live");
        }
        ImGui::SameLine();
        if (ImGui::SmallButton("Refresh Now")) {
            MarketFeed::Get().FetchAsync();
        }
        ImGui::Separator();

        // ── Fear & Greed Gauge ───────────────────────────────────────────────
        ImGui::Text("Fear & Greed Index");
        ImGui::SameLine();
        ImGui::TextDisabled("(%s)", GetFGLabel(s.fearGreedIndex));

        float fgNorm = s.fearGreedIndex / 100.0f;
        ImVec4 fgColor = GetFGColor(s.fearGreedIndex);
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, fgColor);
        ImGui::ProgressBar(fgNorm, ImVec2(-1, 22));
        ImGui::PopStyleColor();
        ImGui::TextDisabled("%d / 100", s.fearGreedIndex);
        ImGui::Spacing();

        // ── Crypto Prices ────────────────────────────────────────────────────
        ImGui::Text("Crypto Markets (24h change)");
        ImGui::Separator();

        RenderChangeBar("BTC", s.btcChange24h, 15.0f);
        RenderChangeBar("ETH", s.ethChange24h, 15.0f);
        ImGui::Spacing();

        // ── FX Rate ──────────────────────────────────────────────────────────
        ImGui::Text("EUR / USD");
        ImGui::SameLine();
        if (s.eurUsd > 0.0f) {
            ImGui::TextColored(ImVec4(0.8f,0.9f,1.0f,1), "%.4f", s.eurUsd);
        } else {
            ImGui::TextDisabled("N/A");
        }
        ImGui::Spacing();

        // ── Global Volatility ────────────────────────────────────────────────
        ImGui::Text("Global Volatility");
        ImGui::SameLine();
        ImVec4 volColor = s.globalVolatility > 0.7f
            ? ImVec4(1,0.3f,0.3f,1)
            : (s.globalVolatility > 0.4f ? ImVec4(1,0.8f,0.2f,1) : ImVec4(0.3f,0.9f,0.4f,1));
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, volColor);
        ImGui::ProgressBar(s.globalVolatility, ImVec2(-1, 14));
        ImGui::PopStyleColor();
        ImGui::Spacing();

        // ── Trending Keyword ─────────────────────────────────────────────────
        if (!s.trendingKeyword.empty()) {
            ImGui::Text("Trending:");
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1,0.85f,0.2f,1), "# %s", s.trendingKeyword.c_str());
        }

        // ── Revenue Multiplier indicator ─────────────────────────────────────
        ImGui::Spacing();
        ImGui::Separator();
        // We can't access bridge directly here without full include, show raw calc
        float mult = 1.0f;
        if (s.fearGreedIndex > 70)  mult += 0.15f;
        if (s.btcChange24h > 5.0f)  mult += 0.08f;
        if (s.fearGreedIndex < 25)  mult -= 0.20f;
        if (s.btcChange24h < -8.0f) mult -= 0.15f;
        mult = std::max(0.3f, std::min(2.0f, mult));

        ImGui::Text("Campaign Revenue Modifier:");
        ImGui::SameLine();
        ImVec4 multColor = mult >= 1.0f ? ImVec4(0.3f,0.9f,0.4f,1) : ImVec4(1,0.3f,0.3f,1);
        ImGui::TextColored(multColor, "x%.2f", mult);
        ImGui::TextDisabled("Derived from Fear&Greed + BTC + news sentiment");
    }

    // ── Newsfeed tab ─────────────────────────────────────────────────────────
    static void RenderNewsfeed() {
        const MarketState& s = MarketFeed::Get().GetState();
        const auto& headlines = s.newsItems;
        const auto& keywords = s.trendingKeywords;

        // Trending keywords pills
        if (!keywords.empty()) {
            ImGui::Text("Trending Topics:");
            ImGui::SameLine();
            for (const auto& kw : keywords) {
                ImGui::TextColored(ImVec4(1,0.85f,0.2f,1), "[%s] ", kw.c_str());
                ImGui::SameLine();
            }
            ImGui::NewLine();
            ImGui::Separator();
        }

        if (headlines.empty()) {
            ImGui::TextDisabled("No headlines loaded. Check network connection.");
            return;
        }

        // Headlines list
        ImGui::BeginChild("##headlines_scroll", ImVec2(0, 0), false);
        int idx = 0;
        for (const auto& h : headlines) {
            ImGui::PushID(idx++);

            // Simple headline display
            ImGui::TextWrapped("[%s] %s", h.source.c_str(), h.title.c_str());
            ImGui::PopID();
        }
        ImGui::EndChild();
    }

private:
    static void RenderChangeBar(const char* label, float change, float maxAbs) {
        ImGui::Text("%-5s", label);
        ImGui::SameLine();

        float norm = std::max(-1.0f, std::min(1.0f, change / maxAbs));
        ImVec4 col = norm >= 0
            ? ImVec4(0.2f + norm*0.1f, 0.7f + norm*0.2f, 0.3f, 1)
            : ImVec4(0.8f + norm*(-0.1f), 0.2f, 0.2f, 1);

        char overlay[32];
        snprintf(overlay, sizeof(overlay), "%+.2f%%", change);

        // Draw bar centered at 0.5 going right (bull) or left (bear)
        float frac = 0.5f + norm * 0.5f;
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, col);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.12f,0.12f,0.12f,1));
        ImGui::ProgressBar(frac, ImVec2(-1, 18), overlay);
        ImGui::PopStyleColor(2);
    }

    static const char* GetFGLabel(int idx) {
        if (idx <= 25)  return "Extreme Fear";
        if (idx <= 45)  return "Fear";
        if (idx <= 55)  return "Neutral";
        if (idx <= 75)  return "Greed";
        return "Extreme Greed";
    }

    static ImVec4 GetFGColor(int idx) {
        if (idx <= 25)  return ImVec4(1.0f, 0.2f, 0.2f, 1.0f);  // red
        if (idx <= 45)  return ImVec4(1.0f, 0.6f, 0.1f, 1.0f);  // orange
        if (idx <= 55)  return ImVec4(0.9f, 0.9f, 0.2f, 1.0f);  // yellow
        if (idx <= 75)  return ImVec4(0.4f, 0.9f, 0.3f, 1.0f);  // green
        return                ImVec4(0.1f, 0.8f, 0.2f, 1.0f);   // bright green
    }
};
