# ImGui Features Used & Planned in AdEmpire

Reference document — maps ImGui capabilities to AdEmpire systems.

---

## ✅ Active (v0.9 / v1.0)

| Feature | ImGui API | AdEmpire Usage |
|---|---|---|
| **Docking** | `ImGuiConfigFlags_DockingEnable` + `DockBuilder*` | `DockSpaceLayout.h` — 4-zone persistent layout |
| **Custom Font** | `io.Fonts->AddFontFromFileTTF()` | Roboto-Medium 16px via `Theme::Init()` |
| **Accent helpers** | `PushStyleColor` / `PopStyleColor` | `Theme::PushAccentButton()` on CTA buttons |
| **DrawList charts** | `ImDrawList::AddRectFilled`, `AddText` | `ChartRenderer.h` — revenue bar charts |
| **Tables API** | `ImGui::BeginTable` + `TableSetupColumn` | Monthly Report, Staff Panel, Leaderboard |
| **Modals** | `ImGui::OpenPopup` + `BeginPopupModal` | `EventPopup.h` — blocks month advance |
| **Toast overlay** | Immediate-mode windows + alpha fade | `ToastSystem.h` — max 6 stacked notifications |

---

## 🟡 Recommended Next (v1.0 → v1.1)

### 1. `ImGui::InputTextWithHint` — Agency Name Field
```cpp
// In AgencyBrandingPanel:
ImGui::InputTextWithHint("##name", "Enter agency name...",
    gs.agencyName, sizeof(gs.agencyName));
```
Instant UX improvement — shows placeholder text when empty.

### 2. `ImPlot` (sister library) — Professional Charts
```cpp
// Replace ChartRenderer.h with ImPlot:
#include "implot.h"
if (ImPlot::BeginPlot("Revenue", ImVec2(-1, 200))) {
    ImPlot::PlotBars("Monthly", stats.revenueHistory.data(),
        stats.revenueHistory.size());
    ImPlot::EndPlot();
}
```
ImPlot gives: zoom, pan, tooltips, legends, line + bar + scatter + heatmap.
Add as submodule: `git submodule add https://github.com/epezent/implot lib/implot`

### 3. `ImGui::BeginTabBar` — Dashboard Tab Navigation
```cpp
if (ImGui::BeginTabBar("##DashboardTabs")) {
    if (ImGui::BeginTabItem("Overview"))  { /* KPIs */ ImGui::EndTabItem(); }
    if (ImGui::BeginTabItem("Channels"))  { /* per-channel bars */ ImGui::EndTabItem(); }
    if (ImGui::BeginTabItem("Clients"))   { /* client table */ ImGui::EndTabItem(); }
    ImGui::EndTabBar();
}
```

### 4. `ImGui::SetNextWindowSize` + `ImGuiCond_FirstUseEver` — Smarter Window Init
```cpp
ImGui::SetNextWindowSize(ImVec2(420, 600), ImGuiCond_FirstUseEver);
ImGui::SetNextWindowPos(ImVec2(100, 80),  ImGuiCond_FirstUseEver);
```
Panels open at a sensible size/position the first time, then user can resize.

### 5. `ImGui::GetContentRegionAvail()` — Fluid Chart Sizing
```cpp
ImVec2 avail = ImGui::GetContentRegionAvail();
// Stretch chart to fill whatever space the docked panel has:
ImGui::PlotHistogram("Revenue", data, count, 0, NULL, 0, max_val,
    ImVec2(avail.x, 120));
```
Charts automatically fill the docked panel — no hardcoded pixel widths.

### 6. `ImGui::PushFont` / `PopFont` — Heading Hierarchy
```cpp
// At init: load two sizes
ImFont* fontLarge = io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto-Medium.ttf", 22.0f);
ImFont* fontBody  = io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto-Medium.ttf", 16.0f);

// Usage:
ImGui::PushFont(fontLarge);
ImGui::Text("📊 Agency Dashboard");
ImGui::PopFont();
```

### 7. `ImGui::Image` + OpenGL Texture — Logo in Branding Panel
```cpp
// Load PNG via stb_image, upload to GL texture:
GLuint texID = LoadTexture("assets/logos/logo_dark.png");
ImGui::Image((ImTextureID)(intptr_t)texID, ImVec2(80, 80));
```
Shows actual PNG logos in `AgencyBrandingPanel` instead of color swatches.

### 8. `ImGui::ProgressBar` — Campaign Progress Rings
```cpp
char overlay[32];
snprintf(overlay, sizeof(overlay), "%.0f%%", progress * 100.f);
ImGui::ProgressBar(progress, ImVec2(-1, 18), overlay);
```
Visual campaign progress bars in the Campaign Editor — much clearer than numbers.

---

## 🔵 Future / Experimental

| Feature | Effort | Benefit |
|---|---|---|
| `ImGuiConfigFlags_ViewportsEnable` | Medium | Tear off panels to separate OS windows |
| `ImGui::GetDrawData` → custom renderer | High | Replace OpenGL3 with Vulkan backend (from `example_glfw_vulkan`) |
| Android port | High | `example_android_opengl3` — swap GLFW for SDL2, add touch input |
| `ImGui::DebugCheckVersionAndDataLayout` | Low | CI compile-time ABI check |
| Node editor (imnodes lib) | Medium | Visual campaign flow builder |

---

## Font Setup (CMakeLists.txt addition)

```cmake
# Auto-download Roboto-Medium.ttf if missing
set(FONT_DIR "${CMAKE_SOURCE_DIR}/assets/fonts")
set(FONT_FILE "${FONT_DIR}/Roboto-Medium.ttf")
file(MAKE_DIRECTORY ${FONT_DIR})
if(NOT EXISTS ${FONT_FILE})
  message(STATUS "Downloading Roboto-Medium.ttf...")
  file(DOWNLOAD
    "https://github.com/google/fonts/raw/main/apache/roboto/static/Roboto-Medium.ttf"
    ${FONT_FILE}
    SHOW_PROGRESS
    STATUS download_status
  )
endif()
```

Add this block before `add_executable(AdEmpire ...)` in CMakeLists.txt.
