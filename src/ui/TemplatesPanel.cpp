#include "TemplatesPanel.h"
#include "../systems/CampaignTemplates.h"
#include "../systems/CampaignEngine.h"
#include "imgui.h"
#include <cstdio>

static int s_templateClientId = -1;

void TemplatesPanel::Render(GameState& gs) {
    if (!gs.showTemplates) return;
    ImGui::SetNextWindowPos(ImVec2(325, 40), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(700, 560), ImGuiCond_Always);
    ImGui::Begin("Campaign Templates", &gs.showTemplates,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    ImGui::TextColored(ImVec4(0.5f,0.9f,1.f,1.f),
        "Quick-launch proven campaign formats.");
    ImGui::Spacing();

    // Client selector
    ImGui::Text("Apply to client:");
    for (auto& cl : gs.clients) {
        if (!cl.active) continue;
        bool sel = (s_templateClientId == cl.id);
        if (ImGui::RadioButton(cl.name.c_str(), sel))
            s_templateClientId = cl.id;
        ImGui::SameLine();
    }
    ImGui::NewLine();
    ImGui::Separator();

    auto& all = CampaignTemplates::GetAll();
    if (ImGui::BeginTable("##tpl", 5,
        ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
        ImGui::TableSetupColumn("Template", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Channel");
        ImGui::TableSetupColumn("Budget");
        ImGui::TableSetupColumn("Months");
        ImGui::TableSetupColumn("Action");
        ImGui::TableHeadersRow();

        for (auto& t : all) {
            ImGui::TableNextRow();
            ImGui::PushID(t.name.c_str());

            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%s", t.name.c_str());
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("%s\nQuality bonus: +%.1f", t.description.c_str(), t.qualityBonus);

            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%s", ChannelNames[(int)t.channel]);

            ImGui::TableSetColumnIndex(2);
            ImGui::Text("$%.0f", t.budgetSuggested);

            ImGui::TableSetColumnIndex(3);
            ImGui::Text("%dmo", t.durationMonths);

            ImGui::TableSetColumnIndex(4);
            bool canLaunch = (s_templateClientId > 0 && gs.budget >= t.budgetSuggested);
            if (!canLaunch) ImGui::BeginDisabled();
            if (ImGui::SmallButton("Launch")) {
                char cname[64];
                snprintf(cname, 64, "%s", t.name.c_str());
                Campaign c = CampaignEngine::CreateCampaign(
                    cname, s_templateClientId,
                    t.channel, t.budgetSuggested,
                    t.durationMonths, gs);
                c.qualityScore = std::min(c.qualityScore + t.qualityBonus, 10.f);
                gs.budget -= t.budgetSuggested;
                gs.campaigns.push_back(c);
                for (auto& cl : gs.clients)
                    if (cl.id == s_templateClientId)
                        cl.campaignIds.push_back(c.id);
            }
            if (!canLaunch) ImGui::EndDisabled();
            ImGui::PopID();
        }
        ImGui::EndTable();
    }
    ImGui::End();
}
