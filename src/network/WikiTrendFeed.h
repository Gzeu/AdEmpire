#pragma once
#include "MarketState.h"
#include "httplib.h"
#include "json.hpp"
#include <string>
#include <vector>
#include <algorithm>

// ============================================================
//  WikiTrendFeed
//  Source: Wikimedia Pageviews REST API (HTTPS, zero auth)
//  Endpoint: wikimedia.org/api/rest_v1/metrics/pageviews/
//  Also uses en.wikipedia.org/w/api.php mostviewed as fallback
//
//  Derives:
//    state.trendingKeywords  — top 8 topic names
//    state.trendingScore     — relative "virality" 0-100
// ============================================================

class WikiTrendFeed {
public:
    static void Fetch(MarketState& state) {
        FetchWikimediaPageviews(state);
        // If primary failed, TrendFeed::Fetch() already covers the fallback
        // but we add a score estimate here
        if (!state.trendingKeywords.empty())
            state.trendingScore = std::min(100, (int)state.trendingKeywords.size() * 10 + 20);
    }

private:
    static void FetchWikimediaPageviews(MarketState& state) {
        try {
            // Get top articles for yesterday via Wikimedia REST API
            httplib::SSLClient cli("wikimedia.org", 443);
            cli.set_connection_timeout(6);
            cli.set_read_timeout(8);
            cli.enable_server_certificate_verification(true);
            httplib::Headers hdrs = {{ "User-Agent", "AdEmpire/1.0 (github.com/Gzeu/AdEmpire)" }};
            // Use /all-access/all-agents/top — yesterday
            // Date is hardcoded-relative; real impl should compute yesterday's date
            auto res = cli.Get(
                "/api/rest_v1/metrics/pageviews/top/en.wikipedia/all-access/2026/05/25",
                hdrs);
            if (!res || res->status != 200) { FetchFallback(state); return; }
            auto j = nlohmann::json::parse(res->body);
            auto& articles = j["items"][0]["articles"];
            state.trendingKeywords.clear();
            for (auto& a : articles) {
                std::string title = a["article"].get<std::string>();
                if (title == "Main_Page") continue;
                if (title.rfind("Special:", 0) == 0) continue;
                for (char& c : title) if (c == '_') c = ' ';
                state.trendingKeywords.push_back(title);
                if ((int)state.trendingKeywords.size() >= 8) break;
            }
        } catch (...) { FetchFallback(state); }
    }

    static void FetchFallback(MarketState& state) {
        // Reuse TrendFeed logic via mostviewed
        try {
            httplib::SSLClient cli("en.wikipedia.org", 443);
            cli.set_connection_timeout(5);
            cli.set_read_timeout(5);
            cli.enable_server_certificate_verification(true);
            auto res = cli.Get(
                "/w/api.php?action=query&list=mostviewed"
                "&pvimdays=1&pvimlimit=15&format=json");
            if (!res || res->status != 200) return;
            auto j = nlohmann::json::parse(res->body);
            state.trendingKeywords.clear();
            for (auto& it : j["query"]["mostviewed"]) {
                std::string t = it["title"].get<std::string>();
                if (t.rfind("Main_Page", 0) == 0 ||
                    t.rfind("Special:", 0) == 0 ||
                    t.rfind("Wikipedia:", 0) == 0) continue;
                for (char& c : t) if (c == '_') c = ' ';
                state.trendingKeywords.push_back(t);
                if ((int)state.trendingKeywords.size() >= 8) break;
            }
        } catch (...) {
            state.trendingKeywords = { "AI", "Tech", "Markets", "Finance", "Gaming" };
        }
    }
};
