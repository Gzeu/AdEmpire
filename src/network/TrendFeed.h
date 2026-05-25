#pragma once
#include "MarketState.h"
#include "httplib.h"
#include "json.hpp"
#include <string>
#include <vector>

// ============================================================
//  TrendFeed
//  Source: Wikipedia Trending (HTTPS, zero auth)
//  Endpoint: en.wikipedia.org/w/api.php — mostviewed articles
// ============================================================

class TrendFeed {
public:
    static void Fetch(MarketState& state) {
        try {
            httplib::SSLClient cli("en.wikipedia.org", 443);
            cli.set_connection_timeout(6);
            cli.set_read_timeout(6);
            cli.enable_server_certificate_verification(true);
            const char* path =
                "/w/api.php?action=query&list=mostviewed"
                "&pvimdays=1&pvimlimit=15&format=json";
            auto res = cli.Get(path);
            if (!res || res->status != 200) return;
            auto j = nlohmann::json::parse(res->body);
            auto& items = j["query"]["mostviewed"];
            state.trendingKeywords.clear();
            for (auto& it : items) {
                std::string title = it["title"].get<std::string>();
                // Skip meta/utility pages
                if (title.rfind("Main_Page", 0) == 0) continue;
                if (title.rfind("Special:", 0) == 0)  continue;
                if (title.rfind("Wikipedia:", 0) == 0) continue;
                // Replace underscores
                for (char& c : title) if (c == '_') c = ' ';
                state.trendingKeywords.push_back(title);
                if ((int)state.trendingKeywords.size() >= 8) break;
            }
        } catch (...) {
            // Fallback keywords
            state.trendingKeywords = { "AI", "Tech", "Markets", "Finance", "Gaming" };
        }
    }
};
