#pragma once
#include "MarketState.h"
#include "../vendor/httplib.h"
#include "../vendor/json.hpp"
#include <string>
#include <vector>
#include <algorithm>
#include <map>

// ============================================================
//  WikiTrendFeed — Wikipedia Most Viewed (global trending)
//  Source: en.wikipedia.org/w/api.php mostviewed (zero auth)
//  Fills: state.trendingKeyword, state.wikiTrendScore,
//         state.aiHypeActive, state.recessionRiskActive
// ============================================================

class WikiTrendFeed {
public:
    static void Fetch(MarketState& state) {
        auto topics = FetchTopics();
        if (topics.empty()) return;
        ClassifyTopics(topics, state);
    }

private:
    struct WikiPage { std::string title; int views; };

    static std::vector<WikiPage> FetchTopics() {
        std::vector<WikiPage> pages;
        try {
            httplib::Client cli("https://en.wikipedia.org");
            cli.set_connection_timeout(6);
            cli.set_read_timeout(8);
            auto res = cli.Get(
                "/w/api.php?action=query&list=mostviewed"
                "&pvimoffset=0&pvimlimit=20&format=json");
            if (!res || res->status != 200) return pages;
            auto j = nlohmann::json::parse(res->body);
            for (auto& p : j["query"]["mostviewed"]) {
                std::string title = p.value("title", "");
                int views         = p.value("count", 0);
                pages.push_back({title, views});
            }
        } catch (...) {}
        return pages;
    }

    // Map Wikipedia titles to MarketState signals
    static void ClassifyTopics(const std::vector<WikiPage>& pages, MarketState& state) {
        // keyword → (field_flag, score_boost)
        static const std::map<std::string, std::string> categoryMap = {
            // AI / Tech
            {"artificial intelligence", "ai"},
            {"openai",  "ai"}, {"chatgpt", "ai"}, {"gemini", "ai"},
            {"large language model", "ai"}, {"machine learning", "ai"},
            // Crypto
            {"bitcoin","crypto"}, {"ethereum","crypto"}, {"cryptocurrency","crypto"},
            {"blockchain","crypto"}, {"solana","crypto"}, {"defi","crypto"},
            // Macro / Risk
            {"recession","macro_bear"}, {"inflation","macro_bear"},
            {"financial crisis","macro_bear"}, {"stock market crash","macro_bear"},
            {"unemployment","macro_bear"}, {"bank failure","macro_bear"},
            // Macro / Bull
            {"ipo","macro_bull"}, {"merger","macro_bull"},
            {"record profit","macro_bull"}, {"economic growth","macro_bull"},
            // Marketing / Ads
            {"advertising","adtech"}, {"social media","adtech"},
            {"tiktok","adtech"}, {"meta","adtech"}, {"google ads","adtech"},
            // Geopolitical
            {"war","geopolitical"}, {"sanctions","geopolitical"},
            {"trade war","geopolitical"}, {"tariff","geopolitical"}
        };

        float aiScore      = 0.0f;
        float cryptoScore  = 0.0f;
        float macroBear    = 0.0f;
        float macroBull    = 0.0f;
        float adtechScore  = 0.0f;
        float geoScore     = 0.0f;
        bool  firstSet     = false;

        for (auto& page : pages) {
            std::string tl = page.title;
            std::transform(tl.begin(), tl.end(), tl.begin(), ::tolower);
            // noise filter
            if (tl.find("main page") != std::string::npos ||
                tl.find("special:")  != std::string::npos ||
                tl.find("wikipedia:") != std::string::npos) continue;
            // score by view weight (log-normalized)
            float weight = (page.views > 0) ? std::log10((float)page.views) / 6.0f : 0.1f;
            for (auto& [keyword, category] : categoryMap) {
                if (tl.find(keyword) != std::string::npos) {
                    if      (category == "ai")          aiScore      += weight;
                    else if (category == "crypto")      cryptoScore  += weight;
                    else if (category == "macro_bear")  macroBear    += weight;
                    else if (category == "macro_bull")  macroBull    += weight;
                    else if (category == "adtech")      adtechScore  += weight;
                    else if (category == "geopolitical") geoScore    += weight;
                }
            }
            // First non-noise article as global trending topic
            if (!firstSet && !page.title.empty()) {
                state.trendingKeyword = page.title;
                firstSet = true;
            }
        }

        // Normalize to 0-1
        auto norm = [](float v) { return std::max(0.0f, std::min(1.0f, v)); };
        state.wikiAiScore        = norm(aiScore);
        state.wikiCryptoScore    = norm(cryptoScore);
        state.wikiMacroBearScore = norm(macroBear);
        state.wikiMacroBullScore = norm(macroBull);
        state.wikiAdtechScore    = norm(adtechScore);
        state.wikiGeoScore       = norm(geoScore);

        // Combine with existing feed scores if already set
        state.aiHypeScore     = std::min(1.0f, state.aiHypeScore     + state.wikiAiScore     * 0.4f);
        state.cryptoSentiment = std::min(1.0f, state.cryptoSentiment + state.wikiCryptoScore * 0.3f);

        // Final boolean flags for EventSystem
        state.aiHypeActive        = (state.aiHypeScore > 0.45f);
        state.recessionRiskActive = (state.wikiMacroBearScore > 0.35f);
        state.adtechBoomActive    = (state.wikiAdtechScore > 0.30f);
        state.geopoliticalRisk    = (state.wikiGeoScore > 0.25f);
    }
};
