#pragma once
#include "../core/GameState.h"
#include <vector>
#include <string>

struct CampaignTemplate {
    std::string  name;
    ChannelType  channel;
    float        budgetSuggested;
    int          durationMonths;
    float        qualityBonus;      // added on top of base quality score
    std::string  description;
    ClientIndustry bestFor;         // industry that benefits most
};

class CampaignTemplates {
public:
    static const std::vector<CampaignTemplate>& GetAll();
    static std::vector<CampaignTemplate> ForIndustry(ClientIndustry ind);
};
