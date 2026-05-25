#pragma once
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <algorithm>

// ============================================================
//  NewsFeed.h  —  AdEmpire v1.0
//  Pulls live headlines from:
//    CoinDesk RSS  → crypto/finance headlines
//    Reddit RSS    → r/marketing + r/digitalmarketing
//    Hacker News   → Firebase API top 10 → AI hype score
//  Zero API keys. All public endpoints.
//  Thread-safe singleton with 10-minute refresh.
// ============================================================

#if __has_include("httplib.h")
  #include "httplib.h"
  #define NF_HAS_HTTP 1
  #define NF_HAS_HTTP 1
#else
  #define NF_HAS_HTTP 0
#endif

class NewsFeed {
public:
    static NewsFeed& Get() {
        static NewsFeed inst;
        return inst;
    }

    void StartAsync() {
        if (running_.exchange(true)) return;
        worker_ = std::thread([this]() {
            while (running_) {
                Fetch();
                // refresh every 10 minutes
                for (int i = 0; i < 600 && running_; ++i)
                    std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        });
        worker_.detach();
    }

    void FetchAsync() {
        std::thread([this]{ Fetch(); }).detach();
    }

    std::vector<std::string> GetHeadlines() const {
        std::lock_guard<std::mutex> lk(mx_);
        return coindesk_;
    }
    std::vector<std::string> GetRedditHeadlines() const {
        std::lock_guard<std::mutex> lk(mx_);
        return reddit_;
    }
    std::vector<std::string> GetHackerNewsHeadlines() const {
        std::lock_guard<std::mutex> lk(mx_);
        return hn_;
    }
    int GetAiHypeScore() const { return aiHype_.load(); }

private:
    NewsFeed() = default;
    ~NewsFeed() { running_ = false; }

    mutable std::mutex mx_;
    std::thread worker_;
    std::atomic<bool> running_{false};
    std::atomic<int>  aiHype_{0};

    std::vector<std::string> coindesk_;
    std::vector<std::string> reddit_;
    std::vector<std::string> hn_;

    // ── XML title extractor (simple, no full XML parser needed)
    static std::vector<std::string> ExtractTitles(const std::string& xml, int maxItems = 8) {
        std::vector<std::string> out;
        size_t pos = 0;
        int count = 0;
        while (count < maxItems) {
            size_t s = xml.find("<title>", pos);
            if (s == std::string::npos) break;
            s += 7;
            size_t e = xml.find("</title>", s);
            if (e == std::string::npos) break;
            std::string t = xml.substr(s, e - s);
            // strip CDATA
            if (t.rfind("<![CDATA[", 0) == 0) {
                t = t.substr(9);
                if (!t.empty() && t.back() == ']') t.pop_back();
                if (!t.empty() && t.back() == ']') t.pop_back();
            }
            if (!t.empty() && t.find("CoinDesk") == std::string::npos &&
                t.find("reddit") == std::string::npos) {
                out.push_back(t);
                ++count;
            }
            pos = e + 8;
        }
        return out;
    }

    static bool ContainsAI(const std::string& s) {
        std::string lo = s;
        std::transform(lo.begin(), lo.end(), lo.begin(), ::tolower);
        return lo.find(" ai ") != std::string::npos ||
               lo.find("openai")  != std::string::npos ||
               lo.find("llm")     != std::string::npos ||
               lo.find("gpt")     != std::string::npos ||
               lo.find("claude")  != std::string::npos ||
               lo.find("gemini")  != std::string::npos ||
               lo.find("machine learning") != std::string::npos;
    }

    void Fetch() {
#if NF_HAS_HTTP
        // 1. CoinDesk RSS
        {
            httplib::Client cli("http://www.coindesk.com");
            cli.set_connection_timeout(4);
            cli.set_read_timeout(5);
            auto res = cli.Get("/arc/outboundfeeds/rss/");
            if (res && res->status == 200) {
                auto titles = ExtractTitles(res->body, 8);
                std::lock_guard<std::mutex> lk(mx_);
                coindesk_ = titles;
            }
        }
        // 2. Reddit RSS — r/marketing
        {
            httplib::Client cli("http://www.reddit.com");
            cli.set_connection_timeout(4);
            cli.set_read_timeout(5);
            cli.set_default_headers({{"User-Agent", "AdEmpire/1.0"}});
            auto res = cli.Get("/r/marketing.rss?limit=6");
            if (res && res->status == 200) {
                auto t1 = ExtractTitles(res->body, 4);
                auto res2 = cli.Get("/r/digitalmarketing.rss?limit=6");
                if (res2 && res2->status == 200) {
                    auto t2 = ExtractTitles(res2->body, 4);
                    for (auto& x : t2) t1.push_back(x);
                }
                std::lock_guard<std::mutex> lk(mx_);
                reddit_ = t1;
            }
        }
        // 3. Hacker News top stories
        {
            httplib::Client cli("hacker-news.firebaseio.com");
            cli.set_connection_timeout(3);
            cli.set_read_timeout(4);
            auto res = cli.Get("/v0/topstories.json?limitToFirst=10&orderBy=\"$key\"");
            if (res && res->status == 200) {
                // parse array of IDs
                std::string body = res->body;
                std::vector<int> ids;
                size_t p = 0;
                while (ids.size() < 10) {
                    p = body.find_first_of("0123456789", p);
                    if (p == std::string::npos) break;
                    ids.push_back(std::stoi(body.substr(p)));
                    p = body.find(',', p);
                    if (p == std::string::npos) break;
                }
                std::vector<std::string> hnTitles;
                int hype = 0;
                for (int id : ids) {
                    char path[64];
                    snprintf(path, sizeof(path), "/v0/item/%d.json", id);
                    auto ir = cli.Get(path);
                    if (ir && ir->status == 200) {
                        // extract "title":"..."
                        auto& b = ir->body;
                        size_t ts = b.find("\"title\":\"");
                        if (ts != std::string::npos) {
                            ts += 9;
                            size_t te = b.find('"', ts);
                            if (te != std::string::npos) {
                                std::string title = b.substr(ts, te - ts);
                                hnTitles.push_back(title);
                                if (ContainsAI(title)) hype += 30;
                            }
                        }
                    }
                }
                aiHype_.store(hype);
                std::lock_guard<std::mutex> lk(mx_);
                hn_ = hnTitles;
            }
        }
#endif
    }
};
