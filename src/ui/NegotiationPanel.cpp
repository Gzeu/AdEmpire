#include "NegotiationPanel.h"
#include "../systems/NegotiationSystem.h"
#include "../systems/FitScoreSystem.h"
#include "imgui.h"
#include <cstdio>

void NegotiationPanel::Render(GameState& gs) {
    if (!gs.showNegotiation) return;
    auto& n = gs.negotiation;

    ImGui::SetNextWindowPos(ImVec2(200, 80), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(860, 560), ImGuiCond_Always);
    ImGui::Begin("Negotiation", &gs.showNegotiation,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    // ── Find client
    Client* cl = nullptr;
    for (auto& c : gs.clients)
        if (c.id == n.clientId) { cl = &c; break; }
    if (!cl) { ImGui::Text("No client found."); ImGui::End(); return; }

    // ── Header
    ImGui::TextColored(ImVec4(0.3f,0.8f,1.f,1.f),
        "Negotiating with: %s", cl->name.c_str());
    ImGui::SameLine(500);
    ImGui::TextColored(ImVec4(0.7f,0.7f,0.7f,1.f),
        "Stage: %s", NegotiationSystem::GetStageTitle(n.stage).c_str());
    ImGui::Separator();

    // ── Two columns
    ImGui::Columns(2, "negcols", true);

    // LEFT: client info + mood + fit
    ImGui::TextColored(ImVec4(1.f,0.8f,0.3f,1.f), "Client Profile");
    ImGui::Text("Industry: %s", IndustryNames[(int)cl->industry]);
    ImGui::Text("Archetype: %s", ArchetypeNames[(int)cl->archetype]);
    ImGui::Text("Budget: $%.0f/mo", cl->budget);
    ImGui::Spacing();

    // Mood bar
    ImVec4 moodCol = n.clientMood > 0.65f ? ImVec4(0.3f,1.f,0.4f,1.f)
                   : n.clientMood > 0.40f ? ImVec4(1.f,0.8f,0.2f,1.f)
                   : ImVec4(1.f,0.3f,0.3f,1.f);
    char moodLabel[32];
    snprintf(moodLabel, 32, "Mood: %.0f%%", n.clientMood * 100.f);
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, moodCol);
    ImGui::ProgressBar(n.clientMood, ImVec2(-1, 18), moodLabel);
    ImGui::PopStyleColor();
    ImGui::Spacing();

    // FitScore breakdown
    ImGui::TextColored(ImVec4(0.7f,0.7f,1.f,1.f), "Fit Score: %.0f / 100", n.fitScore.total());
    ImGui::Text("  Channel Fit:    %.0f%%", n.fitScore.channel * 100.f);
    ImGui::Text("  Industry Exp:   %.0f%%", n.fitScore.industry * 100.f);
    ImGui::Text("  Reputation:     %.0f%%", n.fitScore.reputation * 100.f);
    ImGui::Text("  Capacity:       %.0f%%", n.fitScore.capacity * 100.f);
    ImGui::Spacing();

    float winChance = FitScoreSystem::WinChance(n.fitScore, gs.stats.reputation);
    winChance += (n.clientMood - 0.5f) * 0.4f;
    winChance = std::clamp(winChance, 0.f, 0.97f);
    ImGui::TextColored(ImVec4(0.4f,1.f,0.6f,1.f),
        "Win Probability: %.0f%%", winChance * 100.f);

    // Pressure warning
    if (n.playerPressure > 0)
        ImGui::TextColored(ImVec4(1.f,0.5f,0.2f,1.f),
            "Pressure: %d/3 (too high = client walks)", n.playerPressure);

    ImGui::NextColumn();

    // RIGHT: dialogue + actions
    ImGui::TextColored(ImVec4(1.f,0.8f,0.3f,1.f), "Client Says:");
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.10f,0.12f,0.20f,0.9f));
    ImGui::BeginChild("##dialogue", ImVec2(-1, 80), true);
    ImGui::TextWrapped("%s", n.lastMessage.c_str());
    ImGui::EndChild();
    ImGui::PopStyleColor();
    ImGui::Spacing();

    // Offer display
    ImGui::Text("Your Offer:");
    ImGui::Text("  Budget:   $%.0f/mo", n.offeredBudget);
    ImGui::Text("  Channel:  %s",  ChannelNames[(int)n.offeredChannel]);
    ImGui::Text("  Contract: %s",  ContractTypeNames[(int)n.offeredContract]);
    ImGui::Separator();

    // Action buttons
    if (n.stage != NegotiationStage::Closed) {
        ImGui::TextColored(ImVec4(0.6f,0.9f,1.f,1.f), "Actions:");
        float bw = 195.f;

        if (ImGui::Button("Increase Budget (+10%)",  ImVec2(bw, 30)))
            NegotiationSystem::IncreaseOffer(gs);
        ImGui::SameLine();
        if (ImGui::Button("Decrease Budget (-10%)",  ImVec2(bw, 30)))
            NegotiationSystem::DecreaseOffer(gs);

        if (ImGui::Button("Offer Annual Contract",   ImVec2(bw, 30)))
            NegotiationSystem::OfferLongContract(gs);
        ImGui::SameLine();
        if (ImGui::Button("Offer Monthly Contract",  ImVec2(bw, 30)))
            NegotiationSystem::PressForShortContract(gs);

        // Channel selector
        ImGui::Text("Pitch Channel:");
        for (int i = 0; i < 6; i++) {
            if (i > 0) ImGui::SameLine();
            bool sel = ((int)n.offeredChannel == i);
            if (sel) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f,0.7f,0.2f,1.f));
            if (ImGui::Button(ChannelNames[i], ImVec2(0, 26)))
                n.offeredChannel = (ChannelType)i;
            if (sel) ImGui::PopStyleColor();
        }
        ImGui::Spacing();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f,0.5f,0.9f,1.f));
        if (ImGui::Button("Build Rapport (+mood)",   ImVec2(bw, 30)))
            NegotiationSystem::BuildRapport(gs);
        ImGui::PopStyleColor();
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f,0.4f,0.1f,1.f));
        if (ImGui::Button("Press for Deal (risky)",  ImVec2(bw, 30)))
            NegotiationSystem::PressForDeal(gs);
        ImGui::PopStyleColor();

        ImGui::Spacing();
        if (n.stage == NegotiationStage::FinalOffer) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f,0.7f,0.3f,1.f));
            if (ImGui::Button("CLOSE DEAL", ImVec2(390, 40)))
                NegotiationSystem::Resolve(gs);
            ImGui::PopStyleColor();
        } else {
            if (ImGui::Button("Next Stage >>", ImVec2(190, 34)))
                NegotiationSystem::Advance(gs);
        }
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f,0.1f,0.1f,1.f));
        if (ImGui::Button("Cancel Negotiation", ImVec2(190, 34)))
            NegotiationSystem::Cancel(gs);
        ImGui::PopStyleColor();
    } else {
        // Closed — show result
        if (n.wonDeal)
            ImGui::TextColored(ImVec4(0.3f,1.f,0.4f,1.f), "✅ Deal won! Client added.");
        else
            ImGui::TextColored(ImVec4(1.f,0.3f,0.3f,1.f), "❌ Deal lost.");
        ImGui::Text("%s", n.lastMessage.c_str());
        if (ImGui::Button("Close", ImVec2(200, 36)))
            gs.showNegotiation = false;
    }

    ImGui::Columns(1);
    ImGui::End();
}
