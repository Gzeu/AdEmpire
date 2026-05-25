#include "Newsfeed.h"
#include "imgui.h"

std::vector<NewsEntry> Newsfeed::s_entries;

void Newsfeed::PushNews(const std::string& text, float r, float g, float b) {
    s_entries.push_back({text, r, g, b});
    if (s_entries.size() > 100) s_entries.erase(s_entries.begin());
}

void Newsfeed::Render(GameState& gs) {
    if (!gs.showNewsfeed) return;
    ImGui::SetNextWindowPos(ImVec2(325, 40), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(950, 640), ImGuiCond_Always);
    ImGui::Begin("Newsfeed", &gs.showNewsfeed,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    ImGui::TextColored(ImVec4(1.f,0.8f,0.2f,1.f), "Active Market Events (%d):",
        (int)gs.activeEvents.size());
    ImGui::Spacing();
    if (gs.activeEvents.empty())
        ImGui::TextDisabled("Market is stable. No active events.");
    for (auto& ev : gs.activeEvents) {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.14f,0.11f,0.04f,0.95f));
        if (ImGui::BeginChild(ev.title.c_str(), ImVec2(-1, 68), true)) {
            ImGui::TextColored(ImVec4(1.f,0.75f,0.1f,1.f), "%s", ev.title.c_str());
            ImGui::SameLine(400);
            ImGui::TextColored(ImVec4(0.6f,0.9f,1.f,1.f), "%d month(s) remaining", ev.monthsLeft);
            ImGui::TextWrapped("%s", ev.description.c_str());
            ImGui::TextColored(ImVec4(0.5f,1.f,0.5f,1.f), "Impact: %s", ev.impact.c_str());
        }
        ImGui::EndChild();
        ImGui::PopStyleColor();
    }
    ImGui::Separator();
    ImGui::Text("News History (%d entries):", (int)s_entries.size());
    if (ImGui::BeginChild("##newslog", ImVec2(-1, -1), false,
        ImGuiWindowFlags_HorizontalScrollbar)) {
        for (int i = (int)s_entries.size() - 1; i >= 0; i--) {
            auto& e = s_entries[i];
            ImGui::TextColored(ImVec4(e.r, e.g, e.b, 1.f), "%s", e.text.c_str());
        }
        ImGui::EndChild();
    }
    ImGui::End();
}
