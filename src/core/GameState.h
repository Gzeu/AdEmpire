#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

// ════════════════════════════════════════════════════════════
//  CHANNEL
// ════════════════════════════════════════════════════════════
enum class ChannelType {
    Social, SEO, Email, Influencer, PR, PaidSearch
};
static const char* ChannelNames[] = {
    "Social Media", "SEO", "Email", "Influencer", "PR", "Paid Search"
};
namespace std {
    template<> struct hash<ChannelType> {
        size_t operator()(ChannelType c) const noexcept {
            return hash<int>{}(static_cast<int>(c));
        }
    };
}

// ════════════════════════════════════════════════════════════
//  CLIENT ARCHETYPE  (v0.2)
// ════════════════════════════════════════════════════════════
enum class ClientArchetype {
    QuickWin,     // small budget, short contract, easy to satisfy
    GrowthHunter, // medium budget, wants fast reach growth
    BrandBuilder, // large budget, long contract, wants reputation
    ROIObsessed,  // cares only about conversion numbers
    InnovationSeeker // wants newest channels (Influencer/AI)
};
static const char* ArchetypeNames[] = {
    "Quick Win", "Growth Hunter", "Brand Builder",
    "ROI Obsessed", "Innovation Seeker"
};
static const char* ArchetypeDesc[] = {
    "Small, fast deals. Easy satisfaction.",
    "Wants reach above all. Medium commitment.",
    "Long-term brand investment. High budget.",
    "Only cares about ROI %. Demanding.",
    "Wants Influencer or cutting-edge channels."
};

// ════════════════════════════════════════════════════════════
//  CONTRACT TYPE  (v0.2)
// ════════════════════════════════════════════════════════════
enum class ContractType {
    Monthly,     // 1 month, low commitment
    Quarterly,   // 3 months, 5% discount for client
    Annual,      // 12 months, 10% discount, high LTV
    ProjectBased // fixed scope, one-time fee
};
static const char* ContractTypeNames[] = {
    "Monthly", "Quarterly", "Annual", "Project-Based"
};
static const int ContractDurations[] = { 1, 3, 12, 2 };
static const float ContractDiscounts[] = { 0.f, 0.05f, 0.10f, 0.f };

// ════════════════════════════════════════════════════════════
//  FIT SCORE  (v0.2)
// ════════════════════════════════════════════════════════════
struct FitScore {
    float channel;   // 0-1 how well our channel mix matches client
    float industry;  // 0-1 industry experience bonus
    float reputation;// 0-1 reputation threshold met
    float capacity;  // 0-1 do we have enough staff capacity
    float total() const { return (channel + industry + reputation + capacity) * 25.f; } // 0-100
};

// ════════════════════════════════════════════════════════════
//  CAMPAIGN
// ════════════════════════════════════════════════════════════
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

// ════════════════════════════════════════════════════════════
//  CLIENT INDUSTRY
// ════════════════════════════════════════════════════════════
enum class ClientIndustry {
    Food, Fashion, Tech, Finance, Health, Education, Retail, Gaming
};
static const char* IndustryNames[] = {
    "Food & Beverage", "Fashion", "Tech", "Finance",
    "Health", "Education", "Retail", "Gaming"
};
// Best channels per industry
static const ChannelType IndustryBestChannel[] = {
    ChannelType::Social,      // Food
    ChannelType::Influencer,  // Fashion
    ChannelType::SEO,         // Tech
    ChannelType::PaidSearch,  // Finance
    ChannelType::SEO,         // Health
    ChannelType::Email,       // Education
    ChannelType::PaidSearch,  // Retail
    ChannelType::Social       // Gaming
};
static const float IndustryChannelBonus = 1.30f; // +30% when best channel used

// ════════════════════════════════════════════════════════════
//  CLIENT  (v0.2 extended)
// ════════════════════════════════════════════════════════════
struct Client {
    int             id;
    std::string     name;
    ClientIndustry  industry;
    ClientArchetype archetype;    // NEW v0.2
    ContractType    contractType; // NEW v0.2
    float           budget;
    float           satisfaction; // 0-100
    int             contractMonths;
    bool            active;
    bool            available;
    std::vector<int> campaignIds;
    float           totalRevenue;
    // v0.2 fields
    float           riskModifier;  // 1.0 = normal, >1 = risky client
    bool            inNegotiation; // currently being negotiated
    int             industryExp;   // how many clients of same industry we've had
};

// ════════════════════════════════════════════════════════════
//  STAFF
// ════════════════════════════════════════════════════════════
enum class StaffRole {
    SocialMediaManager, SEOSpecialist, ContentCreator,
    PRManager, DataAnalyst, AccountManager
};
static const char* RoleNames[] = {
    "Social Media Mgr", "SEO Specialist", "Content Creator",
    "PR Manager", "Data Analyst", "Account Manager"
};
static const float RoleSalaries[] = { 3500.f, 4500.f, 3000.f, 5000.f, 6000.f, 4000.f };
static const float RoleBonus[]    = { 0.15f,  0.20f,  0.10f,  0.25f,  0.30f,  0.15f  };
static const int   RoleCapacity[] = { 3, 2, 4, 2, 3, 4 }; // clients per staff member

struct StaffMember {
    int         id;
    std::string name;
    StaffRole   role;
    float       salary;
    float       skill;        // 0-1
    int         monthsHired;
    float       burnout;      // 0-1, NEW v0.2 — rises with overload
    int         assignedClients; // NEW v0.2
};

// ════════════════════════════════════════════════════════════
//  CAPACITY & BURNOUT  (v0.2)
// ════════════════════════════════════════════════════════════
struct CapacityInfo {
    int   maxClients;      // sum of RoleCapacity for all staff
    int   currentClients;  // active clients
    float utilizationPct;  // 0-1
    bool  overloaded;      // >100% capacity
    float burnoutRisk;     // 0-1 agency-level
};

// ════════════════════════════════════════════════════════════
//  QUARTERLY GOALS  (v0.2)
// ════════════════════════════════════════════════════════════
enum class GoalType {
    RevenueTarget,
    ClientCount,
    MarketShare,
    CampaignsLaunched,
    SatisfactionAvg
};
static const char* GoalTypeNames[] = {
    "Revenue Target", "Client Count", "Market Share",
    "Campaigns Launched", "Avg Satisfaction"
};
struct QuarterlyGoal {
    GoalType    type;
    float       target;
    float       current;
    bool        completed;
    bool        failed;
    float       rewardBudget;  // cash reward on completion
    float       rewardReputation;
    int         quarterDue;    // which quarter (1-4)
    std::string description;
};

// ════════════════════════════════════════════════════════════
//  NEGOTIATION  (v0.2)
// ════════════════════════════════════════════════════════════
enum class NegotiationStage {
    Intro,
    BudgetDiscussion,
    ChannelSelection,
    ContractTerms,
    FinalOffer,
    Closed
};
struct NegotiationState {
    bool           active       = false;
    int            clientId     = -1;
    NegotiationStage stage      = NegotiationStage::Intro;
    float          offeredBudget = 0.f;
    ContractType   offeredContract = ContractType::Monthly;
    ChannelType    offeredChannel  = ChannelType::Social;
    int            playerPressure = 0; // 0-3, too high = client walks
    float          clientMood     = 0.7f; // 0-1
    FitScore       fitScore;
    bool           wonDeal       = false;
    bool           lostDeal      = false;
    std::string    lastMessage;
};

// ════════════════════════════════════════════════════════════
//  SPECIALIZATION TREE  (v0.2)
// ════════════════════════════════════════════════════════════
struct Specialization {
    std::string name;
    std::string description;
    ClientIndustry industry;
    float          bonusMultiplier; // applied to all campaigns for this industry
    int            clientsRequired; // how many clients of this industry to unlock
    bool           unlocked;
};

// ════════════════════════════════════════════════════════════
//  AI AGENCY
// ════════════════════════════════════════════════════════════
struct AIAgency {
    std::string name;
    float       marketShare;
    float       budget;
    float       reputation;
    float       aggressiveness;
    int         clientCount;
    std::string strategy;
    // v0.2
    std::string personality; // "aggressive", "premium", "steady"
    float       growthRate;
};

// ════════════════════════════════════════════════════════════
//  NEWS EVENT
// ════════════════════════════════════════════════════════════
struct NewsEvent {
    std::string title;
    std::string description;
    std::string impact;
    float       socialMod;
    float       seoMod;
    float       emailMod;
    float       influencerMod;
    float       prMod;
    float       paidMod;
    float       budgetImpact;
    int         durationMonths;
    bool        active;
    int         monthsLeft;
};

// ════════════════════════════════════════════════════════════
//  AGENCY STATS
// ════════════════════════════════════════════════════════════
struct AgencyStats {
    float totalRevenue         = 0.f;
    float totalSpent           = 0.f;
    int   campaignsCompleted   = 0;
    int   clientsAcquired      = 0;
    int   clientsLost          = 0;
    int   monthsPlayed         = 0;
    float bestMonthRevenue     = 0.f;
    float reputation           = 0.f;
    // v0.2
    int   negotiationsWon      = 0;
    int   negotiationsLost     = 0;
    int   quarterlyGoalsMet    = 0;
    std::unordered_map<int, int> industryClientCount; // industry → count
};

// ════════════════════════════════════════════════════════════
//  MAIN GAME STATE
// ════════════════════════════════════════════════════════════
struct GameState {
    std::string agencyName      = "My Agency";
    float       budget          = 10000.f;
    float       monthlyRevenue  = 0.f;
    float       monthlyExpenses = 0.f;
    int         month           = 1;
    int         year            = 2024;
    int         currentQuarter  = 1; // NEW v0.2

    std::vector<Client>         clients;
    std::vector<Campaign>       campaigns;
    std::vector<StaffMember>    staff;
    std::vector<AIAgency>       competitors;
    std::vector<NewsEvent>      activeEvents;
    std::vector<QuarterlyGoal>  quarterlyGoals;   // NEW v0.2
    std::vector<Specialization> specializations;  // NEW v0.2

    std::unordered_map<ChannelType, float> channelModifiers;

    AgencyStats      stats;
    CapacityInfo     capacity;       // NEW v0.2
    NegotiationState negotiation;    // NEW v0.2

    // UI flags
    bool showDashboard      = true;
    bool showCampaigns      = false;
    bool showClients        = false;
    bool showStaff          = false;
    bool showMarketMap      = false;
    bool showNewsfeed       = false;
    bool showNegotiation    = false; // NEW v0.2
    bool showGoals          = false; // NEW v0.2
    bool showSpecializations= false; // NEW v0.2
    bool showSettings       = false;
    bool showAchievements   = false;
    bool gameOver           = false;
    bool victory            = false;

    float playerMarketShare = 2.f;

    int nextClientId    = 1;
    int nextCampaignId  = 1;
    int nextStaffId     = 1;
};
