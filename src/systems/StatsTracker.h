#pragma once
#include "../core/GameState.h"
#include <array>
#include <string>

// Per-channel stats accumulated each month
struct ChannelStats {
    float totalRevenue  = 0.f;
    float totalSpent    = 0.f;
    int   campaignCount = 0;
    int   wins          = 0; // campaigns that generated > 0 revenue
};

// Per-industry stats
struct IndustryStats {
    int   pitched  = 0;
    int   won      = 0;
    int   lost     = 0;
    float revenue  = 0.f;
};

// Monthly snapshot for history charts
struct MonthSnapshot {
    int   month, year;
    float revenue;
    float expenses;
    float marketShare;
    int   activeClients;
};

class StatsTracker {
public:
    static StatsTracker& Get();

    void RecordMonth(const GameState& gs);
    void RecordCampaignRevenue(ChannelType ch, float revenue, float spent);
    void RecordPitch(ClientIndustry ind, bool won);

    // Accessors for UI
    const ChannelStats&              GetChannelStats(ChannelType ch) const;
    const IndustryStats&             GetIndustryStats(ClientIndustry ind) const;
    const std::vector<MonthSnapshot>& GetHistory() const { return history; }

    float GetWinRateForIndustry(ClientIndustry ind) const;
    float GetROIForChannel(ChannelType ch) const;

    // Chart data helpers (fills float arrays for ImGui::PlotLines)
    void FillRevenueHistory(float* out, int maxCount) const;
    void FillChannelRevenue(float out[6]) const;
    void FillIndustryWinRate(float out[8]) const;

private:
    StatsTracker() = default;
    std::array<ChannelStats,  6> channelStats  = {};
    std::array<IndustryStats, 8> industryStats = {};
    std::vector<MonthSnapshot>   history;
};
