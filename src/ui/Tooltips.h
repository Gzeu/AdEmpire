#pragma once
#include "imgui.h"
#include "../core/GameState.h"

// Central tooltip definitions for all game concepts.
// Usage: if (ImGui::IsItemHovered()) Tooltips::Channel(ChannelType::Social);
namespace Tooltips {

    inline void Channel(ChannelType ch) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(320.f);
        switch (ch) {
        case ChannelType::Social:
            ImGui::TextColored(ImVec4(0.3f,0.8f,1.f,1.f), "Social Media");
            ImGui::Separator();
            ImGui::Text("Best for: Food, Fashion, Gaming");
            ImGui::Text("Reach/$ : High  |  CTR: Medium");
            ImGui::TextWrapped("Boosted by Algorithm events. Volatile — watch the Newsfeed.");
            break;
        case ChannelType::SEO:
            ImGui::TextColored(ImVec4(0.3f,0.8f,1.f,1.f), "SEO");
            ImGui::Separator();
            ImGui::Text("Best for: Tech, Finance, Health");
            ImGui::Text("Reach/$ : Low   |  CTR: High");
            ImGui::TextWrapped("Slower burn but highest conversion. Hit hard by Google Core Updates.");
            break;
        case ChannelType::Email:
            ImGui::TextColored(ImVec4(0.3f,0.8f,1.f,1.f), "Email Marketing");
            ImGui::Separator();
            ImGui::Text("Best for: Finance, Education, Retail");
            ImGui::Text("Reach/$ : Very High  |  CTR: Low");
            ImGui::TextWrapped("Mass reach at low cost. GDPR events can cripple lists temporarily.");
            break;
        case ChannelType::Influencer:
            ImGui::TextColored(ImVec4(0.3f,0.8f,1.f,1.f), "Influencer");
            ImGui::Separator();
            ImGui::Text("Best for: Fashion, Gaming, Food");
            ImGui::Text("Reach/$ : Medium  |  CTR: High");
            ImGui::TextWrapped("High engagement per view. Scandal events can cut effectiveness by 60%%.");
            break;
        case ChannelType::PR:
            ImGui::TextColored(ImVec4(0.3f,0.8f,1.f,1.f), "PR");
            ImGui::Separator();
            ImGui::Text("Best for: Finance, Health, Tech");
            ImGui::Text("Reach/$ : Low   |  CTR: Low");
            ImGui::TextWrapped("Builds reputation over time. Amplified by Award Season events.");
            break;
        case ChannelType::PaidSearch:
            ImGui::TextColored(ImVec4(0.3f,0.8f,1.f,1.f), "Paid Search");
            ImGui::Separator();
            ImGui::Text("Best for: Retail, Finance, Health");
            ImGui::Text("Reach/$ : Medium  |  CTR: Medium-High");
            ImGui::TextWrapped("Reliable and consistent. Reduced by Cookie Apocalypse events.");
            break;
        }
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }

    inline void FitScore(float score) {
        ImGui::BeginTooltip();
        ImGui::TextColored(ImVec4(0.3f,0.8f,1.f,1.f), "Fit Score: %.0f / 100", score);
        ImGui::Separator();
        ImGui::TextWrapped("Measures how well your agency matches this client:");
        ImGui::BulletText("Channel Fit    — do you have staff for their preferred channel?");
        ImGui::BulletText("Industry Exp   — have you worked with this industry before?");
        ImGui::BulletText("Reputation Fit — agency reputation vs. client expectations.");
        ImGui::BulletText("Capacity Fit   — can you take on another client?");
        ImGui::Separator();
        if (score >= 75)
            ImGui::TextColored(ImVec4(0.3f,1.f,0.4f,1.f), "Strong fit — high win chance.");
        else if (score >= 50)
            ImGui::TextColored(ImVec4(1.f,0.8f,0.2f,1.f), "Moderate fit — negotiation helps.");
        else
            ImGui::TextColored(ImVec4(1.f,0.3f,0.3f,1.f), "Weak fit — build reputation or specializations first.");
        ImGui::EndTooltip();
    }

    inline void Satisfaction(float sat) {
        ImGui::BeginTooltip();
        ImGui::TextColored(ImVec4(0.3f,0.8f,1.f,1.f), "Client Satisfaction: %.0f%%", sat);
        ImGui::Separator();
        ImGui::TextWrapped("Satisfaction affects contract renewal and poaching risk:");
        ImGui::BulletText("< 40%%  — vulnerable to AI poaching");
        ImGui::BulletText("40-70%% — stable, renews normally");
        ImGui::BulletText("> 70%%  — loyal, generates referrals");
        ImGui::Separator();
        ImGui::TextWrapped("Improve by: hitting revenue targets, running active campaigns, specializing in their industry.");
        ImGui::EndTooltip();
    }

    inline void MarketShare(float share) {
        ImGui::BeginTooltip();
        ImGui::TextColored(ImVec4(0.3f,0.8f,1.f,1.f), "Market Share: %.1f%%", share);
        ImGui::Separator();
        ImGui::TextWrapped("Your agency's share of the total ad market.");
        ImGui::BulletText("Grows with: successful campaigns, new clients, reputation.");
        ImGui::BulletText("Shrinks when: clients are poached, campaigns underperform.");
        ImGui::Separator();
        ImGui::TextColored(ImVec4(1.f,0.8f,0.2f,1.f), "Win at 35%% market share.");
        if (share >= 25.f)
            ImGui::TextColored(ImVec4(0.3f,1.f,0.4f,1.f), "Almost there!");
        ImGui::EndTooltip();
    }

    inline void ROI(float roi) {
        ImGui::BeginTooltip();
        ImGui::TextColored(ImVec4(0.3f,0.8f,1.f,1.f), "ROI: %.1f%%", roi);
        ImGui::Separator();
        ImGui::TextWrapped("Return on Investment = (Agency Revenue / Budget Spent - 1) x 100");
        ImGui::BulletText("> 50%%  — excellent campaign");
        ImGui::BulletText("10-50%% — good");
        ImGui::BulletText("0-10%%  — marginal");
        ImGui::BulletText("< 0%%   — losing money");
        ImGui::Separator();
        ImGui::TextWrapped("Improve ROI: hire relevant staff, unlock specializations, choose matching channels.");
        ImGui::EndTooltip();
    }

    inline void StaffRole(StaffRole role) {
        ImGui::BeginTooltip();
        ImGui::TextColored(ImVec4(0.3f,0.8f,1.f,1.f), "%s", RoleNames[(int)role]);
        ImGui::Separator();
        switch (role) {
        case StaffRole::SocialMediaManager:
            ImGui::TextWrapped("Boosts Social Media campaigns. +15%% reach and CTR per skill level.");
            break;
        case StaffRole::SEOSpecialist:
            ImGui::TextWrapped("Improves SEO campaign quality. +20%% conversions per skill level.");
            break;
        case StaffRole::ContentCreator:
            ImGui::TextWrapped("Increases Email campaign reach and quality scores.");
            break;
        case StaffRole::PRManager:
            ImGui::TextWrapped("Boosts both PR and Influencer campaigns. Handles crisis events.");
            break;
        case StaffRole::DataAnalyst:
            ImGui::TextWrapped("Improves all campaign quality scores. +30%% effectiveness globally.");
            break;
        case StaffRole::AccountManager:
            ImGui::TextWrapped("Slows client satisfaction decay. Increases contract renewal rates.");
            break;
        }
        ImGui::Text("Salary: $%.0f/month", RoleSalaries[(int)role]);
        ImGui::EndTooltip();
    }

    inline void Budget(float budget) {
        ImGui::BeginTooltip();
        ImGui::TextColored(ImVec4(0.3f,0.8f,1.f,1.f), "Agency Budget: $%.0f", budget);
        ImGui::Separator();
        ImGui::TextWrapped("Your available cash. Spend it on campaigns and staff.");
        ImGui::BulletText("Negative budget reduces client trust.");
        ImGui::BulletText("Game over if budget < -$50,000.");
        ImGui::BulletText("Budget grows with agency fees (18%% of client revenue).");
        ImGui::EndTooltip();
    }
}
