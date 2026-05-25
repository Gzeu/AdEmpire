#pragma once
#include <string>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include "GameState.h"

// ─── Contract Offer ─────────────────────────────────────────────────────────
struct ContractOffer {
    int         id;
    int         clientId;           // -1 = any new client
    std::string clientName;
    std::string description;
    float       bonusMultiplier;    // revenue * this if completed on time (1.2–2.0)
    float       penaltyAmount;      // flat $ deducted if deadline missed
    int         deadlineDays;       // months until deadline
    int         daysLeft;           // countdown
    float       requiredRevenue;    // minimum revenue target to complete
    float       revenueEarned;      // accumulated since accepted
    bool        accepted;           // player accepted
    bool        completed;          // met requiredRevenue before deadline
    bool        failed;             // deadline passed without meeting target
    bool        seen;               // player has seen this offer in UI
};

// ─── ContractSystem ─────────────────────────────────────────────────────────
class ContractSystem {
public:
    std::vector<ContractOffer> offers;   // pending (not yet accepted)
    std::vector<ContractOffer> active;   // accepted & in progress
    std::vector<ContractOffer> history;  // completed or failed
    int nextId = 1;

    // Call once per AdvanceMonth to generate new offer if below cap
    void GenerateOffer(const GameState& gs) {
        // Max 3 pending offers at once
        if ((int)offers.size() >= 3) return;
        // Offer quality scales with market share
        float ms = gs.playerMarketShare;

        // Pick a random industry label for flavour
        static const char* industries[] = {
            "Fashion", "FinTech", "Health", "Gaming", "Retail", "SaaS", "Food"
        };
        static const char* verbs[] = {
            "launch campaign for", "grow social presence of",
            "boost brand awareness for", "drive conversions for"
        };
        std::string industry = industries[rand() % 7];
        std::string verb     = verbs[rand() % 4];

        ContractOffer o;
        o.id              = nextId++;
        o.clientId        = -1;
        o.clientName      = industry + " Client";
        o.description     = "Exclusive deal to " + verb + " a " + industry + " brand.";
        o.deadlineDays    = 3 + rand() % 4;   // 3–6 months
        o.daysLeft        = o.deadlineDays;
        // Revenue target = 20–60% of current monthly revenue (or base)
        float baseRev     = std::max(gs.monthlyRevenue, 500.f);
        o.requiredRevenue = baseRev * (0.20f + (rand() % 40) * 0.01f);
        // Bonus: 1.25 + up to 0.5 based on difficulty
        o.bonusMultiplier = 1.25f + (rand() % 50) * 0.01f;
        // Penalty: 5–15% of required revenue
        o.penaltyAmount   = o.requiredRevenue * (0.05f + (rand() % 10) * 0.01f);
        o.revenueEarned   = 0.f;
        o.accepted        = false;
        o.completed       = false;
        o.failed          = false;
        o.seen            = false;
        offers.push_back(o);
    }

    void Accept(int offerId, GameState& gs) {
        for (auto it = offers.begin(); it != offers.end(); ++it) {
            if (it->id == offerId) {
                it->accepted = true;
                active.push_back(*it);
                offers.erase(it);
                // Unlock a new client slot as reward for commitment
                gs.capacity.maxClients = std::min(gs.capacity.maxClients + 1, 10);
                return;
            }
        }
    }

    void Decline(int offerId) {
        offers.erase(
            std::remove_if(offers.begin(), offers.end(),
                [offerId](const ContractOffer& o){ return o.id == offerId; }),
            offers.end());
    }

    // Call every AdvanceMonth — pass monthly agency revenue earned this month
    // Returns toast message if bonus/penalty triggered (empty = nothing)
    std::string Tick(GameState& gs, float monthRevenue) {
        std::string toast;
        for (auto& c : active) {
            if (c.completed || c.failed) continue;
            c.revenueEarned += monthRevenue;
            c.daysLeft--;

            if (c.revenueEarned >= c.requiredRevenue) {
                c.completed = true;
                float bonus = monthRevenue * (c.bonusMultiplier - 1.0f);
                gs.budget  += bonus;
                gs.stats.totalRevenue += bonus;
                toast = "CONTRACT COMPLETE: +$" + std::to_string((int)bonus)
                      + " bonus (" + c.clientName + ")";
            } else if (c.daysLeft <= 0) {
                c.failed    = true;
                gs.budget  -= c.penaltyAmount;
                toast = "CONTRACT FAILED: -$" + std::to_string((int)c.penaltyAmount)
                      + " penalty (" + c.clientName + ")";
            }
        }
        // Move done contracts to history
        for (auto it = active.begin(); it != active.end(); ) {
            if (it->completed || it->failed) {
                history.push_back(*it);
                it = active.erase(it);
            } else { ++it; }
        }
        return toast;
    }

    int PendingCount() const {
        int n = 0;
        for (auto& o : offers) if (!o.seen) n++;
        return n;
    }
};
