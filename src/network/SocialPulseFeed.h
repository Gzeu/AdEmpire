#pragma once
#include "MarketState.h"
#include "httplib.h"
#include "json.hpp"
#include <string>
#include <vector>

// ============================================================
//  SocialPulseFeed
//  Sources (HTTPS, zero auth):
//    Reddit RSS  — r/cryptocurrency, r/economics sentiment
//    HackerNews  — tech/AI hype score (story count per topic)
//  Derives: aiHypeScore, cryptoSentiment, economicSentiment
// ============================================================

class SocialPulseFeed {
public:
    static void Fetch(MarketState& state) {
        int aiMentions    = 0;
        int cryptoMentions = 0;
        int econMentions  = 0;

        FetchRedditSentiment("cryptocurrency", cryptoMentions, state);
        FetchRedditSentiment("economics",      econMentions,   state);
        FetchHNAIHype(aiMentions);

        // Normalize to 0-100 scores
        state.aiHypeScore       = std::min(100, aiMentions * 5);
        state.cryptoSentiment   = std::min(100, cryptoMentions * 4);
        state.economicSentiment = std::min(100, econMentions * 4);
    }

private:
    static void FetchRedditSentiment(const std::string& sub,
                                     int& mentionCount,
                                     MarketState& state) {
        try {
            httplib::SSLClient cli("www.reddit.com", 443);
            cli.set_connection_timeout(5);
            cli.set_read_timeout(5);
            cli.enable_server_certificate_verification(true);
            httplib::Headers hdrs = {{ "User-Agent", "AdEmpire/1.0" }};
            auto res = cli.Get(("/r/" + sub + ".rss").c_str(), hdrs);
            if (!res || res->status != 200) return;
            // Count positive keywords as simple sentiment proxy
            const std::string& body = res->body;
            auto count = [&](const std::string& kw) {
                size_t pos = 0, n = 0;
                while ((pos = body.find(kw, pos)) != std::string::npos) { ++n; ++pos; }
                return (int)n;
            };
            mentionCount = count("bull") + count("rally") + count("gain") +
                           count("rise") + count("pump") + count("boom");
            // Negative discount
            int neg = count("bear") + count("crash") + count("dump") +
                      count("loss") + count("fall") + count("recession");
            mentionCount = std::max(0, mentionCount - neg / 2);
        } catch (...) {}
    }

    static void FetchHNAIHype(int& aiMentions) {
        try {
            httplib::SSLClient cli("hacker-news.firebaseio.com", 443);
            cli.set_connection_timeout(5);
            cli.set_read_timeout(8);
            cli.enable_server_certificate_verification(true);
            auto res = cli.Get("/v0/topstories.json");
            if (!res || res->status != 200) return;
            auto ids = nlohmann::json::parse(res->body);
            int checked = 0;
            for (auto& id : ids) {
                if (checked >= 20) break;
                std::string path = "/v0/item/" + std::to_string(id.get<int>()) + ".json";
                auto sr = cli.Get(path.c_str());
                if (!sr || sr->status != 200) { checked++; continue; }
                auto item = nlohmann::json::parse(sr->body);
                if (item.contains("title")) {
                    std::string t = item["title"].get<std::string>();
                    if (t.find("AI") != std::string::npos ||
                        t.find("LLM") != std::string::npos ||
                        t.find("GPT") != std::string::npos ||
                        t.find("model") != std::string::npos)
                        aiMentions++;
                }
                checked++;
            }
        } catch (...) {}
    }
};
