#pragma once
// =============================================================================
// ChartRenderer.h  —  AdEmpire v0.9
// Pure ImDrawList chart primitives: Line, Bar (horizontal), Donut.
// Drop-in: #include "ChartRenderer.h" anywhere, no .cpp needed.
// =============================================================================
#include "imgui.h"
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <cstring>

namespace ChartRenderer {

// ---------------------------------------------------------------------------
// Colour helpers  (ABGR for ImGui)
// ---------------------------------------------------------------------------
static inline ImVec4 Lerp4(ImVec4 a, ImVec4 b, float t) {
    return { a.x+(b.x-a.x)*t, a.y+(b.y-a.y)*t,
             a.z+(b.z-a.z)*t, a.w+(b.w-a.w)*t };
}

// Agency gold, rival colours, muted grid
constexpr ImVec4 kGold   = {0.910f, 0.627f, 0.188f, 1.0f};
constexpr ImVec4 kBlue   = {0.290f, 0.510f, 0.769f, 1.0f};
constexpr ImVec4 kRed    = {0.788f, 0.294f, 0.165f, 1.0f};
constexpr ImVec4 kGreen  = {0.353f, 0.620f, 0.271f, 1.0f};
constexpr ImVec4 kPurple = {0.490f, 0.310f, 0.690f, 1.0f};
constexpr ImVec4 kGrid   = {1.0f,   1.0f,   1.0f,   0.06f};
constexpr ImVec4 kAxis   = {1.0f,   1.0f,   1.0f,   0.18f};

static ImVec4 kRivalColors[3] = { kBlue, kRed, kPurple };

// =============================================================================
// 1. LINE CHART  —  revenue trend (up to 24 months)
// =============================================================================
struct LineChartConfig {
    const char*         title       = "Revenue Trend";
    ImVec2              size        = {0.0f, 140.0f};   // 0 width = fill avail
    float               thickness   = 2.2f;
    bool                fill        = true;              // area fill under line
    bool                dots        = true;
    bool                showGrid    = true;
    int                 gridLines   = 4;
    ImVec4              lineColor   = kGold;
    const char*         yPrefix     = "$";
    const char*         xLabel      = nullptr;
};

void DrawLineChart(
    const std::vector<float>& values,
    const std::vector<std::string>& labels,   // month labels, may be empty
    const LineChartConfig& cfg = {}
) {
    if (values.empty()) return;

    float width = cfg.size.x > 0 ? cfg.size.x : ImGui::GetContentRegionAvail().x;
    float height = cfg.size.y;
    ImVec2 origin = ImGui::GetCursorScreenPos();
    ImDrawList* dl = ImGui::GetWindowDrawList();

    // Reserve a dummy item so ImGui accounts for this space
    ImGui::Dummy({width, height});

    const float padL = 48.0f, padR = 12.0f, padT = 18.0f, padB = labels.empty() ? 18.0f : 30.0f;
    float W = width - padL - padR;
    float H = height - padT - padB;
    ImVec2 bl = { origin.x + padL, origin.y + padT + H };  // bottom-left of plot

    float vmin = *std::min_element(values.begin(), values.end());
    float vmax = *std::max_element(values.begin(), values.end());
    if (vmax == vmin) vmax = vmin + 1.0f;
    float range = vmax - vmin;
    // nice padding
    vmin -= range * 0.05f;
    vmax += range * 0.15f;
    range = vmax - vmin;

    int n = (int)values.size();

    // --- title
    if (cfg.title && cfg.title[0]) {
        ImVec2 ts = ImGui::CalcTextSize(cfg.title);
        dl->AddText({ origin.x + padL + W/2.0f - ts.x/2.0f, origin.y + 2.0f },
                    IM_COL32(200,200,200,180), cfg.title);
    }

    // --- grid
    if (cfg.showGrid) {
        for (int i = 0; i <= cfg.gridLines; ++i) {
            float gy = bl.y - (float)i / cfg.gridLines * H;
            float val = vmin + (float)i / cfg.gridLines * range;
            dl->AddLine({bl.x, gy}, {bl.x + W, gy},
                        ImGui::ColorConvertFloat4ToU32(kGrid), 1.0f);
            // y-axis label
            char buf[16];
            if (fabsf(val) >= 1000.0f)
                snprintf(buf, sizeof(buf), "%s%.0fk", cfg.yPrefix ? cfg.yPrefix : "", val/1000.0f);
            else
                snprintf(buf, sizeof(buf), "%s%.0f", cfg.yPrefix ? cfg.yPrefix : "", val);
            ImVec2 ts = ImGui::CalcTextSize(buf);
            dl->AddText({bl.x - ts.x - 4.0f, gy - ts.y/2.0f},
                        IM_COL32(160,160,160,120), buf);
        }
    }

    // axes
    dl->AddLine({bl.x, bl.y - H}, {bl.x, bl.y},
                ImGui::ColorConvertFloat4ToU32(kAxis), 1.0f);
    dl->AddLine({bl.x, bl.y}, {bl.x + W, bl.y},
                ImGui::ColorConvertFloat4ToU32(kAxis), 1.0f);

    // --- compute points
    auto xpos = [&](int i) { return bl.x + (n <= 1 ? W/2.0f : (float)i/(n-1)*W); };
    auto ypos = [&](float v) { return bl.y - (v - vmin) / range * H; };

    // fill area
    if (cfg.fill && n >= 2) {
        for (int i = 0; i < n-1; ++i) {
            float x0=xpos(i), y0=ypos(values[i]);
            float x1=xpos(i+1), y1=ypos(values[i+1]);
            ImVec2 pts[4] = {
                {x0, bl.y}, {x0, y0}, {x1, y1}, {x1, bl.y}
            };
            ImVec4 fc = cfg.lineColor;
            fc.w = 0.12f;
            dl->AddConvexPolyFilled(pts, 4, ImGui::ColorConvertFloat4ToU32(fc));
        }
    }

    // line segments
    ImU32 lc = ImGui::ColorConvertFloat4ToU32(cfg.lineColor);
    for (int i = 0; i < n-1; ++i)
        dl->AddLine({xpos(i), ypos(values[i])},
                    {xpos(i+1), ypos(values[i+1])}, lc, cfg.thickness);

    // dots + hover tooltip
    for (int i = 0; i < n; ++i) {
        float px = xpos(i), py = ypos(values[i]);
        if (cfg.dots) {
            dl->AddCircleFilled({px, py}, 3.5f, lc);
            dl->AddCircle({px, py}, 3.5f, IM_COL32(30,30,30,200), 12, 1.5f);
        }
        // x labels
        if (!labels.empty() && i < (int)labels.size() && (n <= 12 || i % 3 == 0)) {
            ImVec2 ts = ImGui::CalcTextSize(labels[i].c_str());
            dl->AddText({px - ts.x/2.0f, bl.y + 4.0f},
                        IM_COL32(140,140,140,150), labels[i].c_str());
        }
        // hover
        ImVec2 mp = ImGui::GetMousePos();
        if (fabsf(mp.x - px) < 8.0f && fabsf(mp.y - py) < 8.0f) {
            dl->AddCircleFilled({px, py}, 5.5f, IM_COL32(255,255,255,200));
            char tip[64];
            snprintf(tip, sizeof(tip), "%s%.0f",
                     cfg.yPrefix ? cfg.yPrefix : "", values[i]);
            ImGui::SetTooltip("%s", tip);
        }
    }
}

// =============================================================================
// 2. HORIZONTAL BAR CHART  —  channel performance
// =============================================================================
struct BarChartConfig {
    const char* title     = "Channel Performance";
    ImVec2      size      = {0.0f, 0.0f};  // 0,0 = fit to content
    float       barHeight = 18.0f;
    float       barGap    = 6.0f;
    float       labelW    = 80.0f;  // left column width
    const char* valueSuffix = "";
    bool        showValues  = true;
    ImVec4      baseColor   = kGold;
    bool        colorGrade  = true;  // high=gold, low=muted
};

void DrawHorizontalBars(
    const std::vector<std::string>& labels,
    const std::vector<float>&       values,
    const BarChartConfig& cfg = {}
) {
    if (labels.empty() || values.empty()) return;
    int n = (int)std::min(labels.size(), values.size());

    float width = cfg.size.x > 0 ? cfg.size.x : ImGui::GetContentRegionAvail().x;
    float rowH  = cfg.barHeight + cfg.barGap;
    float totalH = rowH * n + 24.0f;  // +title

    ImVec2 origin = ImGui::GetCursorScreenPos();
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImGui::Dummy({width, totalH});

    float vmax = *std::max_element(values.begin(), values.end());
    if (vmax == 0.0f) vmax = 1.0f;

    float barAreaW = width - cfg.labelW - 60.0f;  // 60 for value text

    if (cfg.title && cfg.title[0]) {
        dl->AddText({origin.x, origin.y + 2.0f},
                    IM_COL32(200,200,200,180), cfg.title);
    }

    for (int i = 0; i < n; ++i) {
        float y = origin.y + 22.0f + i * rowH;
        float frac = values[i] / vmax;

        // label
        dl->AddText({origin.x, y + (cfg.barHeight - ImGui::GetTextLineHeight())/2.0f},
                    IM_COL32(190,190,190,200), labels[i].c_str());

        // background track
        ImVec2 trackMin = {origin.x + cfg.labelW, y};
        ImVec2 trackMax = {origin.x + cfg.labelW + barAreaW, y + cfg.barHeight};
        dl->AddRectFilled(trackMin, trackMax,
                          IM_COL32(255,255,255,12), 3.0f);

        // filled bar
        ImVec4 col = cfg.colorGrade ?
            Lerp4({0.55f,0.55f,0.55f,0.7f}, cfg.baseColor, frac) :
            cfg.baseColor;
        col.w = 0.85f;
        dl->AddRectFilled(trackMin,
                          {trackMin.x + barAreaW * frac, trackMax.y},
                          ImGui::ColorConvertFloat4ToU32(col), 3.0f);

        // value
        if (cfg.showValues) {
            char buf[32];
            snprintf(buf, sizeof(buf), "%.0f%s", values[i], cfg.valueSuffix);
            dl->AddText({trackMax.x + 6.0f,
                         y + (cfg.barHeight - ImGui::GetTextLineHeight())/2.0f},
                        IM_COL32(210,210,210,200), buf);
        }
    }
}

// =============================================================================
// 3. DONUT CHART  —  market share (you + 3 rivals + uncaptured)
// =============================================================================
struct DonutConfig {
    const char* title      = "Market Share";
    float       radius     = 60.0f;
    float       innerRatio = 0.58f;   // donut hole
    int         segments   = 80;
    bool        showLegend = true;
    bool        showCenter = true;    // big % in centre
};

void DrawDonut(
    const std::vector<std::string>& labels,
    const std::vector<float>&       values,  // in [0,1] or raw shares
    const std::vector<ImVec4>&      colors,
    const DonutConfig& cfg = {}
) {
    if (values.empty()) return;
    int n = (int)values.size();

    float total = 0.0f;
    for (float v : values) total += v;
    if (total <= 0.0f) return;

    float diam  = cfg.radius * 2.0f;
    float legendH = cfg.showLegend ? n * 20.0f + 8.0f : 0.0f;
    float totalH = diam + legendH + (cfg.title ? 20.0f : 0.0f);
    float width  = diam + (cfg.showLegend ? 120.0f : 0.0f);

    ImVec2 origin = ImGui::GetCursorScreenPos();
    ImDrawList* dl = ImGui::GetWindowDrawList();
    // let content be wider if legend is beside
    float availW = ImGui::GetContentRegionAvail().x;
    ImGui::Dummy({availW, totalH});

    float titleOff = 0.0f;
    if (cfg.title && cfg.title[0]) {
        ImVec2 ts = ImGui::CalcTextSize(cfg.title);
        dl->AddText({origin.x + availW/2.0f - ts.x/2.0f, origin.y},
                    IM_COL32(200,200,200,180), cfg.title);
        titleOff = 20.0f;
    }

    ImVec2 center = {origin.x + cfg.radius + 2.0f, origin.y + titleOff + cfg.radius};
    float innerR = cfg.radius * cfg.innerRatio;

    // draw slices
    float angle = -M_PI / 2.0f;  // start at 12 o'clock
    for (int i = 0; i < n; ++i) {
        float sweep = (values[i] / total) * 2.0f * M_PI;
        ImU32 col = ImGui::ColorConvertFloat4ToU32(colors[i]);
        // outer arc filled as triangle fan from center, then cut with inner circle
        // We approximate with AddConvexPolyFilled fan
        int segs = std::max(4, (int)(cfg.segments * values[i]/total));
        std::vector<ImVec2> pts;
        pts.reserve(segs + 3);
        // outer ring points
        for (int j = 0; j <= segs; ++j) {
            float a = angle + sweep * j / segs;
            pts.push_back({center.x + cosf(a)*cfg.radius, center.y + sinf(a)*cfg.radius});
        }
        // inner ring points (reversed)
        for (int j = segs; j >= 0; --j) {
            float a = angle + sweep * j / segs;
            pts.push_back({center.x + cosf(a)*innerR, center.y + sinf(a)*innerR});
        }
        dl->AddConvexPolyFilled(pts.data(), (int)pts.size(), col);
        // thin gap line
        float aEnd = angle + sweep;
        dl->AddLine(
            {center.x + cosf(aEnd)*innerR, center.y + sinf(aEnd)*innerR},
            {center.x + cosf(aEnd)*cfg.radius, center.y + sinf(aEnd)*cfg.radius},
            IM_COL32(20,20,20,180), 1.5f);
        angle += sweep;
    }

    // inner hole fill
    dl->AddCircleFilled(center, innerR - 1.0f, IM_COL32(22,20,18,255));

    // center text (player share — first entry assumed to be player)
    if (cfg.showCenter && !labels.empty()) {
        float pct = values[0] / total * 100.0f;
        char big[16], small_lbl[24];
        snprintf(big, sizeof(big), "%.1f%%", pct);
        snprintf(small_lbl, sizeof(small_lbl), "You");
        ImVec2 bts = ImGui::CalcTextSize(big);
        ImVec2 sts = ImGui::CalcTextSize(small_lbl);
        dl->AddText({center.x - bts.x/2.0f, center.y - bts.y/2.0f - 6.0f},
                    ImGui::ColorConvertFloat4ToU32(colors[0]), big);
        dl->AddText({center.x - sts.x/2.0f, center.y + bts.y/2.0f - 4.0f},
                    IM_COL32(160,160,160,200), small_lbl);
    }

    // legend
    if (cfg.showLegend) {
        float lx = origin.x + diam + 14.0f;
        float ly = origin.y + titleOff;
        for (int i = 0; i < n; ++i) {
            float pct = values[i] / total * 100.0f;
            ImU32 col = ImGui::ColorConvertFloat4ToU32(colors[i]);
            dl->AddRectFilled({lx, ly + 4.0f}, {lx + 10.0f, ly + 14.0f}, col, 2.0f);
            char buf[48];
            snprintf(buf, sizeof(buf), "%s  %.1f%%",
                     i < (int)labels.size() ? labels[i].c_str() : "?", pct);
            dl->AddText({lx + 14.0f, ly}, IM_COL32(200,200,200,200), buf);
            ly += 20.0f;
        }
    }
}

// =============================================================================
// 4. CONVENIENCE WRAPPER  —  full StatsPanel chart section
//    Call once per frame inside an ImGui child window.
// =============================================================================
//  marketShares: [0]=player, [1..3]=rivals (values 0-100)
//  revenueHistory: up to 24 floats  (monthly net revenue)
//  channelNames / channelRevenue: 6 channels
void DrawFullStatsCharts(
    const std::vector<float>& revenueHistory,
    const std::vector<std::string>& monthLabels,
    const std::vector<float>& marketShares,
    const std::vector<std::string>& entityNames,
    const std::vector<std::string>& channelNames,
    const std::vector<float>& channelRevenue
) {
    // --- Revenue line chart
    ImGui::Spacing();
    LineChartConfig lc;
    lc.title = "Revenue History (Monthly)";
    lc.size  = {0.0f, 150.0f};
    lc.yPrefix = "$";
    DrawLineChart(revenueHistory, monthLabels, lc);

    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    // --- Side-by-side: donut + bar chart
    float half = ImGui::GetContentRegionAvail().x / 2.0f - 8.0f;

    // Donut
    ImGui::BeginGroup();
    std::vector<ImVec4> shareColors;
    shareColors.push_back(kGold);
    for (int i = 0; i < 3; ++i)
        shareColors.push_back(kRivalColors[i]);
    while ((int)shareColors.size() < (int)marketShares.size())
        shareColors.push_back({0.4f,0.4f,0.4f,0.7f});
    DonutConfig dc;
    dc.title  = "Market Share";
    dc.radius = 56.0f;
    DrawDonut(entityNames, marketShares, shareColors, dc);
    ImGui::EndGroup();

    ImGui::SameLine(0.0f, 16.0f);

    // Bar chart
    ImGui::BeginGroup();
    BarChartConfig bc;
    bc.title   = "Revenue by Channel";
    bc.labelW  = 90.0f;
    bc.valueSuffix = "";
    bc.size.x  = half;
    DrawHorizontalBars(channelNames, channelRevenue, bc);
    ImGui::EndGroup();
}

} // namespace ChartRenderer
