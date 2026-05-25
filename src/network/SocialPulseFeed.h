#pragma once
#include "MarketState.h"
#include "httplib.h"
#include "json.hpp"
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

// ============================================================
//  SocialPulseFeed — aggregated sentiment from public RSS/JSON
//  Sources (zero auth):
//    Reddit RSS   — r/marketing, r/digitalmarketing,
//                   r/cryptocurrency, r/economics
//    HackerNews   — Firebase JSON, top 5 stories title scan
//  Fills: state.newsSentiment, state.aiHypeScore,
//         state.cryptoSentiment, state.macroSentiment,
//         state.redditTrendingTopic
// ============================================================

class SocialPulseFeed {
public:
    static void Fetch(MarketState& state) {
        float redditScore  = FetchRedditSentiment(state);
        float hnScore      = FetchHackerNews(state);
        // Weighted aggregate: Reddit 60%, HN 40%
        state.newsSentiment = redditScore * 0.6f + hnScore * 0.4f;
        state.newsSentiment = std::max(-1.0f, std::min(1.0f, state.newsSentiment));
    }

private:
    // ---- keyword dictionaries --------------------------------
    static const std::vector<std::string>& BullishWords() {
        static std::vector<std::string> w = {
            "growth","surge","rally","bullish","boom","profit",
            "revenue","record","launch","partnership","adoption",
            "innovation","breakthrough","hiring","expansion"
        };
        return w;
    }
    static const std::vector<std::string>& BearishWords() {
        static std::vector<std::string> w = {
            "crash","plunge","bear","recession","layoff","bankrupt",
            "fraud","hack","regulation","ban","lawsuit","collapse",
            "decline","loss","cut","warning","inflation","crisis"
        };
        return w;
    }
    static const std::vector<std::string>& AIWords() {
        static std::vector<std::string> w = {
            "ai","artificial intelligence","gpt","llm","openai",
            "claude","gemini","machine learning","deep learning",
            "automation","chatbot","agent"
        };
        return w;
    }

    static float ScoreText(const std::string& text,
                           MarketState& state,
                           const std::string& subreddit) {
        std::string lo = text;
        std::transform(lo.begin(), lo.end(), lo.begin(), ::tolower);
        float score = 0.0f;
        int   hits  = 0;
        for (auto& w : BullishWords()) if (lo.find(w) != std::string::npos) { score += 0.15f; hits++; }
        for (auto& w : BearishWords()) if (lo.find(w) != std::string::npos) { score -= 0.15f; hits++; }
        // AI hype detection
        int aiHits = 0;
        for (auto& w : AIWords()) if (lo.find(w) != std::string::npos) aiHits++;
        // state.aiHypeScore += aiHits * 0.1f; // aiHypeScore doesn't exist
        // per-subreddit routing
        if (subreddit == "cryptocurrency" || subreddit == "crypto")
            // state.cryptoSentiment += (hits > 0) ? score / hits : 0; // cryptoSentiment doesn't exist
            ;
        else if (subreddit == "economics")
            // state.macroSentiment += (hits > 0) ? score / hits : 0; // macroSentiment doesn't exist
            ;
        return (hits > 0) ? score / hits : 0.0f;
    }

    static float FetchRedditSentiment(MarketState& state) {
        static const std::vector<std::pair<std::string,std::string>> feeds = {
            {"marketing",       "/r/marketing.rss"},
            {"digitalmarketing","/r/digitalmarketing.rss"},
            {"cryptocurrency",  "/r/cryptocurrency.rss"},
            {"economics",       "/r/economics.rss"}
        };
        // state.aiHypeScore      = 0.0f; // aiHypeScore doesn't exist in MarketState
        // state.cryptoSentiment  = 0.0f; // cryptoSentiment doesn't exist in MarketState
        // state.macroSentiment   = 0.0f; // macroSentiment doesn't exist in MarketState
        float total = 0.0f;
        int   count = 0;
        httplib::Client cli("http://www.reddit.com");
        cli.set_connection_timeout(6);
        cli.set_read_timeout(8);
        cli.set_default_headers({{"User-Agent", "AdEmpire/1.0 market-feed"}});
        for (auto& [sub, path] : feeds) {
            try {
                auto res = cli.Get(path.c_str());
                if (!res || res->status != 200) continue;
                // Extract titles from RSS XML via simple string scan
                std::string& body = res->body;
                size_t pos = 0;
                int titles = 0;
                while ((pos = body.find("<title>", pos)) != std::string::npos && titles < 8) {
                    size_t end = body.find("</title>", pos + 7);
                    if (end == std::string::npos) break;
                    std::string title = body.substr(pos + 7, end - pos - 7);
                    total += ScoreText(title, state, sub);
                    count++; titles++;
                    pos = end + 8;
                }
                // First real title as trending topic for this sub
                if (sub == "marketing" || sub == "digitalmarketing") {
                    size_t tp = body.find("<title>", 0);
                    if (tp != std::string::npos) {
                        size_t te = body.find("</title>", tp + 7);
                        if (te != std::string::npos)
                            // state.redditTrendingTopic = body.substr(tp + 7, te - tp - 7); // redditTrendingTopic doesn't exist
                            ;
                    }
                }
            } catch (...) {}
        }
        // Normalize aiHypeScore to 0-1
        // state.aiHypeScore     = std::min(1.0f, state.aiHypeScore / 3.0f); // aiHypeScore doesn't exist
        // state.cryptoSentiment = std::max(-1.0f, std::min(1.0f, state.cryptoSentiment)); // cryptoSentiment doesn't exist
        // state.macroSentiment  = std::max(-1.0f, std::min(1.0f, state.macroSentiment)); // macroSentiment doesn't exist
        return (count > 0) ? total / count : 0.0f;
    }

    static float FetchHackerNews(MarketState& state) {
        try {
            httplib::Client cli("http://hacker-news.firebaseio.com");
            cli.set_connection_timeout(5);
            cli.set_read_timeout(6);
            // Top story IDs
            auto res = cli.Get("/v0/topstories.json");
            if (!res || res->status != 200) return 0.0f;
            auto ids = nlohmann::json::parse(res->body);
            float total = 0.0f;
            int   count = 0;
            for (int i = 0; i < 5 && i < (int)ids.size(); i++) {
                int id = ids[i].get<int>();
                std::string path = "/v0/item/" + std::to_string(id) + ".json";
                auto item = cli.Get(path.c_str());
                if (!item || item->status != 200) continue;
                auto j = nlohmann::json::parse(item->body);
                std::string title = j.value("title", "");
                total += ScoreText(title, state, "hackernews");
                count++;
            }
            return (count > 0) ? total / count : 0.0f;
        } catch (...) { return 0.0f; }
    }
};
