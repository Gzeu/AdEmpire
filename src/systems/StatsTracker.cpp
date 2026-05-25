#include "StatsTracker.h"
#include <numeric>
#include <algorithm>

StatsTracker& StatsTracker::Get() {
    static StatsTracker inst;
    return inst;
}

void StatsTracker::Reset() {
    m_channels   = {};
    m_industries = {};
    m_history.clear();
}

void StatsTracker::RecordCampaignRevenue(ChannelType ch, float revenue, float spent, float reach) {
    auto& s = m_channels[(int)ch];
    s.totalRevenue += revenue;
    s.totalSpent   += spent;
    s.totalReach   += reach;
    s.campaigns++;
}

void StatsTracker::RecordCampaignCompleted(ChannelType ch) {
    m_channels[(int)ch].completed++;
}

void StatsTracker::RecordPitch(ClientIndustry ind, bool won) {
    auto& s = m_industries[(int)ind];
    s.pitches++;
    if (won) s.wins++; else s.losses++;
}

void StatsTracker::RecordClientSatisfaction(ClientIndustry ind, float sat) {
    auto& s = m_industries[(int)ind];
    s.satisfactionSamples++;
    s.avgSatisfaction += (sat - s.avgSatisfaction) / s.satisfactionSamples; // running avg
}

void StatsTracker::TakeMonthSnapshot(const GameState& gs) {
    int activeClients = 0, activeCampaigns = 0;
    for (auto& c  : gs.clients)   if (c.active)  activeClients++;
    for (auto& cp : gs.campaigns) if (cp.active) activeCampaigns++;
    m_history.push_back({
        gs.month, gs.year,
        gs.monthlyRevenue, gs.monthlyExpenses,
        gs.budget, gs.playerMarketShare,
        activeClients, activeCampaigns
    });
    if (m_history.size() > 36)
        m_history.erase(m_history.begin());
}

float StatsTracker::GetChannelRevenueShare(ChannelType ch) const {
    float total = 0.f;
    for (auto& s : m_channels) total += s.totalRevenue;
    if (total == 0) return 0.f;
    return m_channels[(int)ch].totalRevenue / total * 100.f;
}

float StatsTracker::GetPitchWinRate(ClientIndustry ind) const {
    auto& s = m_industries[(int)ind];
    if (s.pitches == 0) return 0.f;
    return (float)s.wins / s.pitches * 100.f;
}

float StatsTracker::GetROI(ChannelType ch) const {
    auto& s = m_channels[(int)ch];
    if (s.totalSpent == 0) return 0.f;
    return (s.totalRevenue / s.totalSpent - 1.f) * 100.f;
}
