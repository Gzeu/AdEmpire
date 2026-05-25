#pragma once
#include "GameState.h"

// Forward declarations for tension systems
class ContractSystem;
class CompetitorAI;
class BudgetTracker;

class Simulation {
public:
    static void  AdvanceMonth(GameState& gs);
    static float GetChannelMod(ChannelType ch, const GameState& gs);
    static float CalcReach(const Campaign& c, const GameState& gs);
    static float CalcCTR(const Campaign& c, const GameState& gs);
    static float CalcConversionRate(const Campaign& c, const GameState& gs);
    static float CalcRevenue(const Campaign& c, const GameState& gs);
    static void  ProcessCampaign(Campaign& c, GameState& gs);
    static void  UpdateClientSatisfaction(Client& cl, GameState& gs);
    static void  ProcessAICompetitors(GameState& gs);

    // v1.0: Access tension systems from UI panels
    static ContractSystem& Contracts();
    static CompetitorAI&   Competitors();
    static BudgetTracker&  Budget();
};
