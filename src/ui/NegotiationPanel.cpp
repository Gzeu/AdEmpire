#include "NegotiationPanel.h"
#include "../systems/FitScoreSystem.h"
#include "imgui.h"
#include <cstdlib>
#include <algorithm>

int  NegotiationPanel::s_clientId = -1;
bool NegotiationPanel::s_active   = false;

void NegotiationPanel::StartNegotiation(int clientId, GameState& gs) {
    s_clientId = clientId;
    s_active   = true;
    gs.showNegotiation = true;

    // init negotiation state
    gs.negotiation.stage        = NegotiationStage::Intro;
    gs.negotiation.clientMood   = 60.f;
    gs.negotiation.pressure     = 0;
    gs.negotiation.offeredBudget= 0.f;
    gs.negotiation.won          = false;
    gs.negotiation.closed       = false;

    for (auto& cl : gs.clients)
        if (cl.id == clientId)
            gs.negotiation.offeredBudget = cl.budget;
}

void NegotiationPanel::Render(GameState& gs) {
    if (!gs.showNegotiation || !s_active) return;

    // Find client
    Client* cl = nullptr;
    for (auto& c : gs.clients)
        if (c.id == s_clientId) { cl = &c; break; }
    if (!cl) { s_active = false; return; }

    ImGui::SetNextWindowPos(ImVec2(200, 80), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(880, 540), ImGuiCond_Always);
    ImGui::Begin("Negotiation", &gs.showNegotiation,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    // ─ LEFT: Client profile + FitScore
    ImGui::BeginChild("##neg_left", ImVec2(280, -1), true);
    ImGui::TextColored(ImVec4(0.3f,0.8f,1.f,1.f), "%s", cl->name.c_str());
    ImGui::Text("%s", IndustryNames[(int)cl->industry]);
    ImGui::Text("Budget: $%.0f/mo", cl->budget);
    ImGui::Separator();

    FitScore fit = FitScoreSystem::Calculate(*cl, gs);
    float total  = fit.total();
    float winP   = FitScoreSystem::GetWinProbability(*cl, gs);

    ImGui::TextColored(ImVec4(1.f,0.8f,0.2f,1.f), "Fit Score: %.0f / 100", total);
    char fsl[16]; snprintf(fsl,16,"Fit %.0f%%",total);
    ImGui::ProgressBar(total/100.f, ImVec2(-1,14), fsl);
    ImGui::Spacing();
    ImGui::Text("  Channel fit:    %.0f", fit.channel);
    ImGui::Text("  Industry exp:   %.0f", fit.industry);
    ImGui::Text("  Reputation:     %.0f", fit.reputation);
    ImGui::Text("  Capacity:       %.0f", fit.capacity);
    ImGui::Separator();
    ImGui::TextColored(ImVec4(0.4f,1.f,0.5f,1.f), "Win chance: %.0f%%", winP*100.f);
    ImGui::EndChild();

    ImGui::SameLine();

    // ─ RIGHT: Negotiation dialog
    ImGui::BeginChild("##neg_right", ImVec2(-1, -1), true);

    static const char* STAGE_NAMES[] = {
        "Introduction","Budget Discussion","Channel Selection",
        "Contract Terms","Final Offer","Closed"
    };
    int stageIdx = (int)gs.negotiation.stage;
    ImGui::TextColored(ImVec4(0.8f,0.8f,0.3f,1.f),
        "Stage %d/5: %s", stageIdx+1, STAGE_NAMES[stageIdx]);
    ImGui::Separator();

    // Mood bar
    float mood = gs.negotiation.clientMood;
    ImVec4 moodCol = mood > 65 ? ImVec4(0.3f,1.f,0.3f,1.f)
                   : mood > 35 ? ImVec4(1.f,0.8f,0.2f,1.f)
                               : ImVec4(1.f,0.3f,0.3f,1.f);
    char moodLabel[24]; snprintf(moodLabel,24,"Mood %.0f%%",mood);
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, moodCol);
    ImGui::ProgressBar(mood/100.f, ImVec2(-1,18), moodLabel);
    ImGui::PopStyleColor();
    ImGui::Text("Pressure: %d/3", gs.negotiation.pressure);
    ImGui::Spacing();

    // Stage dialog text
    static const char* DIALOGS[] = {
        "Nice to meet you! We've heard great things about your agency.",
        "We have a monthly budget in mind. Let's discuss what works.",
        "Which channels do you recommend for our industry?",
        "Let's talk about contract length and terms.",
        "This is our final offer. Do we have a deal?",
        "Negotiation complete."
    };
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.08f,0.1f,0.18f,1.f));
    ImGui::BeginChild("##dialog_box", ImVec2(-1, 60), true);
    ImGui::TextWrapped("%s", DIALOGS[stageIdx]);
    ImGui::EndChild();
    ImGui::PopStyleColor();
    ImGui::Spacing();

    // Budget display
    ImGui::Text("Offered budget: $%.0f/mo", gs.negotiation.offeredBudget);
    ImGui::Spacing();

    if (gs.negotiation.closed) {
        if (gs.negotiation.won)
            ImGui::TextColored(ImVec4(0.3f,1.f,0.3f,1.f), "\xE2\x9C\x94 Deal closed! Client acquired.");
        else
            ImGui::TextColored(ImVec4(1.f,0.3f,0.3f,1.f), "\xE2\x9C\x98 No deal. Client walked away.");
        if (ImGui::Button("Close", ImVec2(-1,36))) {
            s_active = false;
            gs.showNegotiation = false;
        }
    } else {
        // Action buttons
        float bw = 190.f;
        if (ImGui::Button("Build Rapport (+7% mood)", ImVec2(bw,30))) {
            gs.negotiation.clientMood = std::clamp(gs.negotiation.clientMood + 7.f, 0.f, 100.f);
        }
        ImGui::SameLine();
        if (ImGui::Button("Increase Offer (+10%)", ImVec2(bw,30)))
            gs.negotiation.offeredBudget *= 1.1f;
        ImGui::SameLine();
        if (ImGui::Button("Decrease Offer (-10%)", ImVec2(bw,30))) {
            gs.negotiation.offeredBudget *= 0.9f;
            gs.negotiation.clientMood -= 5.f;
        }

        if (ImGui::Button("Press for Deal (-mood)", ImVec2(bw,30))) {
            gs.negotiation.clientMood -= 10.f;
            gs.negotiation.pressure++;
        }
        ImGui::SameLine();
        if (ImGui::Button("Offer Long Contract", ImVec2(bw,30)))
            gs.negotiation.clientMood += 5.f;
        ImGui::SameLine();
        if (ImGui::Button("Advance Stage", ImVec2(bw,30))) {
            if (stageIdx < 4)
                gs.negotiation.stage = (NegotiationStage)(stageIdx + 1);
            else {
                // Resolve
                float resolveChance = FitScoreSystem::GetWinProbability(*cl, gs);
                resolveChance += (gs.negotiation.clientMood - 50.f) * 0.005f;
                resolveChance -= gs.negotiation.pressure * 0.1f;
                resolveChance  = std::clamp(resolveChance, 0.02f, 0.97f);

                gs.negotiation.won = ((float)(rand()%100)/100.f < resolveChance);
                gs.negotiation.closed = true;

                if (gs.negotiation.won) {
                    cl->active    = true;
                    cl->available = false;
                    cl->budget    = gs.negotiation.offeredBudget;
                    gs.stats.clientsAcquired++;
                    gs.playerMarketShare = std::clamp(gs.playerMarketShare + 1.5f, 0.f, 100.f);
                }
            }
        }

        if (gs.negotiation.pressure >= 3) {
            ImGui::TextColored(ImVec4(1.f,0.3f,0.3f,1.f),
                "Client is uncomfortable! One more push will end the deal.");
        }
    }
    ImGui::EndChild();
    ImGui::End();
}
