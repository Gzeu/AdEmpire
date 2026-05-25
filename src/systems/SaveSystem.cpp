#include "SaveSystem.h"
#include <fstream>
#include <iostream>

// We use nlohmann/json — auto-downloaded by CMake to lib/json.hpp
#include "json.hpp"
using json = nlohmann::json;

// ───────────── Helpers ───────────────────────────────────────────────────
static json clientToJson(const Client& c) {
    return json{
        {"id",            c.id},
        {"name",          c.name},
        {"industry",      (int)c.industry},
        {"budget",        c.budget},
        {"satisfaction",  c.satisfaction},
        {"contractMonths",c.contractMonths},
        {"active",        c.active},
        {"available",     c.available},
        {"totalRevenue",  c.totalRevenue},
        {"campaignIds",   c.campaignIds}
    };
}
static Client clientFromJson(const json& j) {
    Client c;
    c.id            = j.at("id");
    c.name          = j.at("name").get<std::string>();
    c.industry      = (ClientIndustry)(int)j.at("industry");
    c.budget        = j.at("budget");
    c.satisfaction  = j.at("satisfaction");
    c.contractMonths= j.at("contractMonths");
    c.active        = j.at("active");
    c.available     = j.at("available");
    c.totalRevenue  = j.value("totalRevenue", 0.f);
    if (j.contains("campaignIds"))
        c.campaignIds = j.at("campaignIds").get<std::vector<int>>();
    return c;
}

static json campaignToJson(const Campaign& c) {
    return json{
        {"id",             c.id},
        {"name",           c.name},
        {"clientId",       c.clientId},
        {"channel",        (int)c.channel},
        {"budget",         c.budget},
        {"reach",          c.reach},
        {"ctr",            c.ctr},
        {"conversionRate", c.conversionRate},
        {"revenue",        c.revenue},
        {"agencyFee",      c.agencyFee},
        {"durationMonths", c.durationMonths},
        {"monthsLeft",     c.monthsLeft},
        {"active",         c.active},
        {"completed",      c.completed},
        {"qualityScore",   c.qualityScore}
    };
}
static Campaign campaignFromJson(const json& j) {
    Campaign c;
    c.id             = j.at("id");
    c.name           = j.at("name").get<std::string>();
    c.clientId       = j.at("clientId");
    c.channel        = (ChannelType)(int)j.at("channel");
    c.budget         = j.at("budget");
    c.reach          = j.value("reach", 0.f);
    c.ctr            = j.value("ctr", 0.f);
    c.conversionRate = j.value("conversionRate", 0.f);
    c.revenue        = j.value("revenue", 0.f);
    c.agencyFee      = j.value("agencyFee", 0.f);
    c.durationMonths = j.at("durationMonths");
    c.monthsLeft     = j.at("monthsLeft");
    c.active         = j.at("active");
    c.completed      = j.at("completed");
    c.qualityScore   = j.value("qualityScore", 5.f);
    return c;
}

static json staffToJson(const StaffMember& s) {
    return json{
        {"id",          s.id},
        {"name",        s.name},
        {"role",        (int)s.role},
        {"salary",      s.salary},
        {"skill",       s.skill},
        {"monthsHired", s.monthsHired}
    };
}
static StaffMember staffFromJson(const json& j) {
    StaffMember s;
    s.id          = j.at("id");
    s.name        = j.at("name").get<std::string>();
    s.role        = (StaffRole)(int)j.at("role");
    s.salary      = j.at("salary");
    s.skill       = j.at("skill");
    s.monthsHired = j.value("monthsHired", 0);
    return s;
}

// ───────────── Save ───────────────────────────────────────────────────
bool SaveSystem::Save(const GameState& gs, const std::string& path) {
    try {
        json j;
        j["agencyName"]        = gs.agencyName;
        j["budget"]            = gs.budget;
        j["month"]             = gs.month;
        j["year"]              = gs.year;
        j["playerMarketShare"] = gs.playerMarketShare;
        j["nextClientId"]      = gs.nextClientId;
        j["nextCampaignId"]    = gs.nextCampaignId;
        j["nextStaffId"]       = gs.nextStaffId;
        // Stats
        j["stats"] = {
            {"totalRevenue",       gs.stats.totalRevenue},
            {"totalSpent",         gs.stats.totalSpent},
            {"campaignsCompleted", gs.stats.campaignsCompleted},
            {"clientsAcquired",    gs.stats.clientsAcquired},
            {"clientsLost",        gs.stats.clientsLost},
            {"monthsPlayed",       gs.stats.monthsPlayed},
            {"bestMonthRevenue",   gs.stats.bestMonthRevenue},
            {"reputation",         gs.stats.reputation}
        };
        // Collections
        json jClients = json::array();
        for (auto& c : gs.clients)   jClients.push_back(clientToJson(c));
        j["clients"] = jClients;

        json jCamps = json::array();
        for (auto& c : gs.campaigns) jCamps.push_back(campaignToJson(c));
        j["campaigns"] = jCamps;

        json jStaff = json::array();
        for (auto& s : gs.staff)     jStaff.push_back(staffToJson(s));
        j["staff"] = jStaff;

        // Channel modifiers
        json jMods = json::object();
        for (auto& [k,v] : gs.channelModifiers)
            jMods[std::to_string((int)k)] = v;
        j["channelModifiers"] = jMods;

        std::ofstream f(path);
        if (!f.is_open()) return false;
        f << j.dump(2);
        return true;
    } catch (std::exception& e) {
        std::cerr << "[SaveSystem] Save error: " << e.what() << std::endl;
        return false;
    }
}

// ───────────── Load ───────────────────────────────────────────────────
bool SaveSystem::Load(GameState& gs, const std::string& path) {
    try {
        std::ifstream f(path);
        if (!f.is_open()) return false;
        json j = json::parse(f);

        gs.agencyName        = j.at("agencyName").get<std::string>();
        gs.budget            = j.at("budget");
        gs.month             = j.at("month");
        gs.year              = j.at("year");
        gs.playerMarketShare = j.at("playerMarketShare");
        gs.nextClientId      = j.value("nextClientId",  100);
        gs.nextCampaignId    = j.value("nextCampaignId", 100);
        gs.nextStaffId       = j.value("nextStaffId",   100);

        if (j.contains("stats")) {
            auto& s = j["stats"];
            gs.stats.totalRevenue       = s.value("totalRevenue",       0.f);
            gs.stats.totalSpent         = s.value("totalSpent",         0.f);
            gs.stats.campaignsCompleted = s.value("campaignsCompleted", 0);
            gs.stats.clientsAcquired    = s.value("clientsAcquired",    0);
            gs.stats.clientsLost        = s.value("clientsLost",        0);
            gs.stats.monthsPlayed       = s.value("monthsPlayed",       0);
            gs.stats.bestMonthRevenue   = s.value("bestMonthRevenue",   0.f);
            gs.stats.reputation         = s.value("reputation",         0.f);
        }

        gs.clients.clear();
        if (j.contains("clients"))
            for (auto& jc : j["clients"])
                gs.clients.push_back(clientFromJson(jc));

        gs.campaigns.clear();
        if (j.contains("campaigns"))
            for (auto& jc : j["campaigns"])
                gs.campaigns.push_back(campaignFromJson(jc));

        gs.staff.clear();
        if (j.contains("staff"))
            for (auto& js : j["staff"])
                gs.staff.push_back(staffFromJson(js));

        gs.channelModifiers.clear();
        if (j.contains("channelModifiers"))
            for (auto& [k, v] : j["channelModifiers"].items())
                gs.channelModifiers[(ChannelType)std::stoi(k)] = v.get<float>();

        return true;
    } catch (std::exception& e) {
        std::cerr << "[SaveSystem] Load error: " << e.what() << std::endl;
        return false;
    }
}

bool SaveSystem::HasSave(const std::string& path) {
    std::ifstream f(path);
    return f.good();
}
