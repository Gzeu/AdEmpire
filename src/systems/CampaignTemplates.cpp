#include "CampaignTemplates.h"

std::vector<CampaignTemplate> CampaignTemplates::GetAll() {
    return {
        // Social
        {"Viral Social Blitz",    "Short intense social campaign, max reach",
            ChannelType::Social,     0.8f, 1, 1.5f},
        {"Brand Awareness Push",  "3-month steady brand presence on social",
            ChannelType::Social,     0.5f, 3, 1.0f},
        {"Social Retargeting",    "Target existing audience for conversions",
            ChannelType::Social,     0.4f, 2, 1.2f},
        // SEO
        {"SEO Foundation",        "6-month evergreen content & technical SEO",
            ChannelType::SEO,        0.6f, 6, 1.3f},
        {"Quick SEO Sprint",      "Target 5 high-value keywords fast",
            ChannelType::SEO,        0.4f, 2, 0.9f},
        // Email
        {"Welcome Series",        "5-email onboarding drip for new customers",
            ChannelType::Email,      0.3f, 1, 1.4f},
        {"Monthly Newsletter",    "Ongoing monthly email marketing",
            ChannelType::Email,      0.2f, 3, 1.0f},
        {"Promotional Blast",     "One-time promotional offer email",
            ChannelType::Email,      0.5f, 1, 1.1f},
        // Influencer
        {"Micro-Influencer Pack", "10 micro-influencers, high engagement",
            ChannelType::Influencer, 0.7f, 2, 1.6f},
        {"Macro Influencer Deal", "1 macro-influencer, massive reach",
            ChannelType::Influencer, 1.0f, 1, 1.8f},
        {"Ambassador Program",   "Long-term brand ambassador campaign",
            ChannelType::Influencer, 0.5f, 6, 1.4f},
        // PR
        {"Press Release Sprint",  "Media outreach for product launch",
            ChannelType::PR,         0.4f, 1, 1.5f},
        {"Industry Awards",       "Position client for awards & coverage",
            ChannelType::PR,         0.6f, 3, 1.3f},
        // Paid Search
        {"PPC Quick Win",         "Immediate traffic via paid search",
            ChannelType::PaidSearch, 0.8f, 1, 1.2f},
        {"Search Domination",     "Aggressive 3-month paid search takeover",
            ChannelType::PaidSearch, 1.0f, 3, 1.4f},
    };
}

std::vector<CampaignTemplate> CampaignTemplates::GetForIndustry(ClientIndustry ind) {
    auto all = GetAll();
    // Industry-specific preferred channels
    ChannelType preferred;
    switch(ind) {
        case ClientIndustry::Food:      preferred = ChannelType::Social;     break;
        case ClientIndustry::Fashion:   preferred = ChannelType::Influencer; break;
        case ClientIndustry::Tech:      preferred = ChannelType::SEO;        break;
        case ClientIndustry::Finance:   preferred = ChannelType::PaidSearch; break;
        case ClientIndustry::Health:    preferred = ChannelType::PR;         break;
        case ClientIndustry::Education: preferred = ChannelType::Email;      break;
        case ClientIndustry::Retail:    preferred = ChannelType::PaidSearch; break;
        case ClientIndustry::Gaming:    preferred = ChannelType::Social;     break;
        default:                        preferred = ChannelType::Social;     break;
    }
    std::vector<CampaignTemplate> result;
    // Preferred channel first
    for (auto& t : all) if (t.channel == preferred) result.push_back(t);
    for (auto& t : all) if (t.channel != preferred) result.push_back(t);
    return result;
}
