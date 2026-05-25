#pragma once
// ── REST Leaderboard Client ──────────────────────────────────────────────────
// v0.8: Submits score to a REST API and fetches the global top-10.
// Uses cpp-httplib (header-only) — auto-downloaded by CMake if missing.
// API target: https://adempire-leaderboard.onrender.com (or local mock)
//
// Endpoints used:
//   POST /score   { "name": "...", "score": 12345, "months": 18 }
//   GET  /leaderboard  → JSON array [{rank,name,score,months}]
// ─────────────────────────────────────────────────────────────────────────────
#include <string>
#include <vector>
#include <functional>
#include <thread>
#include <mutex>
#include <atomic>
#include <sstream>

// Simple JSON snippet builders (avoid full nlohmann dep in header)
namespace LeaderboardJSON {
    inline std::string escape(const std::string& s) {
        std::string out; out.reserve(s.size());
        for (char c : s) {
            if (c == '"') out += "\\\"";
            else if (c == '\\') out += "\\\\";
            else out += c;
        }
        return out;
    }
    inline std::string makeEntry(const std::string& name, int score, int months) {
        std::ostringstream ss;
        ss << "{\"name\":\"" << escape(name)
           << "\",\"score\":" << score
           << ",\"months\":" << months << "}";
        return ss.str();
    }
}

struct LeaderEntry {
    int         rank    = 0;
    std::string name;
    int         score   = 0;
    int         months  = 0;
};

class LeaderboardClient {
public:
    static LeaderboardClient& Get() { static LeaderboardClient c; return c; }

    // ── Config ───────────────────────────────────────────────────────────────
    std::string host    = "adempire-leaderboard.onrender.com";
    int         port    = 443;
    bool        useTLS  = true;   // set false for local http mock
    std::string apiKey  = "";     // optional Bearer token

    // ── State ────────────────────────────────────────────────────────────────
    std::atomic<bool>     submitting { false };
    std::atomic<bool>     fetching   { false };
    std::atomic<bool>     submitOK   { false };
    std::atomic<bool>     fetchOK    { false };
    std::string           lastError;
    std::vector<LeaderEntry> entries;  // populated after FetchLeaderboard()
    std::mutex            entriesMutex;

    // ── Submit score (async) ─────────────────────────────────────────────────
    void SubmitScore(const std::string& playerName, int score, int months,
                     std::function<void(bool)> callback = nullptr) {
        if (submitting.load()) return;
        submitting.store(true);
        submitOK.store(false);
        std::string body = LeaderboardJSON::makeEntry(playerName, score, months);
        std::thread([this, body, callback]() {
            bool ok = PostHTTP("/score", body);
            submitOK.store(ok);
            submitting.store(false);
            if (callback) callback(ok);
        }).detach();
    }

    // ── Fetch top-10 (async) ─────────────────────────────────────────────────
    void FetchLeaderboard(std::function<void(bool)> callback = nullptr) {
        if (fetching.load()) return;
        fetching.store(true);
        fetchOK.store(false);
        std::thread([this, callback]() {
            std::string body;
            bool ok = GetHTTP("/leaderboard", body);
            if (ok) {
                auto parsed = ParseEntries(body);
                std::lock_guard<std::mutex> lk(entriesMutex);
                entries = std::move(parsed);
            }
            fetchOK.store(ok);
            fetching.store(false);
            if (callback) callback(ok);
        }).detach();
    }

    // ── Local mock (always available offline) ────────────────────────────────
    static std::vector<LeaderEntry> MockEntries() {
        return {
            {1, "AdKing",      985000, 12},
            {2, "MediaWizard", 871200, 14},
            {3, "CampaignAce", 762500, 16},
            {4, "BrandGuru",   654300, 18},
            {5, "AdEmpire",    543200, 20},
        };
    }

private:
    // ── HTTP helpers (cpp-httplib) ─────────────────────────────────────────
    // cpp-httplib is included via lib/httplib.h (auto-downloaded by CMake)
    bool PostHTTP(const std::string& path, const std::string& body) {
#ifdef HTTPLIB_AVAILABLE
        try {
#  ifdef CPPHTTPLIB_OPENSSL_SUPPORT
            httplib::SSLClient cli(host, port);
#  else
            httplib::Client cli(host, port);
#  endif
            cli.set_connection_timeout(5);
            cli.set_read_timeout(8);
            httplib::Headers hdrs;
            if (!apiKey.empty()) hdrs.emplace("Authorization", "Bearer " + apiKey);
            auto res = cli.Post(path.c_str(), hdrs, body, "application/json");
            if (!res || (res->status < 200 || res->status >= 300)) {
                lastError = res ? "HTTP " + std::to_string(res->status) : "connection failed";
                return false;
            }
            return true;
        } catch (const std::exception& e) {
            lastError = e.what();
            return false;
        }
#else
        (void)path; (void)body;
        lastError = "cpp-httplib not available";
        return false;
#endif
    }

    bool GetHTTP(const std::string& path, std::string& outBody) {
#ifdef HTTPLIB_AVAILABLE
        try {
#  ifdef CPPHTTPLIB_OPENSSL_SUPPORT
            httplib::SSLClient cli(host, port);
#  else
            httplib::Client cli(host, port);
#  endif
            cli.set_connection_timeout(5);
            cli.set_read_timeout(8);
            auto res = cli.Get(path.c_str());
            if (!res || (res->status < 200 || res->status >= 300)) {
                lastError = res ? "HTTP " + std::to_string(res->status) : "connection failed";
                return false;
            }
            outBody = res->body;
            return true;
        } catch (const std::exception& e) {
            lastError = e.what();
            return false;
        }
#else
        (void)path; (void)outBody;
        lastError = "cpp-httplib not available";
        return false;
#endif
    }

    // ── Simple JSON parser for [{rank,name,score,months}] ───────────────────
    static std::vector<LeaderEntry> ParseEntries(const std::string& json) {
        std::vector<LeaderEntry> out;
        // Minimal parser: scan for objects between { }
        size_t pos = 0;
        while (pos < json.size()) {
            size_t start = json.find('{', pos);
            if (start == std::string::npos) break;
            size_t end = json.find('}', start);
            if (end == std::string::npos) break;
            std::string obj = json.substr(start, end - start + 1);
            LeaderEntry e;
            e.rank   = ExtractInt(obj, "rank");
            e.name   = ExtractStr(obj, "name");
            e.score  = ExtractInt(obj, "score");
            e.months = ExtractInt(obj, "months");
            if (!e.name.empty()) out.push_back(e);
            pos = end + 1;
        }
        return out;
    }

    static int ExtractInt(const std::string& s, const std::string& key) {
        std::string search = "\"" + key + "\":";
        size_t p = s.find(search);
        if (p == std::string::npos) return 0;
        p += search.size();
        while (p < s.size() && s[p] == ' ') ++p;
        std::string num;
        while (p < s.size() && (std::isdigit(s[p]) || s[p] == '-')) num += s[p++];
        return num.empty() ? 0 : std::stoi(num);
    }

    static std::string ExtractStr(const std::string& s, const std::string& key) {
        std::string search = "\"" + key + "\":\"";
        size_t p = s.find(search);
        if (p == std::string::npos) return {};
        p += search.size();
        std::string val;
        while (p < s.size() && s[p] != '"') {
            if (s[p] == '\\' && p + 1 < s.size()) { ++p; val += s[p]; }
            else val += s[p];
            ++p;
        }
        return val;
    }
};
