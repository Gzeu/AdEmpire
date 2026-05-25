#include "NegotiationSystem.h"
#include "FitScoreSystem.h"
#include <algorithm>
#include <cstdlib>
#include <cstdio>

std::string NegotiationSystem::GetStageTitle(NegotiationStage stage) {
    switch (stage) {
        case NegotiationStage::Intro:             return "Introduction";
        case NegotiationStage::BudgetDiscussion:  return "Budget Discussion";
        case NegotiationStage::ChannelSelection:  return "Channel Strategy";
        case NegotiationStage::ContractTerms:     return "Contract Terms";
        case NegotiationStage::FinalOffer:        return "Final Offer";
        case NegotiationStage::Closed:            return "Closed";
    }
    return "";
}

std::string NegotiationSystem::GetClientResponse(const GameState& gs) {
    auto& n = gs.negotiation;
    if (!n.active) return "";
    Client* cl = nullptr;
    for (auto& c : const_cast<GameState&>(gs).clients)
        if (c.id == n.clientId) { cl = &c; break; }
    if (!cl) return "Client not found.";

    float mood = n.clientMood;
    switch (n.stage) {
        case NegotiationStage::Intro:
            if (mood > 0.7f) return "We've heard good things about your agency. Tell us more.";
            if (mood > 0.4f) return "We're exploring options. What can you offer us?";
            return "We're talking to several agencies. Make it count.";
        case NegotiationStage::BudgetDiscussion:
            if (n.offeredBudget >= cl->budget)
                return "That budget works for us. What channels do you recommend?";
            return "We were expecting a higher investment from your side.";
        case NegotiationStage::ChannelSelection: {
            ChannelType best = IndustryBestChannel[(int)cl->industry];
            if (n.offeredChannel == best)
                return "That's exactly the channel we had in mind. Great alignment!";
            return "Interesting choice. Can you justify that channel for our industry?";
        }
        case NegotiationStage::ContractTerms:
            if (n.offeredContract == ContractType::Annual)
                return "Annual commitment is bold. We'd need strong guarantees.";
            if (n.offeredContract == ContractType::Monthly)
                return "Monthly is flexible, but we prefer more stability.";
            return "Quarterly sounds reasonable. Let's talk numbers.";
        case NegotiationStage::FinalOffer:
            if (mood > 0.65f) return "We're ready to sign. Send over the contract.";
            if (mood > 0.40f) return "We need to think about this. Follow up tomorrow.";
            return "We've decided to go with another agency. Thank you.";
        default: return "";
    }
}

void NegotiationSystem::Start(int clientId, GameState& gs) {
    auto& n = gs.negotiation;
    n = NegotiationState(); // reset
    n.active    = true;
    n.clientId  = clientId;
    n.stage     = NegotiationStage::Intro;
    n.clientMood = 0.55f + (float)(rand() % 30) / 100.f; // 0.55 - 0.85

    for (auto& cl : gs.clients)
        if (cl.id == clientId) {
            n.offeredBudget   = cl.budget;
            n.fitScore        = FitScoreSystem::Calculate(cl, gs);
            cl.inNegotiation  = true;
            break;
        }
    gs.showNegotiation = true;
    n.lastMessage = GetClientResponse(gs);
}

void NegotiationSystem::IncreaseOffer(GameState& gs) {
    gs.negotiation.offeredBudget *= 1.10f;
    gs.negotiation.clientMood = std::min(gs.negotiation.clientMood + 0.05f, 1.f);
    gs.negotiation.lastMessage = GetClientResponse(gs);
}

void NegotiationSystem::DecreaseOffer(GameState& gs) {
    gs.negotiation.offeredBudget *= 0.90f;
    gs.negotiation.clientMood = std::max(gs.negotiation.clientMood - 0.08f, 0.f);
    gs.negotiation.lastMessage = GetClientResponse(gs);
}

void NegotiationSystem::PressForShortContract(GameState& gs) {
    gs.negotiation.offeredContract = ContractType::Monthly;
    gs.negotiation.clientMood = std::max(gs.negotiation.clientMood - 0.05f, 0.f);
    gs.negotiation.lastMessage = GetClientResponse(gs);
}

void NegotiationSystem::OfferLongContract(GameState& gs) {
    gs.negotiation.offeredContract = ContractType::Annual;
    gs.negotiation.clientMood = std::min(gs.negotiation.clientMood + 0.03f, 1.f);
    gs.negotiation.lastMessage = GetClientResponse(gs);
}

void NegotiationSystem::PressForDeal(GameState& gs) {
    gs.negotiation.playerPressure++;
    gs.negotiation.clientMood -= 0.10f;
    if (gs.negotiation.clientMood < 0) gs.negotiation.clientMood = 0.f;
    gs.negotiation.lastMessage = GetClientResponse(gs);
}

void NegotiationSystem::BuildRapport(GameState& gs) {
    gs.negotiation.clientMood = std::min(gs.negotiation.clientMood + 0.07f, 1.f);
    gs.negotiation.lastMessage = "Thanks for taking the time to understand our needs.";
}

void NegotiationSystem::Advance(GameState& gs) {
    auto& n = gs.negotiation;
    int s = (int)n.stage;
    if (s < (int)NegotiationStage::FinalOffer)
        n.stage = (NegotiationStage)(s + 1);
    n.lastMessage = GetClientResponse(gs);

    // If pressure too high, client walks
    if (n.playerPressure >= 3) {
        n.lostDeal = true;
        n.lastMessage = "You've been too aggressive. We're walking away.";
        Resolve(gs);
    }
}

void NegotiationSystem::Resolve(GameState& gs) {
    auto& n = gs.negotiation;
    n.stage = NegotiationStage::Closed;

    for (auto& cl : gs.clients)
        if (cl.id == n.clientId) cl.inNegotiation = false;

    float winChance = FitScoreSystem::WinChance(n.fitScore, gs.stats.reputation);
    float moodMod   = (n.clientMood - 0.5f) * 0.4f;
    bool  won       = !n.lostDeal && ((float)(rand() % 100) / 100.f < winChance + moodMod);

    if (won) {
        n.wonDeal = true;
        for (auto& cl : gs.clients)
            if (cl.id == n.clientId) {
                cl.active         = true;
                cl.available      = false;
                cl.budget         = n.offeredBudget;
                cl.contractType   = n.offeredContract;
                cl.contractMonths = ContractDurations[(int)n.offeredContract];
                cl.satisfaction   = 70.f;
                break;
            }
        gs.stats.clientsAcquired++;
        gs.stats.negotiationsWon++;
        gs.playerMarketShare += 1.5f;
        n.lastMessage = "Deal closed! Welcome aboard.";
    } else {
        n.lostDeal = true;
        gs.stats.negotiationsLost++;
        if (!n.lastMessage.size())
            n.lastMessage = "We've decided to go with another agency.";
    }
    n.active = false;
}

void NegotiationSystem::Cancel(GameState& gs) {
    for (auto& cl : gs.clients)
        if (cl.id == gs.negotiation.clientId) cl.inNegotiation = false;
    gs.negotiation = NegotiationState();
    gs.showNegotiation = false;
}
