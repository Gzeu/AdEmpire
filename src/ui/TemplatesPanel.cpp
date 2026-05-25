#include "TemplatesPanel.h"
#include "../systems/CampaignEngine.h"
#include "../systems/CampaignTemplates.h"
#include "imgui.h"
#include <cstdio>

// 15 built-in templates
static const CampaignTemplate TEMPLATES[] = {
    // Social
    {"Viral Reel Blitz",     ChannelType::Social,     2000.f, 3, 1.2f, "Short-form video burst, ideal for Fashion", ClientIndustry::Fashion},
    {"Community Engagement", ChannelType::Social,     1500.f, 3, 1.0f, "Build brand community over 3 months", ClientIndustry::Food},
    {"Social Brand Lift",    ChannelType::Social,     3000.f, 6, 1.1f, "Broad awareness push across platforms", ClientIndustry::Retail},
    // SEO
    {"SEO Foundation",       ChannelType::SEO,        1800.f, 6, 0.8f, "Technical SEO + on-page basics", ClientIndustry::Tech},
    {"Content Authority",    ChannelType::SEO,        2500.f, 12, 1.2f, "Long-form content for Finance niche", ClientIndustry::Finance},
    {"Local SEO Sprint",     ChannelType::SEO,        1200.f, 3, 1.0f, "Google Business + local citations", ClientIndustry::Health},
    // Email
    {"Welcome Drip",         ChannelType::Email,      800.f,  3, 1.0f, "5-email onboarding sequence", ClientIndustry::Education},
    {"Re-engagement Blast",  ChannelType::Email,      1000.f, 3, 1.1f, "Win-back cold subscribers", ClientIndustry::Retail},
    // Influencer
    {"Micro-Influencer Pack",ChannelType::Influencer, 4000.f, 6, 1.4f, "10 micro-influencers, high trust", ClientIndustry::Fashion},
    {"Gaming Collab",        ChannelType::Influencer, 5000.f, 6, 1.6f, "Streamer + YouTuber cross-promo", ClientIndustry::Gaming},
    // PR
    {"Press Release Burst",  ChannelType::PR,         1500.f, 3, 1.0f, "5 press releases across trade media", ClientIndustry::Finance},
    {"Product Launch PR",    ChannelType::PR,         3500.f, 6, 1.3f, "Launch coverage + analyst briefings", ClientIndustry::Tech},
    // Paid Search
    {"Google Ads Starter",   ChannelType::PaidSearch, 2000.f, 3, 1.0f, "Search + Shopping campaign setup", ClientIndustry::Retail},
    {"Performance Max",      ChannelType::PaidSearch, 5000.f, 12, 1.5f, "Full PMax campaign across all Google inventory", ClientIndustry::Finance},
    {"Retargeting Engine",   ChannelType::PaidSearch, 1500.f, 3, 1.2f, "Remarketing to warm audiences", ClientIndustry::Health},
};
static const int TEMPLATE_COUNT = 15;

void TemplatesPanel::Render(GameState& gs) {
    if (!gs.showTemplates) return;
    ImGui::SetNextWindowPos(ImVec2(325, 40), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(700, 560), ImGuiCond_Always);
    ImGui::Begin("Campaign Templates", &gs.showTemplates,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    ImGui::TextColored(ImVec4(0.5f,0.9f,1.f,1.f),
        "Select a template and apply to an active client.");
    ImGui::Separator();

    static int  selectedTpl    = -1;
    static int  selectedClient = -1;

    // Template list
    ImGui::BeginChild("##tpllist", ImVec2(330, -1), true);
    for (int i = 0; i < TEMPLATE_COUNT; i++) {
        auto& t = TEMPLATES[i];
        bool sel = (selectedTpl == i);
        if (sel) ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.2f,0.4f,0.8f,1.f));
        if (ImGui::Selectable(t.name.c_str(), sel))
            selectedTpl = i;
        if (sel) ImGui::PopStyleColor();
        ImGui::SameLine(200);
        ImGui::TextColored(ImVec4(0.5f,0.7f,1.f,0.8f),
            "%s", ChannelNames[(int)t.channel]);
    }
    ImGui::EndChild();
    ImGui::SameLine();

    // Template details + launch
    ImGui::BeginChild("##tpldetail", ImVec2(-1, -1), true);
    if (selectedTpl >= 0) {
        auto& t = TEMPLATES[selectedTpl];
        ImGui::TextColored(ImVec4(0.3f,0.9f,1.f,1.f), "%s", t.name.c_str());
        ImGui::Separator();
        ImGui::Text("Channel:  %s", ChannelNames[(int)t.channel]);
        ImGui::Text("Budget:   $%.0f", t.budgetSuggested);
        ImGui::Text("Quality+: %.1f", t.qualityBonus);
        ImGui::Text("Best for: %s", IndustryNames[(int)t.bestFor]);
        ImGui::TextWrapped("%s", t.description.c_str());
        ImGui::Separator();

        // Client picker
        ImGui::Text("Apply to client:");
        for (auto& cl : gs.clients) {
            if (!cl.active) continue;
            bool cs = (selectedClient == cl.id);
            if (ImGui::RadioButton(cl.name.c_str(), cs))
                selectedClient = cl.id;
        }
        ImGui::Spacing();
        bool canLaunch = selectedClient > 0 && gs.budget >= t.budgetSuggested;
        if (!canLaunch) ImGui::BeginDisabled();
        if (ImGui::Button("Launch Template Campaign", ImVec2(-1, 36))) {
            Campaign c = CampaignEngine::CreateCampaign(
                t.name, selectedClient, t.channel,
                t.budgetSuggested, 3, gs);
            c.qualityScore = std::min(c.qualityScore + t.qualityBonus, 10.f);
            gs.budget -= t.budgetSuggested;
            gs.campaigns.push_back(c);
        }
        if (!canLaunch) ImGui::EndDisabled();
        if (gs.budget < t.budgetSuggested)
            ImGui::TextColored(ImVec4(1,0.3f,0.3f,1), "Insufficient budget!");
    } else {
        ImGui::TextDisabled("Select a template from the list.");
    }
    ImGui::EndChild();
    ImGui::End();
}
