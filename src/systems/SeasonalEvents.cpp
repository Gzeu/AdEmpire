#include "SeasonalEvents.h"

// month -> {social, seo, email, influencer, pr, paidSearch, label}
static const SeasonalModifiers SEASONS[13] = {
    {1,1,1,1,1,1,"Neutral"},             // 0 unused
    {0.85f,1.0f,0.9f,0.8f,1.0f,0.9f,  "January Slowdown"},
    {0.9f, 1.0f,1.0f,0.9f,1.0f,0.95f, "February Steady"},
    {1.0f, 1.1f,1.0f,1.0f,1.1f,1.0f,  "March Spring Push"},
    {1.05f,1.1f,1.1f,1.05f,1.0f,1.0f, "April Growth"},
    {1.0f, 1.0f,1.0f,1.0f,1.0f,1.0f,  "May Neutral"},
    {0.95f,0.9f,0.9f,0.9f,0.9f,0.9f,  "June Summer Dip"},
    {0.80f,0.85f,0.85f,0.85f,0.85f,0.80f, "July Summer Slump −20%"},
    {0.9f, 1.0f,1.0f,0.9f,0.9f,0.9f,  "August Recovery"},
    {1.05f,1.1f,1.05f,1.0f,1.0f,1.1f, "September Back-to-Work"},
    {1.1f, 1.1f,1.1f,1.1f,1.1f,1.1f,  "October Q4 Warmup"},
    {1.60f,1.3f,1.5f,1.6f,1.3f,1.7f,  "November Black Friday +60%"},
    {1.80f,1.2f,1.6f,1.8f,1.4f,1.9f,  "December Christmas +80%"},
};

SeasonalModifiers SeasonalEvents::GetForMonth(int month) {
    if (month < 1 || month > 12) return SEASONS[0];
    return SEASONS[month];
}

const char* SeasonalEvents::GetSeasonLabel(int month) {
    return GetForMonth(month).description;
}

void SeasonalEvents::Apply(GameState& gs) {
    auto s = GetForMonth(gs.month);
    gs.channelModifiers[ChannelType::Social]     *= s.social;
    gs.channelModifiers[ChannelType::SEO]        *= s.seo;
    gs.channelModifiers[ChannelType::Email]      *= s.email;
    gs.channelModifiers[ChannelType::Influencer] *= s.influencer;
    gs.channelModifiers[ChannelType::PR]         *= s.pr;
    gs.channelModifiers[ChannelType::PaidSearch] *= s.paidSearch;
}
