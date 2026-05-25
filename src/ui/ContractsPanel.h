#pragma once
#include "imgui.h"
#include "../core/GameState.h"
#include "../core/ContractSystem.h"
#include "../core/Simulation.h"
#include <string>

// ─── ContractsPanel ──────────────────────────────────────────────────────────
// Dashboard tab: "Contracts" — shows pending offers + active contracts
class ContractsPanel {
public:
    static void Draw(GameState& gs) {
        ContractSystem& cs = Simulation::Contracts();

        ImGui::Spacing();
        ImGui::TextColored(ImVec4(1.f,0.85f,0.1f,1.f), "CONTRACT OFFERS");
        ImGui::SameLine();
        if (cs.PendingCount() > 0) {
            ImGui::TextColored(ImVec4(1.f,0.3f,0.3f,1.f),
                "  [%d NEW]", cs.PendingCount());
        }
        ImGui::Separator();
        ImGui::Spacing();

        if (cs.offers.empty()) {
            ImGui::TextDisabled("No offers right now — advance a month to receive offers.");
        }

        for (auto& offer : cs.offers) {
            offer.seen = true; // mark as seen
            ImGui::PushID(offer.id);

            // Card background
            ImVec2 pos  = ImGui::GetCursorScreenPos();
            float  w    = ImGui::GetContentRegionAvail().x;
            ImGui::GetWindowDrawList()->AddRectFilled(
                pos, ImVec2(pos.x + w, pos.y + 90),
                IM_COL32(40,40,55,220), 6.f);
            ImGui::Dummy(ImVec2(0, 4));

            ImGui::Indent(10);
            ImGui::TextColored(ImVec4(0.9f,0.7f,1.f,1.f), "%s", offer.clientName.c_str());
            ImGui::TextDisabled("%s", offer.description.c_str());
            ImGui::Text("Target: $%.0f  |  Deadline: %d months  |  Bonus: x%.2f  |  Penalty: $%.0f",
                offer.requiredRevenue, offer.deadlineDays,
                offer.bonusMultiplier, offer.penaltyAmount);
            ImGui::Spacing();

            if (ImGui::SmallButton("Accept")) {
                cs.Accept(offer.id, gs);
                ImGui::Unindent(10);
                ImGui::PopID();
                break; // iterator invalidated
            }
            ImGui::SameLine();
            if (ImGui::SmallButton("Decline")) {
                cs.Decline(offer.id);
                ImGui::Unindent(10);
                ImGui::PopID();
                break;
            }
            ImGui::Unindent(10);
            ImGui::Dummy(ImVec2(0, 8));
            ImGui::PopID();
        }

        ImGui::Spacing();
        ImGui::TextColored(ImVec4(0.4f,0.8f,1.f,1.f), "ACTIVE CONTRACTS");
        ImGui::Separator();
        ImGui::Spacing();

        if (cs.active.empty()) {
            ImGui::TextDisabled("No active contracts — accept an offer above.");
        }

        for (auto& c : cs.active) {
            ImGui::PushID(c.id + 1000);
            float pct = c.requiredRevenue > 0.f
                      ? std::min(c.revenueEarned / c.requiredRevenue, 1.f)
                      : 0.f;
            ImVec4 barCol = pct > 0.7f ? ImVec4(0.2f,0.9f,0.4f,1.f)
                          : pct > 0.3f ? ImVec4(1.f,0.8f,0.1f,1.f)
                                       : ImVec4(1.f,0.3f,0.3f,1.f);

            ImGui::Text("%s", c.clientName.c_str());
            ImGui::SameLine();
            ImGui::TextDisabled("  %d months left", c.daysLeft);

            char label[64];
            snprintf(label, sizeof(label), "$%.0f / $%.0f",
                     c.revenueEarned, c.requiredRevenue);
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, barCol);
            ImGui::ProgressBar(pct, ImVec2(-1, 12), label);
            ImGui::PopStyleColor();
            ImGui::Spacing();
            ImGui::PopID();
        }

        ImGui::Spacing();
        ImGui::TextColored(ImVec4(0.5f,0.5f,0.5f,1.f), "HISTORY  (%d)",
            (int)cs.history.size());
        ImGui::Separator();
        for (auto& h : cs.history) {
            ImVec4 col = h.completed ? ImVec4(0.3f,1.f,0.5f,1.f)
                                     : ImVec4(1.f,0.35f,0.35f,1.f);
            ImGui::TextColored(col, "%s  %s  ($%.0f earned)",
                h.clientName.c_str(),
                h.completed ? "[COMPLETED]" : "[FAILED]",
                h.revenueEarned);
        }
    }
};
