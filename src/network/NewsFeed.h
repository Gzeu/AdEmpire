#pragma once
#include "MarketState.h"
#include "httplib.h"
#include "json.hpp"
#include <string>
#include <vector>
#include <sstream>

// ============================================================
//  NewsFeed
//  Sources (HTTPS RSS, zero auth):
//    CoinDesk  — crypto news
//    Hacker News Firebase  — tech/AI news
//    Reddit RSS r/marketing, r/digitalmarketing
// ============================================================

class NewsFeed {
public:
    static void Fetch(MarketState& state) {
        state.newsItems.clear();
        FetchCoinDesk(state);
        FetchHackerNews(state);
        FetchReddit(state, "marketing");
        FetchReddit(state, "digitalmarketing");
    }

private:
    // Simple XML title extractor (no full XML parser needed)
    static std::vector<std::string> ExtractTitles(const std::string& xml, int maxItems = 5) {
        std::vector<std::string> titles;
        size_t pos = 0;
        int count = 0;
        while (count < maxItems) {
            size_t start = xml.find("<title>", pos);
            if (start == std::string::npos) break;
            start += 7;
            size_t end = xml.find("</title>", start);
            if (end == std::string::npos) break;
            std::string t = xml.substr(start, end - start);
            // Strip CDATA if present
            if (t.rfind("<![CDATA[", 0) == 0)
                t = t.substr(8, t.size() - 10);
            if (!t.empty() && t.size() < 200)
                titles.push_back(t);
            pos = end + 8;
            count++;
        }
        return titles;
    }

    static void FetchCoinDesk(MarketState& state) {
        try {
            httplib::SSLClient cli("www.coindesk.com", 443);
            cli.set_connection_timeout(5);
            cli.set_read_timeout(5);
            cli.enable_server_certificate_verification(true);
            auto res = cli.Get("/arc/outboundfeeds/rss/");
            if (!res || res->status != 200) return;
            for (auto& t : ExtractTitles(res->body, 4)) {
                NewsItem item;
                item.title = t;
                item.source = "CoinDesk";
                item.link = "";
                state.newsItems.push_back(item);
            }
        } catch (...) {}
    }

    static void FetchHackerNews(MarketState& state) {
        try {
            // HN Firebase API — returns array of story IDs, then fetch top 5
            httplib::SSLClient cli("hacker-news.firebaseio.com", 443);
            cli.set_connection_timeout(5);
            cli.set_read_timeout(8);
            cli.enable_server_certificate_verification(true);
            auto res = cli.Get("/v0/topstories.json");
            if (!res || res->status != 200) return;
            auto ids = nlohmann::json::parse(res->body);
            int fetched = 0;
            for (auto& id : ids) {
                if (fetched >= 4) break;
                std::string path = "/v0/item/" + std::to_string(id.get<int>()) + ".json";
                auto sr = cli.Get(path.c_str());
                if (!sr || sr->status != 200) continue;
                auto item = nlohmann::json::parse(sr->body);
                if (item.contains("title")) {
                    NewsItem ni;
                    ni.title = item["title"].get<std::string >();
                    ni.source = "HackerNews";
                    ni.link = "";
                    state.newsItems.push_back(ni);
                }
                fetched++;
            }
        } catch (...) {}
    }

    static void FetchReddit(MarketState& state, const std::string& sub) {
        try {
            httplib::SSLClient cli("www.reddit.com", 443);
            cli.set_connection_timeout(5);
            cli.set_read_timeout(5);
            cli.enable_server_certificate_verification(true);
            httplib::Headers headers = {{ "User-Agent", "AdEmpire/1.0" }};
            auto res = cli.Get(("/r/" + sub + ".rss").c_str(), headers);
            if (!res || res->status != 200) return;
            for (auto& t : ExtractTitles(res->body, 3)) {
                NewsItem item;
                item.title = t;
                item.source = "r/" + sub;
                item.link = "";
                state.newsItems.push_back(item);
            }
        } catch (...) {}
    }
};
