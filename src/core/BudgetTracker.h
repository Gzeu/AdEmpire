#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include "GameState.h"

// ─── Monthly snapshot ────────────────────────────────────────────────────────
struct CashflowSnapshot {
    int   month;
    int   year;
    float revenue;
    float expenses;
    float cashflow;   // revenue - expenses
    float balance;    // gs.budget at end of month
};

// ─── BudgetTracker ───────────────────────────────────────────────────────────
class BudgetTracker {
public:
    static constexpr float OVERDRAFT_INTEREST  = 0.025f;  // 2.5% per month on negative balance
    static constexpr float WARN_THRESHOLD      = 2000.f;  // orange warning below this
    static constexpr float DANGER_THRESHOLD    = 0.f;     // red danger at 0 or below
    static constexpr int   HISTORY_MONTHS      = 12;      // rolling window

    std::vector<CashflowSnapshot> history;  // last 12 months
    int  overdraftMonths = 0;               // consecutive months in red
    bool inDanger        = false;
    bool inWarning       = false;

    // Call at end of AdvanceMonth, AFTER budget has been updated
    // Returns toast message string (empty = nothing to show)
    std::string Tick(GameState& gs) {
        std::string toast;

        // ── Overdraft interest
        if (gs.budget < 0.f) {
            float interest = std::abs(gs.budget) * OVERDRAFT_INTEREST;
            gs.budget -= interest;
            overdraftMonths++;
            toast = "OVERDRAFT: -$" + std::to_string((int)interest)
                  + " interest (month " + std::to_string(overdraftMonths) + " in red)";
        } else {
            overdraftMonths = 0;
        }

        // ── Record snapshot
        CashflowSnapshot snap;
        snap.month    = gs.month;
        snap.year     = gs.year;
        snap.revenue  = gs.monthlyRevenue;
        snap.expenses = gs.monthlyExpenses;
        snap.cashflow = gs.monthlyRevenue - gs.monthlyExpenses;
        snap.balance  = gs.budget;
        history.push_back(snap);
        if ((int)history.size() > HISTORY_MONTHS)
            history.erase(history.begin());

        // ── Status flags
        inDanger  = (gs.budget <= DANGER_THRESHOLD);
        inWarning = (!inDanger && gs.budget < WARN_THRESHOLD);

        return toast;
    }

    // 12-month average cashflow (positive = healthy)
    float AverageCashflow() const {
        if (history.empty()) return 0.f;
        float sum = 0.f;
        for (auto& s : history) sum += s.cashflow;
        return sum / (float)history.size();
    }

    // Runway in months at current average burn rate (if cashflow negative)
    int RunwayMonths(float currentBalance) const {
        float avg = AverageCashflow();
        if (avg >= 0.f) return 999;  // profitable
        return (int)(currentBalance / -avg);
    }

    // For sparkline chart: last N cashflow values
    std::vector<float> CashflowSeries() const {
        std::vector<float> v;
        for (auto& s : history) v.push_back(s.cashflow);
        return v;
    }
};
