#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

// ─── Channel Types ─────────────────────────────────────────────────────────
enum class ChannelType {
    Social, SEO, Email, Influencer, PR, PaidSearch
};
namespace std {
    template<> struct hash<ChannelType> {
        size_t operator()(ChannelType c) const noexcept {
            return hash<int>{}(static_cast<int>(c));
        }
    };
}
static const char* ChannelNames[] = {
    "Social Media", "SEO", "Email", "Influencer", "PR", "Paid Search"
};

// ─── Campaign ───────────────────────────────────────────────────────────────
struct Campaign {
    int         id;
    std::string name;
    int         clientId;
    ChannelType channel;
    float       budget;
    float       reach;
    float       ctr;
    float       conversionRate;
    float       revenue;
    float       agencyFee;
    int         durationMonths;
    int         monthsLeft;
    bool        active;
    bool        completed;
    float       qualityScore;
};

// ─── Client ─────────────────────────────────────────────────────────────────
enum class ClientIndustry {
    Food, Fashion, Tech, Finance, Health, Education, Retail, Gaming
};
static const char* IndustryNames[] = {
    "Food & Beverage", "Fashion", "Tech", "Finance",
    "Health", "Education", "Retail", "Gaming"
};

struct Client {
    int            id;
    std::string    name;
    ClientIndustry industry;
    float          budget;
    float          satisfaction;
    int            contractMonths;
    bool           active;
    bool           available;
    std::vector<int> campaignIds;
    float          totalRevenue;
    int            unlockMonth = 1; // month when client becomes pitchable
};

// ─── Staff ──────────────────────────────────────────────────────────────────
enum class StaffRole {
    SocialMediaManager, SEOSpecialist, ContentCreator,
    PRManager, DataAnalyst, AccountManager
};
static const char* RoleNames[] = {
    "Social Media Mgr", "SEO Specialist", "Content Creator",
    "PR Manager", "Data Analyst", "Account Manager"
};
static const float RoleSalaries[] = { 3500.f, 4500.f, 3000.f, 5000.f, 6000.f, 4000.f };
static const float RoleBonus[]    = { 0.15f, 0.20f, 0.10f, 0.25f, 0.30f, 0.15f };

// Staff level tiers (matches StaffLeveling.cpp)
static const char* LevelNames[] = { "Junior", "Mid", "Senior", "Lead", "Principal" };
static const float LevelThresholds[] = { 0.f, 0.35f, 0.55f, 0.75f, 0.90f };

struct StaffMember {
    int         id;
    std::string name;
    StaffRole   role;
    float       salary;
    float       skill;       // 0.0 – 1.0
    int         monthsHired;
    int         level = 0;   // 0=Junior … 4=Principal
};

// ─── AI Competitor ───────────────────────────────────────────────────────────
struct AIAgency {
    std::string name;
    float       marketShare;
    float       budget;
    float       reputation;
    float       aggressiveness;
    int         clientCount;
    std::string strategy; // "premium" | "volume" | "niche"
};

// ─── News Event ──────────────────────────────────────────────────────────────
struct NewsEvent {
    std::string title, description, impact;
    float socialMod, seoMod, emailMod, influencerMod, prMod, paidMod;
    float budgetImpact;
    int   durationMonths;
    bool  active;
    int   monthsLeft;
};

// ─── Achievement ─────────────────────────────────────────────────────────────
struct Achievement {
    std::string id;
    std::string name;
    std::string description;
    bool        unlocked = false;
};

// ─── Agency Stats ────────────────────────────────────────────────────────────
struct AgencyStats {
    float totalRevenue       = 0.f;
    float totalSpent         = 0.f;
    int   campaignsCompleted = 0;
    int   clientsAcquired    = 0;
    int   clientsLost        = 0;
    int   monthsPlayed       = 0;
    float bestMonthRevenue   = 0.f;
    float reputation         = 0.f;
};

// ─── v0.2: FitScore ──────────────────────────────────────────────────────────
struct FitScore {
    float channel    = 0.f;
    float industry   = 0.f;
    float reputation = 0.f;
    float capacity   = 0.f;
    float total() const { return channel + industry + reputation + capacity; }
};

// ─── v0.2: CapacityInfo ──────────────────────────────────────────────────────
struct CapacityInfo {
    int   maxClients     = 4;
    float utilizationPct = 0.f;
    bool  overloaded     = false;
    bool  burnoutRisk    = false;
};

// ─── v0.2: Quarterly Goals ───────────────────────────────────────────────────
enum class GoalType { Revenue, ClientCount, MarketShare, CampaignCount, Reward };
struct QuarterlyGoal {
    GoalType    type;
    float       target;
    float       current;
    float       reward;
    int         quarter;
    bool        completed;
    bool        failed;
};

// ─── v0.2: Negotiation ───────────────────────────────────────────────────────
enum class NegotiationStage {
    Intro, BudgetDiscussion, ChannelSelection, ContractTerms, FinalOffer, Closed
};
struct NegotiationState {
    NegotiationStage stage         = NegotiationStage::Intro;
    float            clientMood    = 60.f;
    int              pressure      = 0;
    float            offeredBudget = 0.f;
    bool             won           = false;
    bool             closed        = false;
};

// ─── v0.2: Specialization ────────────────────────────────────────────────────
struct Specialization {
    ClientIndustry industry;
    float          bonusMultiplier;
    int            clientsRequired;
    int            clientsServed;
    bool           unlocked;
};

// ─── v0.5: Campaign Template ─────────────────────────────────────────────────
struct CampaignTemplate {
    std::string    name;
    ChannelType    channel;
    float          suggestedBudget;
    float          qualityBonus;   // added to qualityScore
    ClientIndustry bestFor;
    std::string    description;
};

// ─── v0.5: Save Slot ─────────────────────────────────────────────────────────
struct SaveSlotMeta {
    bool        occupied    = false;
    std::string agencyName;
    float       budget      = 0.f;
    float       marketShare = 0.f;
    int         month       = 0;
    int         year        = 0;
    int         clients     = 0;
};

// ─── Main GameState ──────────────────────────────────────────────────────────
struct GameState {
    // Agency info
    std::string agencyName        = "My Agency";
    float       budget            = 10000.f;
    float       monthlyRevenue    = 0.f;
    float       monthlyExpenses   = 0.f;
    int         month             = 1;
    int         year              = 2024;

    // Collections
    std::vector<Client>         clients;
    std::vector<Campaign>       campaigns;
    std::vector<StaffMember>    staff;
    std::vector<AIAgency>       competitors;
    std::vector<NewsEvent>      activeEvents;
    std::vector<QuarterlyGoal>  quarterlyGoals;
    std::vector<Specialization> specializations;
    std::vector<Achievement>    achievements;

    // Channel modifiers (rebuilt each month from active events)
    std::unordered_map<ChannelType, float> channelModifiers;

    // Stats
    AgencyStats stats;

    // v0.2 systems
    CapacityInfo     capacity;
    NegotiationState negotiation;

    // Market
    float playerMarketShare = 2.f;

    // Auto-increment IDs
    int nextClientId   = 1;
    int nextCampaignId = 1;
    int nextStaffId    = 1;

    // ── UI flags (all panel visibility) ──────────────────────────────────────
    bool showDashboard       = true;
    bool showCampaigns       = false;
    bool showClients         = false;
    bool showStaff           = false;
    bool showMarketMap       = false;
    bool showNewsfeed        = false;
    bool showReport          = false;
    bool showAchievements    = false;  // v0.5
    bool showGoals           = false;
    bool showSpecializations = false;
    bool showNegotiation     = false;
    bool showTemplates       = false;  // v0.5
    bool showSaveSlots       = false;  // v0.5
    bool gameOver            = false;
    bool victory             = false;
};
