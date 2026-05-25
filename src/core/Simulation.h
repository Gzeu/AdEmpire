#pragma once
#include "GameState.h"

class Simulation {
public:
    static void  AdvanceMonth(GameState& gs);
    static void  ProcessCampaign(Campaign& c, GameState& gs);
    static float CalcReach(const Campaign& c, const GameState& gs);
    static float CalcCTR(const Campaign& c, const GameState& gs);
    static float CalcConversionRate(const Campaign& c, const GameState& gs);
    static float CalcRevenue(const Campaign& c, const GameState& gs);
    static void  UpdateClientSatisfaction(Client& cl, const GameState& gs);
    static void  ProcessAICompetitors(GameState& gs);
    static float GetChannelMod(ChannelType ch, const GameState& gs);
};
