#include "SeasonalEvents.h"
#include "ToastSystem.h"
#include <unordered_map>

struct SeasonMod {
    float social, seo, email, influencer, pr, paid;
    const char* note;
};

// Month 1..12 -> modifiers (multiplicative on top of event mods)
static const SeasonMod SEASON[13] = {
    {1,1,1,1,1,1,""},                              // placeholder [0]
    {1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,"January — steady market"},
    {1.05f,1.0f,1.05f,1.0f,1.0f,1.05f,"February — Valentine boost +5%"},
    {1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,"March — baseline"},
    {1.05f,1.05f,1.0f,1.0f,1.0f,1.05f,"April — Spring uptick"},
    {1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,"May — stable"},
    {1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,"June — stable"},
    {0.8f,0.9f,0.85f,0.9f,0.85f,0.8f,"July — Summer Slump \u22120%"},
    {0.9f,0.95f,0.9f,0.95f,0.9f,0.9f,"August — slow recovery"},
    {1.05f,1.05f,1.0f,1.0f,1.05f,1.05f,"September — Back to school boost"},
    {1.1f,1.05f,1.1f,1.05f,1.05f,1.1f,"October — Q4 ramp-up"},
    {1.6f,1.1f,1.3f,1.4f,1.1f,1.7f,"November — Black Friday season"},
    {1.8f,1.1f,1.6f,1.5f,1.2f,1.9f,"December — Holiday peak"},
};

void SeasonalEvents::Apply(GameState& gs) {
    if (gs.month < 1 || gs.month > 12) return;
    const SeasonMod& s = SEASON[gs.month];
    // Multiply onto existing channel modifiers (initialised each month in Simulation)
    gs.channelModifiers[ChannelType::Social]     *= s.social;
    gs.channelModifiers[ChannelType::SEO]        *= s.seo;
    gs.channelModifiers[ChannelType::Email]      *= s.email;
    gs.channelModifiers[ChannelType::Influencer] *= s.influencer;
    gs.channelModifiers[ChannelType::PR]         *= s.pr;
    gs.channelModifiers[ChannelType::PaidSearch] *= s.paid;
    // Notify player about extreme months
    if (gs.month == 7)  TOAST_WARN("Summer Slump! All channels -15 to -20%");
    if (gs.month == 11) TOAST_SUCCESS("Black Friday season! Social +60%, Paid +70%");
    if (gs.month == 12) TOAST_SUCCESS("Holiday Peak! Social +80%, Paid +90%");
}

const char* SeasonalEvents::GetNote(int month) {
    if (month < 1 || month > 12) return "";
    return SEASON[month].note;
}
