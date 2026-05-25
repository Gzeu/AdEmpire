#include "TemplatesPanel.h"
#include "../systems/CampaignEngine.h"
#include "imgui.h"
#include <cstdio>

// 15 built-in templates
static const CampaignTemplate TEMPLATES[] = {
    // Social
    {"Viral Reel Blitz",     ChannelType::Social,     2000.f, 1.2f, ClientIndustry::Fashion,   "Short-form video burst, ideal for Fashion"},
    {"Community Engagement", ChannelType::Social,     1500.f, 1.0f, ClientIndustry::Food,       "Build brand community over 3 months"},
    {"Social Brand Lift",    ChannelType::Social,     3000.f, 1.1f, ClientIndustry::Retail,     "Broad awareness push across platforms"},
    // SEO
    {"SEO Foundation",       ChannelType::SEO,        1800.f, 0.8f, ClientIndustry::Tech,       "Technical SEO + on-page basics"},
    {"Content Authority",    ChannelType::SEO,        2500.f, 1.2f, ClientIndustry::Finance,    "Long-form content for Finance niche"},
    {"Local SEO Sprint",     ChannelType::SEO,        1200.f, 1.0f, ClientIndustry::Health,     "Google Business + local citations"},
    // Email
    {"Welcome Drip",         ChannelType::Email,      800.f,  1.0f, ClientIndustry::Education,  "5-email onboarding sequence"},
    {"Re-engagement Blast",  ChannelType::Email,      1000.f, 1.1f, ClientIndustry::Retail,     "Win-back cold subscribers"},
    // Influencer
    {"Micro-Influencer Pack",ChannelType::Influencer, 4000.f, 1.4f, ClientIndustry::Fashion,    "10 micro-influencers, high trust"},
    {"Gaming Collab",        ChannelType::Influencer, 5000.f, 1.6f, ClientIndustry::Gaming,     "Streamer + YouTuber cross-promo"},
    // PR
    {"Press Release Burst",  ChannelType::PR,         1500.f, 1.0f, ClientIndustry::Finance,    "5 press releases across trade media"},
    {"Product Launch PR",    ChannelType::PR,         3500.f, 1.3f, ClientIndustry::Tech,       "Launch coverage + analyst briefings"},
    // Paid Search
    {"Google Ads Starter",   ChannelType::PaidSearch, 2000.f, 1.0f, ClientIndustry::Retail,     "Search + Shopping campaign setup"},
    {"Performance Max",      ChannelType::PaidSearch, 5000.f, 1.5f, ClientIndustry::Finance,    "Full PMax campaign across all Google inventory"},
    {"Retargeting Engine",   ChannelType::PaidSearch, 1500.f, 1.2f, ClientIndustry::Health,     "Remarketing to warm audiences"},
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
        ImGui::Text("Budget:   $%.0f", t.suggestedBudget);
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
        bool canLaunch = selectedClient > 0 && gs.budget >= t.suggestedBudget;
        if (!canLaunch) ImGui::BeginDisabled();
        if (ImGui::Button("Launch Template Campaign", ImVec2(-1, 36))) {
            Campaign c = CampaignEngine::CreateCampaign(
                t.name, selectedClient, t.channel,
                t.suggestedBudget, 3, gs);
            c.qualityScore = std::min(c.qualityScore + t.qualityBonus, 10.f);
            gs.budget -= t.suggestedBudget;
            gs.campaigns.push_back(c);
        }
        if (!canLaunch) ImGui::EndDisabled();
        if (gs.budget < t.suggestedBudget)
            ImGui::TextColored(ImVec4(1,0.3f,0.3f,1), "Insufficient budget!");
    } else {
        ImGui::TextDisabled("Select a template from the list.");
    }
    ImGui::EndChild();
    ImGui::End();
}
