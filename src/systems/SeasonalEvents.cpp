#include "SeasonalEvents.h"
#include "../ui/ToastSystem.h"

const char* SeasonalEvents::GetSeasonName(int month) {
    if (month >= 3 && month <= 5) return "Spring";
    if (month >= 6 && month <= 8) return "Summer";
    if (month >= 9 && month <= 11) return "Autumn";
    return "Winter";
}

void SeasonalEvents::Apply(GameState& gs) {
    // Reset seasonal modifiers first
    for (int i = 0; i < 6; i++)
        gs.channelModifiers[(ChannelType)i] =
            std::max(0.1f, gs.channelModifiers[(ChannelType)i]);

    float social = 1.f, seo = 1.f, email = 1.f;
    float infl = 1.f, pr = 1.f, paid = 1.f;
    const char* msg = nullptr;

    switch (gs.month) {
        // January — post-holiday slump
        case 1:
            social=0.8f; email=0.9f; paid=0.8f;
            msg = "\u2744\uFE0F January Slump: Social & Paid Search -20%";
            break;
        // February — Valentine
        case 2:
            social=1.3f; infl=1.4f; email=1.2f;
            msg = "\u2764\uFE0F Valentine's Season: Social +30%, Influencer +40%";
            break;
        // March — Q1 push
        case 3:
            seo=1.2f; paid=1.1f;
            msg = "\uD83C\uDF31 Q1 Push: SEO +20%, Paid Search +10%";
            break;
        // April — Spring momentum
        case 4:
            social=1.1f; infl=1.2f;
            break;
        // May — stable
        case 5: break;
        // June — Summer begins, engagement drops slightly
        case 6:
            social=0.9f; email=0.85f;
            msg = "\u2600\uFE0F Summer Slowdown: Social -10%, Email -15%";
            break;
        // July — deepest summer slump
        case 7:
            social=0.8f; email=0.8f; paid=0.85f; seo=1.1f;
            msg = "\uD83C\uDFD6\uFE0F Summer Slump: All channels -15% except SEO";
            break;
        // August — slight recovery
        case 8:
            social=0.9f;
            break;
        // September — Back to school, B2B surge
        case 9:
            seo=1.3f; email=1.2f; paid=1.1f;
            msg = "\uD83C\uDF92 Back-to-School: SEO +30%, Email +20%";
            break;
        // October — Q4 prep
        case 10:
            social=1.1f; paid=1.2f;
            break;
        // November — Black Friday
        case 11:
            social=1.5f; infl=1.6f; email=1.4f; paid=1.6f; pr=1.3f;
            msg = "\uD83D\uDED2 Black Friday Season: ALL channels +40-60%!";
            break;
        // December — Christmas peak
        case 12:
            social=1.8f; infl=1.7f; email=1.5f; paid=1.5f; pr=1.4f;
            msg = "\uD83C\uDF84 Christmas Peak: Social +80%, Influencer +70%!";
            break;
    }

    // Apply seasonal multipliers on top of existing modifiers
    gs.channelModifiers[ChannelType::Social]     *= social;
    gs.channelModifiers[ChannelType::SEO]        *= seo;
    gs.channelModifiers[ChannelType::Email]      *= email;
    gs.channelModifiers[ChannelType::Influencer] *= infl;
    gs.channelModifiers[ChannelType::PR]         *= pr;
    gs.channelModifiers[ChannelType::PaidSearch] *= paid;

    if (msg) ToastSystem::Get().Push(msg, ToastType::Warning, 5.f);
}
