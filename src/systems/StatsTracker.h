#pragma once
#include <cstdint>

// ─── AgencyStats ─────────────────────────────────────────────────────────────
// Central stats struct embedded in GameState.
// Updated by: CampaignEngine (revenue), ContractSystem (contracts),
//             AchievementSystem (achievements), EventPopup (market ticks)
// Read by:    EndGameSummary, StatsPanel, LeaderboardPersistence
// ─────────────────────────────────────────────────────────────────────────────

struct AgencyStats {
    // Revenue tracking
    float    totalRevenue         = 0.0f;
    float    monthlyRevenue       = 0.0f;  // resets each month
    float    bestMonthRevenue     = 0.0f;

    // Campaigns
    int      campaignsCompleted   = 0;
    int      campaignsFailed      = 0;
    int      campaignsStarted     = 0;

    // Contracts
    int      contractsFulfilled   = 0;
    int      contractsBroken      = 0;

    // Market
    float    peakMarketShare      = 0.0f;
    int      marketBonusTicks     = 0;  // frames where revenueMultiplier > 1.2
    int      marketPenaltyTicks   = 0;  // frames where revenueMultiplier < 0.7

    // Staff & Achievements
    int      maxStaffLevel        = 1;
    int      achievementsUnlocked = 0;
    int      negotiationsWon      = 0;
    int      negotiationsLost     = 0;

    // Helpers
    void UpdateBestMonth() {
        if (monthlyRevenue > bestMonthRevenue)
            bestMonthRevenue = monthlyRevenue;
    }
    void ResetMonth() {
        UpdateBestMonth();
        monthlyRevenue = 0.0f;
    }
    float WinRate() const {
        int total = negotiationsWon + negotiationsLost;
        return total > 0 ? (float)negotiationsWon / total : 0.0f;
    }
};

// Alias for backward compatibility (old code using StatsTracker)
using StatsTracker = AgencyStats;
