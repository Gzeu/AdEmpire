#include "StaffPanel.h"
#include "imgui.h"
#include <cstdlib>

static const char* STAFF_NAMES[] = {
    "Alex Popescu","Maria Ionescu","Dan Radu","Elena Stan",
    "Mihai Popa","Ioana Constantin","Bogdan Gheorghe","Alina Dumitru"
};

void StaffPanel::Render(GameState& gs) {
    if (!gs.showStaff) return;
    ImGui::SetNextWindowPos(ImVec2(325, 40), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(700, 560), ImGuiCond_Always);
    ImGui::Begin("Staff", &gs.showStaff,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    ImGui::Text("Monthly Payroll: $%.0f", gs.monthlyExpenses);
    ImGui::Separator();

    // Current staff
    if (!gs.staff.empty()) {
        ImGui::Text("Current Team:");
        if (ImGui::BeginTable("##staff", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
            ImGui::TableSetupColumn("Name");
            ImGui::TableSetupColumn("Role");
            ImGui::TableSetupColumn("Salary");
            ImGui::TableSetupColumn("Skill");
            ImGui::TableHeadersRow();
            for (auto& s : gs.staff) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("%s", s.name.c_str());
                ImGui::TableSetColumnIndex(1); ImGui::Text("%s", RoleNames[(int)s.role]);
                ImGui::TableSetColumnIndex(2); ImGui::Text("$%.0f", s.salary);
                ImGui::TableSetColumnIndex(3);
                char sl[16]; snprintf(sl, 16, "%.0f%%", s.skill * 100.f);
                ImGui::ProgressBar(s.skill, ImVec2(-1, 14), sl);
            }
            ImGui::EndTable();
        }
        ImGui::Separator();
    }

    // Hire new
    ImGui::Text("Hire New Staff:");
    for (int r = 0; r < 6; r++) {
        ImGui::PushID(r);
        float sal = RoleSalaries[r];
        bool canAfford = gs.budget > sal * 2;
        if (!canAfford) ImGui::BeginDisabled();
        char btnLabel[64];
        snprintf(btnLabel, 64, "Hire %s  $%.0f/mo", RoleNames[r], sal);
        if (ImGui::Button(btnLabel, ImVec2(400, 32))) {
            int nameIdx = rand() % 8;
            StaffMember sm;
            sm.id     = gs.nextStaffId++;
            sm.name   = STAFF_NAMES[nameIdx];
            sm.role   = (StaffRole)r;
            sm.salary = sal;
            sm.skill  = 0.4f + (float)(rand() % 40) / 100.f; // 0.4-0.8
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
