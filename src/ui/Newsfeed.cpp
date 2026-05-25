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
    std::string evtId = MarketEventBridge::GetDominantEventId(ms);
    if (!evtId.empty() && evtId != "none") {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.18f, 0.14f, 0.06f, 1.f));
        ImGui::BeginChild("##pulse", ImVec2(0, 36), true);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4.f);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.85f, 0.1f, 1.f));
        ImGui::Text("  ACTIVE EVENT: %s   |   Campaign modifier: x%.2f",
            evtId.c_str(),
            MarketEventBridge::GetCampaignMultiplier(ms));
        ImGui::PopStyleColor();
        ImGui::EndChild();
        ImGui::PopStyleColor();
        ImGui::Spacing();
    }

    // ── Tabs: Headlines / Reddit / Hacker News ──────────────
    if (ImGui::BeginTabBar("##nf_tabs")) {

        if (ImGui::BeginTabItem(" Headlines ")) {
            ImGui::Spacing();
            const auto& hl = NewsFeed::Get().GetHeadlines();
            if (hl.empty()) {
                ImGui::TextDisabled("Fetching live headlines...");
            } else {
                for (size_t i = 0; i < hl.size(); ++i) {
                    ImGui::PushID((int)i);
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.85f, 0.85f, 0.85f, 1.f));
                    ImGui::Bullet();
                    ImGui::SameLine();
                    ImGui::TextWrapped("%s", hl[i].c_str());
                    ImGui::PopStyleColor();
                    ImGui::Spacing();
                    ImGui::PopID();
                }
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem(" Reddit ")) {
            ImGui::Spacing();
            const auto& rh = NewsFeed::Get().GetRedditHeadlines();
            if (rh.empty()) {
                ImGui::TextDisabled("Loading r/marketing + r/digitalmarketing...");
            } else {
                for (size_t i = 0; i < rh.size(); ++i) {
                    ImGui::PushID((int)(i + 1000));
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.55f, 0.25f, 1.f));
                    ImGui::Bullet();
                    ImGui::SameLine();
                    ImGui::TextWrapped("%s", rh[i].c_str());
                    ImGui::PopStyleColor();
                    ImGui::Spacing();
                    ImGui::PopID();
                }
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem(" Hacker News ")) {
            ImGui::Spacing();
            ImGui::Text("AI Hype Score: %d / 300 pts", NewsFeed::Get().GetAiHypeScore());
            ImGui::Spacing();
            const auto& hn = NewsFeed::Get().GetHackerNewsHeadlines();
            if (hn.empty()) {
                ImGui::TextDisabled("Loading top HN stories...");
            } else {
                for (size_t i = 0; i < hn.size(); ++i) {
                    ImGui::PushID((int)(i + 2000));
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.70f, 0.15f, 1.f));
                    ImGui::Bullet();
                    ImGui::SameLine();
                    ImGui::TextWrapped("%s", hn[i].c_str());
                    ImGui::PopStyleColor();
                    ImGui::Spacing();
                    ImGui::PopID();
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
