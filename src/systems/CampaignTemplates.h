#pragma once
#include "../core/GameState.h"
#include <string>
#include <vector>

struct CampaignTemplate {
    std::string name;
    std::string description;
    ChannelType channel;
    float       budgetRatio;   // fraction of client's monthly budget
    int         duration;
    float       qualityBonus;  // extra quality score
};

class CampaignTemplates {
public:
    static std::vector<CampaignTemplate> GetAll();
    static std::vector<CampaignTemplate> GetForIndustry(ClientIndustry ind);
};
