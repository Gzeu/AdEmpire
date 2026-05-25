#include "CampaignEditor.h"
#include "Newsfeed.h"
#include "../systems/CampaignEngine.h"
#include "../core/Simulation.h"
#include "imgui.h"
#include <cstdio>

static char  s_campName[64]    = "My Campaign";
static int   s_selClient       = -1;
static int   s_selChannel      = 0;
static float s_budget          = 1000.f;
static int   s_duration        = 3;

void CampaignEditor::Render(GameState& gs) {
    if (!gs.showCampaigns) return;
    ImGui::SetNextWindowPos(ImVec2(325, 40), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(950, 640), ImGuiCond_Always);
    ImGui::Begin("Campaigns", &gs.showCampaigns,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    if (ImGui::BeginTabBar("##camptabs")) {
        // --- New Campaign
        if (ImGui::BeginTabItem("New Campaign")) {
            ImGui::SetNextItemWidth(300);
            ImGui::InputText("Campaign Name", s_campName, 64);
            ImGui::Spacing();

            // Client selector
            ImGui::Text("Select Client:");
            int col = 0;
            for (auto& cl : gs.clients) {
                if (!cl.active) continue;
                if (col > 0 && col % 3 != 0) ImGui::SameLine();
                bool sel = (s_selClient == cl.id);
                if (sel) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f,0.5f,0.2f,1.f));
                char lbl[48]; snprintf(lbl, 48, "%s\n$%.0f", cl.name.c_str(), cl.budget);
                if (ImGui::Button(lbl, ImVec2(200, 44))) s_selClient = cl.id;
                if (sel) ImGui::PopStyleColor();
                col++;
            }
            if (s_selClient < 0) ImGui::TextColored(ImVec4(1.f,0.6f,0.2f,1.f), "No client selected.");
            ImGui::Spacing();

            // Channel buttons
            ImGui::Text("Channel:");
            for (int i = 0; i < 6; i++) {
                if (i > 0) ImGui::SameLine();
                bool sel = (s_selChannel == i);
                if (sel) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f,0.4f,0.85f,1.f));
                if (ImGui::Button(ChannelNames[i])) s_selChannel = i;
                if (sel) ImGui::PopStyleColor();
            }
            ImGui::Spacing();

            ImGui::SetNextItemWidth(400);
            ImGui::SliderFloat("Budget ($)", &s_budget, 300.f, 25000.f, "$%.0f");
            ImGui::SetNextItemWidth(200);
            ImGui::SliderInt("Duration (months)", &s_duration, 1, 12);
            ImGui::Spacing();

            // Live ROI preview
            if (s_selClient > 0) {
                Campaign prev = CampaignEngine::CreateCampaign(
                    s_campName, s_selClient,
                    (ChannelType)s_selChannel, s_budget, s_duration, gs);
                float roi   = CampaignEngine::EstimateROI(prev, gs);
                float reach = Simulation::CalcReach(prev, gs);
                float ctr   = Simulation::CalcCTR(prev, gs) * 100.f;
                float fee   = Simulation::CalcRevenue(prev, gs) * 0.15f;
                ImGui::Separator();
                ImGui::TextColored(ImVec4(0.5f,1.f,0.5f,1.f), "Live Estimate (per month):");
                ImGui::Columns(2, nullptr, false);
                ImGui::Text("Reach");          ImGui::NextColumn(); ImGui::Text("%.0f people", reach); ImGui::NextColumn();
                ImGui::Text("CTR");            ImGui::NextColumn(); ImGui::Text("%.2f%%", ctr); ImGui::NextColumn();
                ImGui::Text("Agency Fee/mo");  ImGui::NextColumn();
                ImGui::TextColored(ImVec4(0.3f,1.f,0.5f,1.f), "$%.0f", fee); ImGui::NextColumn();
                ImGui::Text("Total over %dmo", s_duration); ImGui::NextColumn();
                ImGui::TextColored(ImVec4(0.3f,1.f,0.5f,1.f), "$%.0f", fee * s_duration); ImGui::NextColumn();
                ImGui::Text("Est. ROI");       ImGui::NextColumn();
                ImVec4 roiCol = roi > 100 ? ImVec4(0.2f,1.f,0.4f,1.f) : ImVec4(1.f,0.7f,0.2f,1.f);
                ImGui::TextColored(roiCol, "%.1f%%", roi); ImGui::NextColumn();
                ImGui::Columns(1);
            }
            ImGui::Spacing();

            bool canLaunch = (s_selClient > 0 && s_budget <= gs.budget);
            if (!canLaunch) ImGui::BeginDisabled();
            if (ImGui::Button("  Launch Campaign  ", ImVec2(220, 40))) {
                Campaign c = CampaignEngine::CreateCampaign(
                    s_campName, s_selClient,
                    (ChannelType)s_selChannel, s_budget, s_duration, gs);
                gs.budget -= s_budget;
                gs.campaigns.push_back(c);
                for (auto& cl : gs.clients)
                    if (cl.id == s_selClient) cl.campaignIds.push_back(c.id);
                Newsfeed::PushNews("Launched: " + std::string(s_campName)
                    + " via " + ChannelNames[s_selChannel], 0.4f, 0.8f, 1.f);
            }
            if (!canLaunch) ImGui::EndDisabled();
            if (s_selClient > 0 && s_budget > gs.budget)
                ImGui::TextColored(ImVec4(1.f,0.3f,0.3f,1.f), "Insufficient budget! Need $%.0f more.",
                    s_budget - gs.budget);
            ImGui::EndTabItem();
        }

        // --- Active Campaigns
        if (ImGui::BeginTabItem("Active")) {
            int active = 0;
            for (auto& c : gs.campaigns) if (c.active) active++;
            ImGui::Text("%d active campaign(s)", active);
            if (ImGui::BeginTable("##active", 7,
                ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingFixedFit)) {
                ImGui::TableSetupScrollFreeze(0, 1);
                ImGui::TableSetupColumn("Name",    ImGuiTableColumnFlags_WidthFixed, 160);
                ImGui::TableSetupColumn("Channel", ImGuiTableColumnFlags_WidthFixed, 110);
                ImGui::TableSetupColumn("Budget",  ImGuiTableColumnFlags_WidthFixed, 80);
                ImGui::TableSetupColumn("Reach",   ImGuiTableColumnFlags_WidthFixed, 80);
                ImGui::TableSetupColumn("CTR",     ImGuiTableColumnFlags_WidthFixed, 60);
                ImGui::TableSetupColumn("Fee/mo",  ImGuiTableColumnFlags_WidthFixed, 80);
                ImGui::TableSetupColumn("Left",    ImGuiTableColumnFlags_WidthFixed, 50);
                ImGui::TableHeadersRow();
                for (auto& c : gs.campaigns) {
                    if (!c.active) continue;
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("%s", c.name.c_str());
                    ImGui::TableSetColumnIndex(1); ImGui::Text("%s", ChannelNames[(int)c.channel]);
                    ImGui::TableSetColumnIndex(2); ImGui::Text("$%.0f", c.budget);
                    ImGui::TableSetColumnIndex(3); ImGui::Text("%.0f", c.reach);
                    ImGui::TableSetColumnIndex(4); ImGui::Text("%.2f%%", c.ctr * 100.f);
                    ImGui::TableSetColumnIndex(5);
                    ImGui::TextColored(ImVec4(0.3f,1.f,0.5f,1.f), "$%.0f", c.agencyFee);
                    ImGui::TableSetColumnIndex(6); ImGui::Text("%dmo", c.monthsLeft);
                }
                ImGui::EndTable();
            }
            ImGui::EndTabItem();
        }

        // --- Completed Campaigns
        if (ImGui::BeginTabItem("Completed")) {
            float total = 0.f;
            for (auto& c : gs.campaigns) if (c.completed) total += c.agencyFee * c.durationMonths;
            ImGui::Text("Total earned from completed campaigns: $%.0f", total);
            if (ImGui::BeginTable("##done", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                ImGui::TableSetupColumn("Name"); ImGui::TableSetupColumn("Channel");
                ImGui::TableSetupColumn("Duration"); ImGui::TableSetupColumn("Total Fee");
                ImGui::TableHeadersRow();
                for (auto& c : gs.campaigns) {
                    if (!c.completed) continue;
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("%s", c.name.c_str());
                    ImGui::TableSetColumnIndex(1); ImGui::Text("%s", ChannelNames[(int)c.channel]);
                    ImGui::TableSetColumnIndex(2); ImGui::Text("%dmo", c.durationMonths);
                    ImGui::TableSetColumnIndex(3);
                    ImGui::TextColored(ImVec4(0.5f,0.9f,0.5f,1.f),
                        "$%.0f", c.agencyFee * c.durationMonths);
                }
                ImGui::EndTable();
            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::End();
}
