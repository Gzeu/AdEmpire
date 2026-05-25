#include "IndustryBonuses.h"

IndustryChannelBonus IndustryBonuses::Get(ClientIndustry ind) {
    switch (ind) {
        case ClientIndustry::Food:
            return {1.4f, 0.9f, 1.0f, 1.3f, 0.8f, 1.0f}; // social+infl best
        case ClientIndustry::Fashion:
            return {1.3f, 0.8f, 0.9f, 1.6f, 1.0f, 1.1f}; // influencer king
        case ClientIndustry::Tech:
            return {1.0f, 1.5f, 1.1f, 0.9f, 1.2f, 1.3f}; // SEO + paid search
        case ClientIndustry::Finance:
            return {0.8f, 1.3f, 1.4f, 0.7f, 1.1f, 1.5f}; // email + paid
        case ClientIndustry::Health:
            return {1.1f, 1.2f, 1.1f, 1.0f, 1.5f, 1.2f}; // PR strongest
        case ClientIndustry::Education:
            return {1.0f, 1.3f, 1.5f, 0.9f, 1.1f, 1.0f}; // email + SEO
        case ClientIndustry::Retail:
            return {1.2f, 1.0f, 1.1f, 1.1f, 0.9f, 1.4f}; // paid search best
        case ClientIndustry::Gaming:
            return {1.5f, 0.9f, 0.8f, 1.4f, 0.9f, 1.0f}; // social + influencer
        default:
            return {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
    }
}

float IndustryBonuses::GetBonus(ClientIndustry ind, ChannelType ch) {
    auto b = Get(ind);
    switch (ch) {
        case ChannelType::Social:     return b.social;
        case ChannelType::SEO:        return b.seo;
        case ChannelType::Email:      return b.email;
        case ChannelType::Influencer: return b.influencer;
        case ChannelType::PR:         return b.pr;
        case ChannelType::PaidSearch: return b.paid;
        default: return 1.0f;
    }
}
