#pragma once
// ── SettingsPanel v0.8 ───────────────────────────────────────────────────────
// Audio volume + enable/disable toggle + REST leaderboard server config
// ─────────────────────────────────────────────────────────────────────────────
#include "imgui.h"
#include "UIStyle.h"
#include "../core/GameState.h"
#include "../audio/AudioSystem.h"
#include "../network/LeaderboardClient.h"

namespace SettingsPanel {

inline void Render(GameState& gs) {
    if (!gs.showSettings) return;

    ImGui::SetNextWindowPos(ImVec2(300, 100), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(480, 380), ImGuiCond_FirstUseEver);
    ImGui::Begin("⚙  Settings — v0.8", &gs.showSettings, ImGuiWindowFlags_NoCollapse);

    // ── Audio ───────────────────────────────────────────────────────────────
    ImGui::TextColored(UIStyle::Gold, "🔊 Audio");
    ImGui::Separator();
    ImGui::Spacing();

    auto& audio = AudioSystem::Get();
#ifdef AUDIO_ENABLED
    ImGui::Checkbox("Enable sound effects", &audio.enabled);
    if (audio.enabled) {
        ImGui::SetNextItemWidth(260);
        if (ImGui::SliderFloat("Master Volume", &audio.masterVolume, 0.0f, 1.0f))
            SOUND(ButtonClick);
    } else {
        ImGui::TextColored(UIStyle::Muted, "Audio disabled");
    }
#else
    ImGui::TextColored(UIStyle::Muted, "Audio not compiled.");
    ImGui::TextColored(UIStyle::Muted, "Rebuild with -DAUDIO=ON (requires libopenal-dev)");
#endif

    ImGui::Spacing();
    ImGui::Spacing();

    // ── Network ─────────────────────────────────────────────────────────────
    ImGui::TextColored(UIStyle::Gold, "🌐 Global Leaderboard");
    ImGui::Separator();
    ImGui::Spacing();

    auto& lc = LeaderboardClient::Get();
    static char hostBuf[128]; std::strncpy(hostBuf, lc.host.c_str(), 127);
    static int  portBuf = lc.port;
    static bool tlsBuf  = lc.useTLS;

    ImGui::Text("API Host:");
    ImGui::SetNextItemWidth(320);
    if (ImGui::InputText("##host", hostBuf, sizeof(hostBuf)))
        lc.host = hostBuf;
    ImGui::Text("Port:");
    ImGui::SetNextItemWidth(100);
    if (ImGui::InputInt("##port", &portBuf))
        lc.port = portBuf;
    if (ImGui::Checkbox("Use TLS (HTTPS)", &tlsBuf))
        lc.useTLS = tlsBuf;

    ImGui::Spacing();
    if (ImGui::Button("Test Connection", ImVec2(160, 30))) {
        lc.FetchLeaderboard([](bool ok){
            (void)ok; // result visible in leaderboard panel
        });
    }
    ImGui::SameLine();
    if (lc.fetching.load())      ImGui::TextColored(UIStyle::Muted,    "Testing...");
    else if (lc.fetchOK.load())  ImGui::TextColored(UIStyle::Positive,  "✅ Connected");
    else if (!lc.lastError.empty()) ImGui::TextColored(UIStyle::Negative, "❌ %s", lc.lastError.c_str());

    ImGui::Spacing();
    ImGui::TextColored(UIStyle::Muted, "Default server: adempire-leaderboard.onrender.com");
    ImGui::TextColored(UIStyle::Muted, "Local mock: http://localhost:3000 (port 3000, TLS off)");

    ImGui::End();
}

} // namespace SettingsPanel
