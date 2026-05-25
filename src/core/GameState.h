#pragma once
#include <string>
#include <vector>
#include <unordered_map>

enum class ChannelType {
    Social = 0, SEO, Email, Influencer, PR, PaidSearch
};

static const char* ChannelNames[] = {
    "Social Media", "SEO", "Email", "Influencer", "PR", "Paid Search"
};

struct Campaign {
    int         id             = 0;
    std::string name;
    int         clientId       = 0;
    ChannelType channel        = ChannelType::Social;
    float       budget         = 0.f;
    float       reach          = 0.f;
    float       ctr            = 0.f;
    float       conversionRate = 0.f;
    float       revenue        = 0.f;
    float       agencyFee      = 0.f;
    int         durationMonths = 1;
    int         monthsLeft     = 1;
    bool        active         = false;
    bool        completed      = false;
    float       qualityScore   = 5.f;
};

enum class ClientIndustry {
    Food = 0, Fashion, Tech, Finance, Health, Education, Retail, Gaming
};

static const char* IndustryNames[] = {
    "Food & Beverage", "Fashion", "Tech", "Finance",
    "Health", "Education", "Retail", "Gaming"
};

struct Client {
    int            id             = 0;
    std::string    name;
    ClientIndustry industry       = ClientIndustry::Tech;
    float          budget         = 0.f;
    float          satisfaction   = 70.f;
    int            contractMonths = 6;
    bool           active         = false;
    bool           available      = true;
    std::vector<int> campaignIds;
    float          totalRevenue   = 0.f;
};

enum class StaffRole {
    SocialMediaManager = 0, SEOSpecialist, ContentCreator,
    PRManager, DataAnalyst, AccountManager
};

static const char* RoleNames[] = {
    "Social Media Mgr", "SEO Specialist", "Content Creator",
    "PR Manager", "Data Analyst", "Account Manager"
};

static const float RoleSalaries[] = { 3500.f, 4500.f, 3000.f, 5000.f, 6000.f, 4000.f };
static const float RoleBonus[]    = { 0.15f, 0.20f, 0.10f, 0.25f, 0.30f, 0.15f };

struct StaffMember {
    int         id          = 0;
    std::string name;
    StaffRole   role        = StaffRole::SocialMediaManager;
    float       salary      = 0.f;
    float       skill       = 0.5f;
    int         monthsHired = 0;
};

struct AIAgency {
    std::string name;
    float       marketShare    = 10.f;
    float       budget         = 30000.f;
    float       reputation     = 60.f;
    float       aggressiveness = 0.5f;
    int         clientCount    = 2;
    std::string strategy       = "volume";
};

struct NewsEvent {
    std::string title;
    std::string description;
    std::string impact;
    float socialMod     = 1.f;
    float seoMod        = 1.f;
    float emailMod      = 1.f;
    float influencerMod = 1.f;
    float prMod         = 1.f;
    float paidMod       = 1.f;
    float budgetImpact  = 0.f;
    int   durationMonths = 1;
    bool  active        = false;
    int   monthsLeft    = 0;
};

struct AgencyStats {
    float totalRevenue       = 0.f;
    float totalSpent         = 0.f;
    int   campaignsCompleted = 0;
    int   clientsAcquired    = 0;
    int   clientsLost        = 0;
    int   monthsPlayed       = 0;
    float bestMonthRevenue   = 0.f;
    float reputation         = 20.f;
};

struct GameState {
    std::string agencyName      = "My Agency";
    float       budget          = 10000.f;
    float       monthlyRevenue  = 0.f;
    float       monthlyExpenses = 0.f;
    int         month           = 1;
    int         year            = 2024;

    std::vector<Client>      clients;
    std::vector<Campaign>    campaigns;
    std::vector<StaffMember> staff;
    std::vector<AIAgency>    competitors;
    std::vector<NewsEvent>   activeEvents;

    std::unordered_map<ChannelType, float> channelModifiers;

    AgencyStats stats;

    bool showDashboard = true;
    bool showCampaigns = false;
    bool showClients   = false;
    bool showStaff     = false;
    bool showMarketMap = false;
    bool showNewsfeed  = false;
    bool showSettings  = false;
    bool gameOver      = false;
    bool victory       = false;

    float playerMarketShare = 2.f;
    int   nextClientId      = 1;
    int   nextCampaignId    = 1;
    int   nextStaffId       = 1;
};

// std::hash specialization so ChannelType works in unordered_map
namespace std {
    template<> struct hash<ChannelType> {
        size_t operator()(ChannelType c) const noexcept {
            return hash<int>{}(static_cast<int>(c));
        }
    };
}
