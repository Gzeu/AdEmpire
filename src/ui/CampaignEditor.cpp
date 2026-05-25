#include "CampaignEditor.h"
#include "../systems/CampaignEngine.h"
#include "../core/Simulation.h"
#include "imgui.h"
#include <cstdio>

static char  s_campName[64] = "Campaign";
static int   s_selectedClient = -1;
static int   s_selectedChannel = 0;
static float s_budget = 1000.f;
static int   s_duration = 3;

void CampaignEditor::Render(GameState& gs) {
    if (!gs.showCampaigns) return;
    ImGui::SetNextWindowPos(ImVec2(325, 40), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(700, 560), ImGuiCond_Always);
    ImGui::Begin("Campaigns", &gs.showCampaigns,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    if (ImGui::BeginTabBar("##camptabs")) {
        // ── New Campaign tab
        if (ImGui::BeginTabItem("New Campaign")) {
            ImGui::InputText("Name", s_campName, 64);

            // Client selector
            ImGui::Text("Client:");
            for (int i = 0; i < (int)gs.clients.size(); i++) {
                auto& cl = gs.clients[i];
                if (!cl.active) continue;
                bool sel = (s_selectedClient == cl.id);
                if (ImGui::RadioButton(cl.name.c_str(), sel))
                    s_selectedClient = cl.id;
            }

            // Channel
            ImGui::Text("Channel:");
            for (int i = 0; i < 6; i++) {
                if (i > 0) ImGui::SameLine();
                bool sel = (s_selectedChannel == i);
                if (sel) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f,0.6f,1.f,1.f));
                if (ImGui::Button(ChannelNames[i]))
                    s_selectedChannel = i;
                if (sel) ImGui::PopStyleColor();
            }

            ImGui::SliderFloat("Budget ($)", &s_budget, 500.f, 20000.f, "$%.0f");
            ImGui::SliderInt("Duration (months)", &s_duration, 1, 12);

            // Preview ROI
            if (s_selectedClient > 0) {
                Campaign preview = CampaignEngine::CreateCampaign(
                    s_campName, s_selectedClient,
                    (ChannelType)s_selectedChannel, s_budget, s_duration, gs);
                float roi = CampaignEngine::EstimateROI(preview, gs);
                float reach = Simulation::CalcReach(preview, gs);
                float ctr   = Simulation::CalcCTR(preview, gs) * 100.f;

                ImGui::Separator();
                ImGui::TextColored(ImVec4(0.5f,1.f,0.5f,1.f), "Estimated Results (per month):");
                ImGui::Text("  Reach: %.0f people", reach);
                ImGui::Text("  CTR: %.2f%%", ctr);
                ImGui::Text("  Agency Fee: $%.0f", preview.budget * 0.005f * ctr);
                ImGui::TextColored(ImVec4(0.3f,1.f,0.5f,1.f),
                    "  Estimated ROI: %.1f%%", roi);
            }

            ImGui::Spacing();
            bool canLaunch = (s_selectedClient > 0 && s_budget <= gs.budget);
            if (!canLaunch) ImGui::BeginDisabled();
            if (ImGui::Button("Launch Campaign", ImVec2(200, 36))) {
                Campaign c = CampaignEngine::CreateCampaign(
                    s_campName, s_selectedClient,
                    (ChannelType)s_selectedChannel, s_budget, s_duration, gs);
                gs.budget -= s_budget;
                gs.campaigns.push_back(c);
                for (auto& cl : gs.clients)
                    if (cl.id == s_selectedClient) cl.campaignIds.push_back(c.id);
            }
            if (!canLaunch) ImGui::EndDisabled();
            if (!canLaunch && s_budget > gs.budget)
                ImGui::TextColored(ImVec4(1,0.3f,0.3f,1), "Insufficient budget!");

            ImGui::EndTabItem();
        }

        // ── Active Campaigns tab
        if (ImGui::BeginTabItem("Active Campaigns")) {
            if (ImGui::BeginTable("##active", 6,
                ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
                ImGui::TableSetupColumn("Name");
                ImGui::TableSetupColumn("Channel");
                ImGui::TableSetupColumn("Budget");
                ImGui::TableSetupColumn("Reach");
                ImGui::TableSetupColumn("CTR");
                ImGui::TableSetupColumn("Fee/mo");
                ImGui::TableHeadersRow();
                for (auto& c : gs.campaigns) {
                    if (!c.active) continue;
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("%s", c.name.c_str());
                    ImGui::TableSetColumnIndex(1); ImGui::Text("%s", ChannelNames[(int)c.channel]);
                    ImGui::TableSetColumnIndex(2); ImGui::Text("$%.0f", c.budget);
                    ImGui::TableSetColumnIndex(3); ImGui::Text("%.0f", c.reach);
                    ImGui::TableSetColumnIndex(4); ImGui::Text("%.2f%%", c.ctr * 100.f);
                    ImGui::TableSetColumnIndex(5); ImGui::Text("$%.0f", c.agencyFee);
                }
                ImGui::EndTable();
            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::End();
}
