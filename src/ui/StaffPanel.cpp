#include "StaffPanel.h"
#include "imgui.h"
#include <cstdlib>
#include <cstdio>

static const char* STAFF_NAMES[] = {
    "Alex Popescu","Maria Ionescu","Dan Radu","Elena Stan",
    "Mihai Popa","Ioana Constantin","Bogdan Gheorghe","Alina Dumitru"
};
static const int STAFF_NAME_COUNT = 8;

void StaffPanel::Render(GameState& gs) {
    if (!gs.showStaff) return;
    ImGui::SetNextWindowPos(ImVec2(325, 40), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(700, 560), ImGuiCond_Always);
    ImGui::Begin("Staff", &gs.showStaff,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    ImGui::Text("Monthly Payroll: $%.0f", gs.monthlyExpenses);
    ImGui::Separator();

    if (!gs.staff.empty()) {
        ImGui::Text("Current Team (%d employees):", static_cast<int>(gs.staff.size()));
        if (ImGui::BeginTable("##stafftable", 4,
            ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
            ImGui::TableSetupColumn("Name",   ImGuiTableColumnFlags_WidthFixed, 160.f);
            ImGui::TableSetupColumn("Role",   ImGuiTableColumnFlags_WidthFixed, 160.f);
            ImGui::TableSetupColumn("Salary", ImGuiTableColumnFlags_WidthFixed, 80.f);
            ImGui::TableSetupColumn("Skill");
            ImGui::TableHeadersRow();
            for (const auto& s : gs.staff) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("%s", s.name.c_str());
                ImGui::TableSetColumnIndex(1); ImGui::Text("%s", RoleNames[static_cast<int>(s.role)]);
                ImGui::TableSetColumnIndex(2); ImGui::Text("$%.0f", s.salary);
                ImGui::TableSetColumnIndex(3);
                char sl[16]; snprintf(sl, sizeof(sl), "%.0f%%", s.skill * 100.f);
                ImGui::ProgressBar(s.skill, ImVec2(-1.f, 14.f), sl);
            }
            ImGui::EndTable();
        }
        ImGui::Separator();
    }

    ImGui::Text("Hire New Staff:");
    ImGui::Spacing();
    for (int r = 0; r < 6; r++) {
        ImGui::PushID(r);
        float sal       = RoleSalaries[r];
        bool  canAfford = gs.budget > sal * 2.f;
        if (!canAfford) ImGui::BeginDisabled();
        char btnLabel[80];
        snprintf(btnLabel, sizeof(btnLabel), "Hire %s  -  $%.0f/mo", RoleNames[r], sal);
        if (ImGui::Button(btnLabel, ImVec2(400.f, 32.f))) {
            StaffMember sm;
            sm.id          = gs.nextStaffId++;
            sm.name        = STAFF_NAMES[rand() % STAFF_NAME_COUNT];
            sm.role        = static_cast<StaffRole>(r);
            sm.salary      = sal;
            sm.skill       = 0.40f + static_cast<float>(rand() % 40) / 100.f;
            sm.monthsHired = 0;
            gs.staff.push_back(sm);
        }
        if (!canAfford) ImGui::EndDisabled();
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.5f,0.9f,0.5f,1.f),
            "+%.0f%% boost to %s campaigns",
            RoleBonus[r] * 100.f, ChannelNames[r]);
        ImGui::PopID();
    }
    ImGui::End();
}
