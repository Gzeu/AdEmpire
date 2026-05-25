#pragma once
#include "../core/GameState.h"
#include <string>

class NegotiationSystem {
public:
    // Start a negotiation with a client
    static void Start(int clientId, GameState& gs);

    // Player actions during negotiation
    static void IncreaseOffer(GameState& gs);    // raise budget offer +10%
    static void DecreaseOffer(GameState& gs);    // lower budget offer -10%
    static void PressForShortContract(GameState& gs);
    static void OfferLongContract(GameState& gs);
    static void PressForDeal(GameState& gs);     // risky, mood -0.1
    static void BuildRapport(GameState& gs);     // mood +0.05, takes 1 step

    // Advance to next stage
    static void Advance(GameState& gs);

    // Final resolution
    static void Resolve(GameState& gs);
    static void Cancel(GameState& gs);

    // Get current stage description
    static std::string GetStageTitle(NegotiationStage stage);
    static std::string GetClientResponse(const GameState& gs);
};
