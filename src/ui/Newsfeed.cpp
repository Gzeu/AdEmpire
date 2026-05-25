#include "Newsfeed.h"
#include "imgui.h"

std::vector<NewsEntry> Newsfeed::s_entries;

void Newsfeed::PushNews(const std::string& text, float r, float g, float b) {
    s_entries.push_back({text, r, g, b});
    if (s_entries.size() > 50) s_entries.erase(s_entries.begin());
}

void Newsfeed::Render(GameState& gs) {
    if (!gs.showNewsfeed) return;
    ImGui::SetNextWindowPos(ImVec2(325, 40), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(700, 560), ImGuiCond_Always);
    ImGui::Begin("Newsfeed", &gs.showNewsfeed,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    // Active events panel
    ImGui::TextColored(ImVec4(1.f,0.8f,0.2f,1.f), "Active Market Events:");
    if (gs.activeEvents.empty())
        ImGui::TextDisabled("No active events.");
    for (auto& ev : gs.activeEvents) {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.15f,0.12f,0.05f,0.9f));
        ImGui::BeginChild(ev.title.c_str(), ImVec2(-1, 60), true);
        ImGui::TextColored(ImVec4(1.f,0.7f,0.1f,1.f), "%s", ev.title.c_str());
        ImGui::TextWrapped("%s", ev.description.c_str());
        ImGui::TextColored(ImVec4(0.5f,1.f,0.5f,1.f),
            "Impact: %s | %d months remaining", ev.impact.c_str(), ev.monthsLeft);
        ImGui::EndChild();
        ImGui::PopStyleColor();
    }
    ImGui::Separator();
    ImGui::Text("History:");
    ImGui::BeginChild("##newslog", ImVec2(-1, -1), false);
    for (int i = (int)s_entries.size() - 1; i >= 0; i--) {
        auto& e = s_entries[i];
        ImGui::TextColored(ImVec4(e.r, e.g, e.b, 1.f), "%s", e.text.c_str());
    }
    ImGui::EndChild();
    ImGui::End();
}
