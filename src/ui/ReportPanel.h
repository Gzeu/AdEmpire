#pragma once
#include "../core/GameState.h"

class ReportPanel {
public:
    static void Render(GameState& gs);
    static void GenerateMonthlyReport(const GameState& gs);

private:
    struct ReportData {
        int   month = 0, year = 0;
        float revenue = 0.f, expenses = 0.f, profit = 0.f;
        float marketShare = 0.f;
        int   activeClients = 0, activeCampaigns = 0;
        float bestCampaignFee = 0.f;
        std::string bestCampaignName;
        int   eventsCount = 0;
    };
    static ReportData s_last;
    static bool       s_hasReport;
};
