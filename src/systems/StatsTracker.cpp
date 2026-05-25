#include "StatsTracker.h"
#include <algorithm>

StatsTracker& StatsTracker::Get() {
    static StatsTracker instance;
    return instance;
}

void StatsTracker::RecordMonth(const GameState& gs) {
    MonthSnapshot snap;
    snap.month         = gs.month;
    snap.year          = gs.year;
    snap.revenue       = gs.monthlyRevenue;
    snap.expenses      = gs.monthlyExpenses;
    snap.marketShare   = gs.playerMarketShare;
    snap.activeClients = 0;
    for (auto& c : gs.clients) if (c.active) snap.activeClients++;
    history.push_back(snap);
    if (history.size() > 48) history.erase(history.begin()); // keep 4 years max
}

void StatsTracker::RecordCampaignRevenue(ChannelType ch, float revenue, float spent) {
    auto& s = channelStats[(int)ch];
    s.totalRevenue  += revenue;
    s.totalSpent    += spent;
    s.campaignCount++;
    if (revenue > 0.f) s.wins++;
}

void StatsTracker::RecordPitch(ClientIndustry ind, bool won) {
    auto& s = industryStats[(int)ind];
    s.pitched++;
    if (won) s.won++;
    else     s.lost++;
}

const ChannelStats& StatsTracker::GetChannelStats(ChannelType ch) const {
    return channelStats[(int)ch];
}

const IndustryStats& StatsTracker::GetIndustryStats(ClientIndustry ind) const {
    return industryStats[(int)ind];
}

float StatsTracker::GetWinRateForIndustry(ClientIndustry ind) const {
    auto& s = industryStats[(int)ind];
    return s.pitched > 0 ? (float)s.won / s.pitched : 0.f;
}

float StatsTracker::GetROIForChannel(ChannelType ch) const {
    auto& s = channelStats[(int)ch];
    if (s.totalSpent <= 0.f) return 0.f;
    return (s.totalRevenue / s.totalSpent) * 100.f;
}

void StatsTracker::FillRevenueHistory(float* out, int maxCount) const {
    int n = (int)std::min((int)history.size(), maxCount);
    for (int i = 0; i < n; i++)
        out[i] = history[history.size() - n + i].revenue;
    for (int i = n; i < maxCount; i++) out[i] = 0.f;
}

void StatsTracker::FillChannelRevenue(float out[6]) const {
    for (int i = 0; i < 6; i++) out[i] = channelStats[i].totalRevenue;
}

void StatsTracker::FillIndustryWinRate(float out[8]) const {
    for (int i = 0; i < 8; i++) out[i] = GetWinRateForIndustry((ClientIndustry)i);
}
