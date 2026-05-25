// SaveSystem.cpp — full save/load using nlohmann/json (header-only, included via lib/json.hpp)
#include "SaveSystem.h"
#include "../../lib/json.hpp"
#include <fstream>
#include <iostream>

using json = nlohmann::json;

// ---- helpers ----
static json clientToJson(const Client& c) {
    return {
        {"id",             c.id},
        {"name",           c.name},
        {"industry",       (int)c.industry},
        {"budget",         c.budget},
        {"satisfaction",   c.satisfaction},
        {"contractMonths", c.contractMonths},
        {"active",         c.active},
        {"available",      c.available},
        {"totalRevenue",   c.totalRevenue},
        {"campaignIds",    c.campaignIds}
    };
}
static Client clientFromJson(const json& j) {
    Client c;
    c.id             = j.at("id");
    c.name           = j.at("name").get<std::string>();
    c.industry       = (ClientIndustry)(int)j.at("industry");
    c.budget         = j.at("budget");
    c.satisfaction   = j.at("satisfaction");
    c.contractMonths = j.at("contractMonths");
    c.active         = j.at("active");
    c.available      = j.at("available");
    c.totalRevenue   = j.at("totalRevenue");
    c.campaignIds    = j.at("campaignIds").get<std::vector<int>>();
    return c;
}

static json campaignToJson(const Campaign& c) {
    return {
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
    c.reach          = j.at("reach");
    c.ctr            = j.at("ctr");
    c.conversionRate = j.at("conversionRate");
    c.revenue        = j.at("revenue");
    c.agencyFee      = j.at("agencyFee");
    c.durationMonths = j.at("durationMonths");
    c.monthsLeft     = j.at("monthsLeft");
    c.active         = j.at("active");
    c.completed      = j.at("completed");
    c.qualityScore   = j.at("qualityScore");
    return c;
}

static json staffToJson(const StaffMember& s) {
    return {
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
    s.monthsHired = j.at("monthsHired");
    return s;
}

// ---- public API ----
bool SaveSystem::HasSave(const std::string& path) {
    std::ifstream f(path);
    return f.good();
}

bool SaveSystem::Save(const GameState& gs, const std::string& path) {
    try {
        json j;
        j["agencyName"]        = gs.agencyName;
        j["budget"]            = gs.budget;
        j["month"]             = gs.month;
        j["year"]              = gs.year;
        j["playerMarketShare"] = gs.playerMarketShare;
        j["monthlyRevenue"]    = gs.monthlyRevenue;
        j["monthlyExpenses"]   = gs.monthlyExpenses;
        j["nextClientId"]      = gs.nextClientId;
        j["nextCampaignId"]    = gs.nextCampaignId;
        j["nextStaffId"]       = gs.nextStaffId;
        // stats
        j["stats"] = {
            {"totalRevenue",        gs.stats.totalRevenue},
            {"totalSpent",          gs.stats.totalSpent},
            {"campaignsCompleted",  gs.stats.campaignsCompleted},
            {"clientsAcquired",     gs.stats.clientsAcquired},
            {"clientsLost",         gs.stats.clientsLost},
            {"monthsPlayed",        gs.stats.monthsPlayed},
            {"bestMonthRevenue",    gs.stats.bestMonthRevenue},
            {"reputation",          gs.stats.reputation}
        };
        // collections
        json clients = json::array();
        for (auto& c : gs.clients)   clients.push_back(clientToJson(c));
        j["clients"] = clients;

        json campaigns = json::array();
        for (auto& c : gs.campaigns) campaigns.push_back(campaignToJson(c));
        j["campaigns"] = campaigns;

        json staff = json::array();
        for (auto& s : gs.staff)     staff.push_back(staffToJson(s));
        j["staff"] = staff;

        std::ofstream f(path);
        if (!f.is_open()) return false;
        f << j.dump(2);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[SaveSystem] Save error: " << e.what() << std::endl;
        return false;
    }
}

bool SaveSystem::Load(GameState& gs, const std::string& path) {
    try {
        std::ifstream f(path);
        if (!f.is_open()) return false;
        json j;
        f >> j;

        gs.agencyName        = j.at("agencyName").get<std::string>();
        gs.budget            = j.at("budget");
        gs.month             = j.at("month");
        gs.year              = j.at("year");
        gs.playerMarketShare = j.at("playerMarketShare");
        gs.monthlyRevenue    = j.at("monthlyRevenue");
        gs.monthlyExpenses   = j.at("monthlyExpenses");
        gs.nextClientId      = j.at("nextClientId");
        gs.nextCampaignId    = j.at("nextCampaignId");
        gs.nextStaffId       = j.at("nextStaffId");

        auto& st = gs.stats;
        st.totalRevenue       = j["stats"]["totalRevenue"];
        st.totalSpent         = j["stats"]["totalSpent"];
        st.campaignsCompleted = j["stats"]["campaignsCompleted"];
        st.clientsAcquired    = j["stats"]["clientsAcquired"];
        st.clientsLost        = j["stats"]["clientsLost"];
        st.monthsPlayed       = j["stats"]["monthsPlayed"];
        st.bestMonthRevenue   = j["stats"]["bestMonthRevenue"];
        st.reputation         = j["stats"]["reputation"];

        gs.clients.clear();
        for (auto& jc : j["clients"])   gs.clients.push_back(clientFromJson(jc));
        gs.campaigns.clear();
        for (auto& jc : j["campaigns"]) gs.campaigns.push_back(campaignFromJson(jc));
        gs.staff.clear();
        for (auto& js : j["staff"])     gs.staff.push_back(staffFromJson(js));

        return true;
    } catch (const std::exception& e) {
        std::cerr << "[SaveSystem] Load error: " << e.what() << std::endl;
        return false;
    }
}
