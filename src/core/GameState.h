#pragma once
#include <string>
#include <vector>
#include <unordered_map>

// ─── Channel Types ───────────────────────────────────────────────
enum class ChannelType {
    Social,
    SEO,
    Email,
    Influencer,
    PR,
    PaidSearch
};

static const char* ChannelNames[] = {
    "Social Media", "SEO", "Email", "Influencer", "PR", "Paid Search"
};

// ─── Campaign ────────────────────────────────────────────────────
struct Campaign {
    int         id;
    std::string name;
    int         clientId;
    ChannelType channel;
    float       budget;       // $ allocated
    float       reach;        // people reached
    float       ctr;          // click-through rate 0-1
    float       conversionRate;
    float       revenue;      // generated for client
    float       agencyFee;    // 15% of revenue
    int         durationMonths;
    int         monthsLeft;
    bool        active;
    bool        completed;
    // Performance modifiers
    float       qualityScore; // 0-10, affects results
};

// ─── Client ──────────────────────────────────────────────────────
enum class ClientIndustry {
    Food, Fashion, Tech, Finance, Health, Education, Retail, Gaming
};

static const char* IndustryNames[] = {
    "Food & Beverage", "Fashion", "Tech", "Finance",
    "Health", "Education", "Retail", "Gaming"
};

struct Client {
    int           id;
    std::string   name;
    ClientIndustry industry;
    float         budget;          // monthly ad budget
    float         satisfaction;    // 0-100
    int           contractMonths;  // remaining months
    bool          active;
    bool          available;       // can be pitched to
    std::vector<int> campaignIds;
    float         totalRevenue;    // historical revenue
};

// ─── Staff Member ────────────────────────────────────────────────
enum class StaffRole {
    SocialMediaManager,
    SEOSpecialist,
    ContentCreator,
    PRManager,
    DataAnalyst,
    AccountManager
};

static const char* RoleNames[] = {
    "Social Media Mgr", "SEO Specialist", "Content Creator",
    "PR Manager", "Data Analyst", "Account Manager"
};

static const float RoleSalaries[] = {
    3500.f, 4500.f, 3000.f, 5000.f, 6000.f, 4000.f
};

static const float RoleBonus[] = {
    0.15f, 0.20f, 0.10f, 0.25f, 0.30f, 0.15f
};

struct StaffMember {
    int         id;
    std::string name;
    StaffRole   role;
    float       salary;
    float       skill;     // 0-1, affects campaign quality
    int         monthsHired;
};

// ─── AI Competitor ───────────────────────────────────────────────
struct AIAgency {
    std::string name;
    float       marketShare;   // 0-100 %
    float       budget;
    float       reputation;    // 0-100
    float       aggressiveness; // 0-1, how often they poach clients
    int         clientCount;
    std::string strategy;      // "premium", "volume", "niche"
};

// ─── News Event ──────────────────────────────────────────────────
struct NewsEvent {
    std::string title;
    std::string description;
    std::string impact;       // human-readable impact
    float       socialMod;    // multiplier on social campaigns
    float       seoMod;
    float       emailMod;
    float       influencerMod;
    float       prMod;
    float       paidMod;
    float       budgetImpact; // +/- flat on player budget
    int         durationMonths;
    bool        active;
    int         monthsLeft;
};

// ─── Agency Stats (player) ───────────────────────────────────────
struct AgencyStats {
    float totalRevenue;
    float totalSpent;
    int   campaignsCompleted;
    int   clientsAcquired;
    int   clientsLost;
    int   monthsPlayed;
    float bestMonthRevenue;
    float reputation;  // 0-100, grows with success
};

// ─── Main Game State ─────────────────────────────────────────────
struct GameState {
    // Agency info
    std::string agencyName = "My Agency";
    float       budget     = 10000.f;
    float       monthlyRevenue  = 0.f;
    float       monthlyExpenses = 0.f;
    int         month = 1;
    int         year  = 2024;

    // Collections
    std::vector<Client>      clients;
    std::vector<Campaign>    campaigns;
    std::vector<StaffMember> staff;
    std::vector<AIAgency>    competitors;
    std::vector<NewsEvent>   activeEvents;

    // Channel modifiers (affected by events)
    std::unordered_map<ChannelType, float> channelModifiers;

    // Stats
    AgencyStats stats;

    // UI state flags
    bool showDashboard    = true;
    bool showCampaigns    = false;
    bool showClients      = false;
    bool showStaff        = false;
    bool showMarketMap    = false;
    bool showNewsfeed     = false;
    bool showSettings     = false;
    bool gameOver         = false;
    bool victory          = false;

    // Market share
    float playerMarketShare = 2.f; // start at 2%

    // Next IDs
    int nextClientId   = 1;
    int nextCampaignId = 1;
    int nextStaffId    = 1;
};
