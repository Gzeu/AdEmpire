#pragma once
#include "../core/GameState.h"

struct MonthlyReport {
    int   month;
    int   year;
    float totalRevenue;
    float totalExpenses;
    float netProfit;
    float revenueByChannel[6];  // indexed by ChannelType
    int   clientsWon;
    int   clientsLost;
    float marketShareDelta;
    float marketShareEnd;
    float topCampaignFee;
    std::string topCampaignName;
    int   campaignsCompleted;
    int   activeCampaigns;
};

class ReportPanel {
public:
    static void Render(GameState& gs);
    static void GenerateReport(GameState& gs); // call at month end
    static MonthlyReport s_last;
    static bool          s_showOnMonthEnd;
};
