#pragma once
#include "../core/GameState.h"

// ============================================================
//  StatsTracker v1.1
//  Extension of AgencyStats with market + monthly helpers.
//  AgencyStats IS the stats struct — this header adds:
//    - Extended fields via macro patch (injected into AgencyStats)
//    - Free functions: ResetMonth(), WinRate()
//    - Alias: StatsTracker = AgencyStats (backward compat)
//
//  NOTE: The fields below are declared directly in AgencyStats
//  (GameState.h was updated in v1.1 push).
// ============================================================

// Backward-compat alias so any code using StatsTracker compiles
using StatsTracker = AgencyStats;

namespace StatsTrackerFn {

// Call at the end of each month (after revenue is totalled)
inline void ResetMonth(AgencyStats& s) {
    // bestMonthRevenue is kept (it's a running max, not reset)
    // Only transient per-month counters reset here if needed in future
    (void)s; // currently all fields are cumulative; placeholder for future
}

// Negotiation win rate 0-100%, returns -1 if no negotiations played
inline float WinRate(const AgencyStats& s) {
    int total = s.negotiationsWon + s.negotiationsLost;
    if (total == 0) return -1.f;
    return 100.f * s.negotiationsWon / (float)total;
}

// Track a market bonus tick (bull market month)
inline void RecordBullTick(AgencyStats& s)  { s.marketBonusTicks++;   }
inline void RecordBearTick(AgencyStats& s)  { s.marketPenaltyTicks++; }

} // namespace StatsTrackerFn
