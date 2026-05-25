#pragma once
#include "../core/GameState.h"
#include <array>
#include <string>

// Tracks per-channel, per-industry and per-month stats for advanced charts
struct ChannelStats {
    float totalRevenue  = 0.f;
    float totalReach    = 0.f;
    float totalSpent    = 0.f;
    int   campaigns     = 0;
    int   completed     = 0;
};

struct IndustryStats {
    int   pitches       = 0;
    int   wins          = 0;
    int   losses        = 0;
    float totalRevenue  = 0.f;
    float avgSatisfaction = 0.f;
    int   satisfactionSamples = 0;
};

struct MonthSnapshot {
    int   month;
    int   year;
    float revenue;
    float expenses;
    float budget;
    float marketShare;
    int   activeClients;
    int   activeCampaigns;
};

class StatsTracker {
public:
    static StatsTracker& Get();

    void Reset();
    void RecordCampaignRevenue(ChannelType ch, float revenue, float spent, float reach);
    void RecordCampaignCompleted(ChannelType ch);
    void RecordPitch(ClientIndustry ind, bool won);
    void RecordClientSatisfaction(ClientIndustry ind, float sat);
    void TakeMonthSnapshot(const GameState& gs);

    // Accessors for UI
    const ChannelStats&  GetChannel(ChannelType ch) const  { return m_channels[(int)ch]; }
    const IndustryStats& GetIndustry(ClientIndustry ind) const { return m_industries[(int)ind]; }
    const std::vector<MonthSnapshot>& GetHistory() const   { return m_history; }
    float GetChannelRevenueShare(ChannelType ch) const;
    float GetPitchWinRate(ClientIndustry ind) const;
    float GetROI(ChannelType ch) const;

private:
    StatsTracker() { Reset(); }
    std::array<ChannelStats,  6> m_channels;
    std::array<IndustryStats, 8> m_industries;
    std::vector<MonthSnapshot>   m_history;  // up to 36 months
};
