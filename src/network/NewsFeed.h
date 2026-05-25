#pragma once
#include "MarketState.h"
#include "../vendor/httplib.h"
#include <string>
#include <vector>
#include <algorithm>
#include <regex>
#include <sstream>

// ============================================================
//  NewsFeed
//  Sources (zero API key, all public RSS/JSON):
//    CoinDesk RSS   — crypto news
//    Reddit RSS     — r/marketing, r/digitalmarketing, r/cryptocurrency
//    Hacker News    — tech top stories (Firebase JSON)
//  Fills: headlines, newsSentiment, aiHypeActive
// ============================================================

class NewsFeed {
public:
    static void Fetch(MarketState& state) {
        state.headlines.clear();
        FetchCoinDeskRSS(state);
        FetchRedditRSS(state, "marketing");
        FetchRedditRSS(state, "digitalmarketing");
        FetchHackerNews(state);
        ScoreSentiment(state);
    }

private:
    // Naive XML title extractor — no dependency needed
    static std::vector<std::string> ExtractRSSTitles(const std::string& xml, int max = 5) {
        std::vector<std::string> out;
        std::regex re("<title><!\\[CDATA\\[([^\\]]+)\\]\\]></title>|<title>([^<]+)</title>");
        auto begin = std::sregex_iterator(xml.begin(), xml.end(), re);
        auto end   = std::sregex_iterator();
        for (auto it = begin; it != end && (int)out.size() < max; ++it) {
            std::string t = (*it)[1].str().empty() ? (*it)[2].str() : (*it)[1].str();
            if (!t.empty() && t != "RSS" && t.size() > 6)
                out.push_back(t);
        }
        return out;
    }

    static void FetchCoinDeskRSS(MarketState& state) {
        try {
            httplib::Client cli("https://www.coindesk.com");
            cli.set_connection_timeout(5); cli.set_read_timeout(5);
            // CoinDesk feed
            auto res = cli.Get("/arc/outboundfeeds/rss/?outputType=xml");
            if (!res || res->status != 200) return;
            auto titles = ExtractRSSTitles(res->body, 3);
            for (auto& t : titles) state.headlines.push_back("[Crypto] " + t);
        } catch (...) {}
    }

    static void FetchRedditRSS(MarketState& state, const std::string& sub) {
        try {
            httplib::Client cli("https://www.reddit.com");
            cli.set_connection_timeout(5); cli.set_read_timeout(5);
            httplib::Headers h = {{"User-Agent", "AdEmpire/1.0"}};
            auto res = cli.Get(("/r/" + sub + "/hot.rss?limit=5").c_str(), h);
            if (!res || res->status != 200) return;
            auto titles = ExtractRSSTitles(res->body, 2);
            for (auto& t : titles) state.headlines.push_back("[" + sub + "] " + t);
        } catch (...) {}
    }

    static void FetchHackerNews(MarketState& state) {
        try {
            httplib::Client cli("https://hacker-news.firebaseio.com");
            cli.set_connection_timeout(5); cli.set_read_timeout(5);
            auto res = cli.Get("/v0/topstories.json");
            if (!res || res->status != 200) return;
            auto ids = nlohmann::json::parse(res->body);
            int aiCount = 0, fetched = 0;
            for (int i = 0; i < (int)ids.size() && fetched < 8; ++i) {
                int id = ids[i].get<int>();
                auto r2 = cli.Get(("/v0/item/" + std::to_string(id) + ".json").c_str());
                if (!r2 || r2->status != 200) continue;
                auto item = nlohmann::json::parse(r2->body);
                if (!item.contains("title")) continue;
                std::string title = item["title"].get<std::string>();
                std::string tl = title;
                std::transform(tl.begin(), tl.end(), tl.begin(), ::tolower);
                if (tl.find("ai") != std::string::npos ||
                    tl.find("llm") != std::string::npos ||
                    tl.find("gpt") != std::string::npos) aiCount++;
                if (fetched < 3) state.headlines.push_back("[HN] " + title);
                fetched++;
            }
            state.aiHypeActive = (aiCount >= 3);
        } catch (...) { state.aiHypeActive = false; }
    }

    // Simple keyword sentiment scoring
    static void ScoreSentiment(MarketState& state) {
        static const std::vector<std::string> bullish = {
            "surge","rally","bullish","record","growth","launch","boom",
            "adoption","breakout","partnership","milestone","profit"
        };
        static const std::vector<std::string> bearish = {
            "crash","drop","recession","bear","warning","risk","loss",
            "ban","hack","scam","collapse","fear","inflation","layoffs"
        };
        int score = 0;
        for (auto& h : state.headlines) {
            std::string hl = h;
            std::transform(hl.begin(), hl.end(), hl.begin(), ::tolower);
            for (auto& w : bullish) if (hl.find(w) != std::string::npos) score++;
            for (auto& w : bearish) if (hl.find(w) != std::string::npos) score--;
        }
        float total = (float)std::max((int)state.headlines.size(), 1);
        state.newsSentiment = std::max(-1.0f, std::min(1.0f, score / total));
    }
};
