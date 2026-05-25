#include "CampaignTemplates.h"

static const std::vector<CampaignTemplate> ALL_TEMPLATES = {
    // Social
    {"Viral Reel Blitz",      ChannelType::Social,     1500, 2, 1.2f, "Short-form video push, max reach.",                 ClientIndustry::Fashion},
    {"Community Builder",    ChannelType::Social,     2000, 3, 0.8f, "Engagement focus, grows following.",               ClientIndustry::Food},
    {"Meme Campaign",        ChannelType::Social,      800, 1, 1.5f, "Low cost, very high viral potential.",              ClientIndustry::Gaming},
    // SEO
    {"Blog Authority Push",  ChannelType::SEO,        2500, 6, 1.0f, "Long-form content, sustained organic traffic.",    ClientIndustry::Tech},
    {"Local SEO Blitz",      ChannelType::SEO,        1200, 3, 0.9f, "Google My Business + local backlinks.",            ClientIndustry::Food},
    {"Technical Audit Sprint",ChannelType::SEO,       3000, 2, 1.3f, "Site speed + Core Web Vitals fix.",               ClientIndustry::Finance},
    // Email
    {"Welcome Series",       ChannelType::Email,       600, 1, 1.4f, "5-email onboarding, high conversion rate.",        ClientIndustry::Education},
    {"Re-engagement Blast",  ChannelType::Email,       900, 2, 1.1f, "Win back cold subscribers.",                      ClientIndustry::Retail},
    {"Newsletter Growth",    ChannelType::Email,      1500, 4, 0.9f, "Content-driven, grows subscriber list.",           ClientIndustry::Finance},
    // Influencer
    {"Micro-Influencer Pack",ChannelType::Influencer, 3000, 2, 1.3f, "10 micro-influencers, niche trust.",              ClientIndustry::Fashion},
    {"Macro Brand Deal",     ChannelType::Influencer, 8000, 1, 1.6f, "1 macro influencer, massive reach.",              ClientIndustry::Gaming},
    // PR
    {"Press Release Wave",   ChannelType::PR,         1000, 1, 1.0f, "10 outlets, product launch announcement.",        ClientIndustry::Tech},
    {"Thought Leadership",   ChannelType::PR,         2000, 3, 1.2f, "CEO quoted in 5 industry publications.",          ClientIndustry::Finance},
    // Paid Search
    {"Brand Keyword Blitz",  ChannelType::PaidSearch, 2000, 2, 1.1f, "Defend brand terms, capture intent.",            ClientIndustry::Retail},
    {"Competitor Conquest",  ChannelType::PaidSearch, 3500, 1, 1.4f, "Bid on competitor keywords.",                    ClientIndustry::Tech},
};

const std::vector<CampaignTemplate>& CampaignTemplates::GetAll() {
    return ALL_TEMPLATES;
}

std::vector<CampaignTemplate> CampaignTemplates::ForIndustry(ClientIndustry ind) {
    std::vector<CampaignTemplate> result;
    for (auto& t : ALL_TEMPLATES)
        if (t.bestFor == ind) result.push_back(t);
    if (result.empty()) result = ALL_TEMPLATES; // fallback: show all
    return result;
}
