#include "StaffPanel.h"
#include "imgui.h"
#include <cstdlib>
#include <cstdio>

static const char* STAFF_NAMES[] = {
    "Alex Popescu","Maria Ionescu","Dan Radu","Elena Stan",
    "Mihai Popa","Ioana Constantin","Bogdan Gheorghe","Alina Dumitru",
    "Cristian Neagu","Raluca Marin","Vlad Dumitrescu","Simona Florescu"
};

void StaffPanel::Render(GameState& gs) {
    if (!gs.showStaff) return;
    ImGui::SetNextWindowPos(ImVec2(325, 40), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(950, 640), ImGuiCond_Always);
    ImGui::Begin("Staff", &gs.showStaff,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    float payroll = 0.f;
    for (auto& s : gs.staff) payroll += s.salary;
    ImGui::Text("Team size: %d members", (int)gs.staff.size());
    ImGui::SameLine(200);
    ImGui::TextColored(ImVec4(1.f,0.6f,0.3f,1.f), "Monthly Payroll: $%.0f", payroll);
    ImGui::Separator();

    if (!gs.staff.empty()) {
        if (ImGui::BeginTable("##stafftbl", 5,
            ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit)) {
            ImGui::TableSetupColumn("Name",       ImGuiTableColumnFlags_WidthFixed, 160);
            ImGui::TableSetupColumn("Role",       ImGuiTableColumnFlags_WidthFixed, 160);
            ImGui::TableSetupColumn("Salary",     ImGuiTableColumnFlags_WidthFixed, 90);
            ImGui::TableSetupColumn("Skill",      ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Months",     ImGuiTableColumnFlags_WidthFixed, 60);
            ImGui::TableHeadersRow();
            for (auto& s : gs.staff) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("%s", s.name.c_str());
                ImGui::TableSetColumnIndex(1); ImGui::Text("%s", RoleNames[(int)s.role]);
                ImGui::TableSetColumnIndex(2); ImGui::Text("$%.0f", s.salary);
                ImGui::TableSetColumnIndex(3);
                char sl[16]; snprintf(sl, 16, "%.0f%%", s.skill * 100.f);
                ImGui::ProgressBar(s.skill, ImVec2(-1, 14), sl);
                ImGui::TableSetColumnIndex(4); ImGui::Text("%d", s.monthsHired);
            }
            ImGui::EndTable();
        }
        ImGui::Separator();
    }

    ImGui::Spacing();
    ImGui::TextColored(ImVec4(0.4f,0.9f,1.f,1.f), "Hire New Staff:");
    ImGui::TextDisabled("Each specialist boosts their channel's campaign performance.");
    ImGui::Spacing();
    for (int r = 0; r < 6; r++) {
        ImGui::PushID(r);
        float sal = RoleSalaries[r];
        bool canAfford = gs.budget > sal * 3.f;
        if (!canAfford) ImGui::BeginDisabled();
        char btnLabel[80];
        snprintf(btnLabel, 80, "  Hire %-18s  $%.0f/mo  ", RoleNames[r], sal);
        if (ImGui::Button(btnLabel, ImVec2(380, 34))) {
            int nameIdx = rand() % 12;
            StaffMember sm;
            sm.id          = gs.nextStaffId++;
            sm.name        = STAFF_NAMES[nameIdx];
            sm.role        = (StaffRole)r;
            sm.salary      = sal;
            sm.skill       = 0.40f + (float)(rand() % 45) / 100.f;
            sm.monthsHired = 0;
            gs.staff.push_back(sm);
            Newsfeed::PushNews("Hired " + sm.name + " as " + RoleNames[r], 0.5f, 0.9f, 1.f);
        }
        if (!canAfford) ImGui::EndDisabled();
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.5f,0.95f,0.5f,1.f),
            "+%.0f%% to %s", RoleBonus[r] * 100.f, ChannelNames[r]);
        if (!canAfford) {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1.f,0.4f,0.3f,1.f), "(need $%.0f)", sal * 3.f);
        }
        ImGui::PopID();
    }
    ImGui::End();
}
