#include "Newsfeed.h"
#include "imgui.h"
#include "../network/NewsFeed.h"
#include "../network/MarketFeed.h"
#include "../systems/MarketEventBridge.h"
#include <ctime>

std::vector<NewsEntry> Newsfeed::s_entries;

void Newsfeed::Render(GameState& gs) {
    ImGui::Begin("Newsfeed");

    ImGui::TextUnformatted("LIVE MARKET INTELLIGENCE");
    ImGui::Separator();
    ImGui::Spacing();

    const MarketState& ms = MarketFeed::Get().GetState();

    // ── Market pulse banner ─────────────────────────────────
    auto triggered = MarketEventBridge::Get().Evaluate(ms, gs.month * 30.0f);
    if (!triggered.empty()) {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.18f, 0.14f, 0.06f, 1.f));
        ImGui::BeginChild("##pulse", ImVec2(0, 36), true);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4.f);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.85f, 0.1f, 1.f));
        ImGui::Text("  ACTIVE EVENT: %s   |   Campaign modifier: x%.2f",
            triggered[0].c_str(),
            MarketEventBridge::Get().GetRevenueMultiplier(ms));
        ImGui::PopStyleColor();
        ImGui::EndChild();
        ImGui::PopStyleColor();
        ImGui::Spacing();
    }

    // ── Tabs: Headlines / Reddit / Hacker News ──────────────
    if (ImGui::BeginTabBar("##nf_tabs")) {

        if (ImGui::BeginTabItem(" Headlines ")) {
            ImGui::Spacing();
            const auto& hl = ms.newsItems;
            if (hl.empty()) {
                ImGui::TextDisabled("Fetching live headlines...");
            } else {
                for (size_t i = 0; i < hl.size(); ++i) {
                    ImGui::PushID((int)i);
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.85f, 0.85f, 0.85f, 1.f));
                    ImGui::Bullet();
                    ImGui::SameLine();
                    ImGui::TextWrapped("[%s] %s", hl[i].source.c_str(), hl[i].title.c_str());
                    ImGui::PopStyleColor();
                    ImGui::Spacing();
                    ImGui::PopID();
                }
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem(" Reddit ")) {
            ImGui::Spacing();
            int redditCount = 0;
            for (const auto& item : ms.newsItems) {
                if (item.source.rfind("r/", 0) == 0) redditCount++;
            }
            if (redditCount == 0) {
                ImGui::TextDisabled("Loading r/marketing + r/digitalmarketing...");
            } else {
                int idx = 0;
                for (const auto& item : ms.newsItems) {
                    if (item.source.rfind("r/", 0) == 0) {
                        ImGui::PushID((int)(idx + 1000));
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.55f, 0.25f, 1.f));
                        ImGui::Bullet();
                        ImGui::SameLine();
                        ImGui::TextWrapped("[%s] %s", item.source.c_str(), item.title.c_str());
                        ImGui::PopStyleColor();
                        ImGui::Spacing();
                        ImGui::PopID();
                        idx++;
                    }
                }
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem(" Hacker News ")) {
            ImGui::Spacing();
            ImGui::Text("AI Hype Score: %d / 100", ms.aiHypeScore);
            ImGui::Spacing();
            int hnCount = 0;
            for (const auto& item : ms.newsItems) {
                if (item.source == "HackerNews") hnCount++;
            }
            if (hnCount == 0) {
                ImGui::TextDisabled("Loading top HN stories...");
            } else {
                int idx = 0;
                for (const auto& item : ms.newsItems) {
                    if (item.source == "HackerNews") {
                        ImGui::PushID((int)(idx + 2000));
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.70f, 0.15f, 1.f));
                        ImGui::Bullet();
                        ImGui::SameLine();
                        ImGui::TextWrapped("%s", item.title.c_str());
                        ImGui::PopStyleColor();
                        ImGui::Spacing();
                        ImGui::PopID();
                        idx++;
                    }
                }
            }
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    // ── footer: FX strip ────────────────────────────────────
    ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 28.f);
    ImGui::Separator();
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.55f, 0.55f, 0.55f, 1.f));
    ImGui::Text("  BTC: $%.0f  (%+.2f%%)   |   F&G: %d   |   EUR/USD: %.4f   |   AdHealth: %.0f%%",
        ms.btcPrice, ms.btcChange24h,
        ms.fearGreedIndex,
        ms.eurUsd > 0 ? ms.eurUsd : 1.0850,
        ms.adMarketHealth * 100.f);
    ImGui::PopStyleColor();

    ImGui::End();
}

void Newsfeed::PushNews(const std::string& text, float r, float g, float b) {
    s_entries.push_back({text, r, g, b});
}
