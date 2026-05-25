#include "StaffLeveling.h"
#include "../ui/ToastSystem.h"
#include <algorithm>

int StaffLeveling::GetLevel(const StaffMember& s) {
    if (s.skill >= 0.90f) return 5;
    if (s.skill >= 0.75f) return 4;
    if (s.skill >= 0.60f) return 3;
    if (s.skill >= 0.45f) return 2;
    return 1;
}

const char* StaffLeveling::GetLevelTitle(int level) {
    static const char* titles[] = { "Junior", "Mid", "Senior", "Lead", "Principal" };
    return titles[std::clamp(level - 1, 0, 4)];
}

void StaffLeveling::Tick(GameState& gs) {
    for (auto& s : gs.staff) {
        int prevLevel = GetLevel(s);
        // Skill grows ~0.5% per month, faster in first 6 months
        float growth = (s.monthsHired < 6) ? 0.008f : 0.004f;
        s.skill = std::min(s.skill + growth, 1.0f);
        int newLevel = GetLevel(s);
        if (newLevel > prevLevel) {
            std::string msg = s.name + " leveled up to " +
                GetLevelTitle(newLevel) + " (" + RoleNames[(int)s.role] + ")";
            ToastSystem::Get().Push(msg, ToastType::Info, 4.f);
            // Small salary bump on level up
            s.salary *= 1.08f;
        }
    }
}
