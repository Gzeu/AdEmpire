#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include "imgui.h"
#include "../systems/Leaderboard.h"

// ─── Channel Types ───────────────────────────────────────────────────────────────────────
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

// ─── Campaign ───────────────────────────────────────────────────────────────────────────
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

// ─── Client ────────────────────────────────────────────────────────────────────────────
enum class ContractType {
    Monthly, Quarterly, Annual
};

enum class ClientIndustry {
    Food, Fashion, Tech, Finance, Health, Education, Retail, Gaming
};
static const char* IndustryNames[] = {
    "Food & Beverage", "Fashion", "Tech", "Finance",
    "Health", "Education", "Retail", "Gaming"
};

static const int ContractDurations[] = { 6, 9, 12, 18 };

static const ChannelType IndustryBestChannel[] = {
    ChannelType::Social,    // Food
    ChannelType::Social,    // Fashion
    ChannelType::SEO,       // Tech
    ChannelType::PR,        // Finance
    ChannelType::PR,        // Health
    ChannelType::SEO,       // Education
    ChannelType::PaidSearch,// Retail
    ChannelType::Influencer // Gaming
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
    bool           inNegotiation = false;
    ContractType   contractType = ContractType::Monthly;
    std::vector<int> campaignIds;
    float          totalRevenue;
    int            unlockMonth = 1;
};

// ─── Staff ─────────────────────────────────────────────────────────────────────────────
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

static const char* LevelNames[]      = { "Junior", "Mid", "Senior", "Lead", "Principal" };
static const float LevelThresholds[] = { 0.f, 0.35f, 0.55f, 0.75f, 0.90f };

struct StaffMember {
    int         id;
    std::string name;
    StaffRole   role;
    float       salary;
    float       skill;
    int         monthsHired;
    int         level = 0;
};

// ─── AI Competitor (legacy stub kept for save compat) ─────────────────────────────────
struct AIAgency {
    std::string name;
    float       marketShare;
    float       budget;
    float       reputation;
    float       aggressiveness;
    int         clientCount;
    std::string strategy;
};

// ─── News Event ──────────────────────────────────────────────────────────────────────────
struct NewsEvent {
    std::string title, description, impact;
    float socialMod, seoMod, emailMod, influencerMod, prMod, paidMod;
    float budgetImpact;
    int   durationMonths;
    bool  active;
    int   monthsLeft;
};

// ─── v0.9: Game Event ───────────────────────────────────────────────────────────────────────
struct GameEvent {
    std::string id;
    std::string title;
    std::string description;
    std::string impact;
    float       budgetDelta      = 0.f;
    float       reputationDelta  = 0.f;
    float       marketShareDelta = 0.f;
    float socialMod     = 1.f;
    float seoMod        = 1.f;
    float emailMod      = 1.f;
    float influencerMod = 1.f;
    float prMod         = 1.f;
    float paidMod       = 1.f;
    int   durationMonths = 1;
};

// ─── Agency Stats ────────────────────────────────────────────────────────────────────────
struct AgencyStats {
    float totalRevenue       = 0.f;
    float totalSpent         = 0.f;
    int   campaignsCompleted = 0;
    int   clientsAcquired    = 0;
    int   clientsLost        = 0;
    int   monthsPlayed       = 0;
    float bestMonthRevenue   = 0.f;
    float reputation         = 0.f;
    int   negotiationsWon    = 0;
    int   negotiationsLost   = 0;
};

// ─── v0.2: FitScore ──────────────────────────────────────────────────────────────────────────
struct FitScore {
    float channel    = 0.f;
    float industry   = 0.f;
    float reputation = 0.f;
    float capacity   = 0.f;
    float total() const { return channel + industry + reputation + capacity; }
};

// ─── v0.2: CapacityInfo ───────────────────────────────────────────────────────────────────────
struct CapacityInfo {
    int   maxClients     = 4;
    float utilizationPct = 0.f;
    bool  overloaded     = false;
    bool  burnoutRisk    = false;
};

// ─── v0.2: Quarterly Goals ─────────────────────────────────────────────────────────────────────
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

// ─── v0.2: Negotiation ────────────────────────────────────────────────────────────────────────
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
    bool             active        = false;
    int              clientId      = -1;
    std::string      lastMessage;
    bool             lostDeal      = false;
    bool             wonDeal       = false;
    ContractType     offeredContract = ContractType::Monthly;
    ChannelType      offeredChannel  = ChannelType::Social;
    FitScore         fitScore;
    int              playerPressure  = 0;
};

// ─── v0.2: Specialization ──────────────────────────────────────────────────────────────────────
struct Specialization {
    ClientIndustry industry;
    float          bonusMultiplier;
    int            clientsRequired;
    int            clientsServed;
    bool           unlocked;
};

// ─── v0.5: Save Slot ───────────────────────────────────────────────────────────────────────────
struct SaveSlotMeta {
    bool        occupied    = false;
    std::string agencyName;
    float       budget      = 0.f;
    float       marketShare = 0.f;
    int         month       = 0;
    int         year        = 0;
    int         clients     = 0;
};

// ─── Achievement (forward declaration) ──────────────────────────────────────────────────────
struct Achievement;

// ─── v1.0: Contract Offer (forward declaration) ─────────────────────────────────────────────
struct ContractOffer;

// ─── v1.0: CashflowSnapshot (forward declaration) ───────────────────────────────────────────
struct CashflowSnapshot;

// ─── Main GameState ────────────────────────────────────────────────────────────────────────
struct GameState {
    // Agency info
    std::string agencyName      = "My Agency";
    float       budget          = 10000.f;
    float       monthlyRevenue  = 0.f;
    float       monthlyExpenses = 0.f;
    int         month           = 1;
    int         year            = 2024;

    // Collections
    std::vector<Client>         clients;
    std::vector<Campaign>       campaigns;
    std::vector<StaffMember>    staff;
    std::vector<AIAgency>       competitors;      // legacy
    std::vector<NewsEvent>      activeEvents;
    std::vector<QuarterlyGoal>  quarterlyGoals;
    std::vector<Specialization> specializations;
    std::vector<Achievement>    achievements;

    // Channel modifiers (rebuilt each month from active events)
    std::unordered_map<ChannelType, float> channelModifiers;

    // Stats
    AgencyStats stats;
    std::vector<LeaderboardEntry> leaderboard;

    // v0.2 systems
    CapacityInfo     capacity;
    NegotiationState negotiation;

    // Market
    float playerMarketShare = 2.f;

    // ── v1.0: Real-market revenue multiplier (set by MarketEventBridge each month)
    // Range: 0.3 (extreme bear/panic) – 2.0 (bull+euphoria)
    float revenueMultiplier = 1.0f;

    // Auto-increment IDs
    int nextClientId   = 1;
    int nextCampaignId = 1;
    int nextStaffId    = 1;

    // ── v0.9: Agency Branding
    ImVec4      agencyColor = ImVec4(1.f, 0.78f, 0.f, 1.f);
    std::string agencyLogo  = "Diamond";

    // ── v0.9: Event Popup gate
    bool        pendingEventPopup = false;
    GameEvent   currentEvent;

    // ── v1.0: Pending toasts from systems (shown by Dashboard next frame)
    std::vector<std::string> pendingToasts;

    // ── UI flags
    bool showDashboard       = true;
    bool showCampaigns       = false;
    bool showClients         = false;
    bool showStaff           = false;
    bool showMarketMap       = false;
    bool showNewsfeed        = false;
    bool showReport          = false;
    bool showAchievements    = false;
    bool showGoals           = false;
    bool showSpecializations = false;
    bool showNegotiation     = false;
    bool showTemplates       = false;
    bool showSaveSlots       = false;
    bool showLeaderboard     = false;
    bool showSettings        = false;
    bool showStats           = false;
    bool showContracts       = false;   // v1.0 NEW
    bool gameOver            = false;
    bool victory             = false;
};

// ─── Achievement ────────────────────────────────────────────────────────────────────────────
struct Achievement {
    int         id;
    std::string title;
    std::string description;
    std::string category;
    bool        unlocked;
    std::function<bool(const GameState&)> condition;
};
