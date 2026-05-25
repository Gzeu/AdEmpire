// DashboardTabs.inl — v1.0
// Included inside Dashboard::Render() after the existing Overview section.
// Wraps the entire dashboard body with BeginTabBar.
//
// Usage in Dashboard.cpp:
//   Replace the direct body render with:
//
//   if (ImGui::BeginTabBar("##dash_tabs")) {
//       if (ImGui::BeginTabItem("  Overview  ")) {
//           /* existing Dashboard body code */
//           ImGui::EndTabItem();
//       }
//       if (ImGui::BeginTabItem(" Live Market ")) {
//           LiveMarketPanel::RenderLiveMarket();
//           ImGui::EndTabItem();
//       }
//       if (ImGui::BeginTabItem("  Newsfeed  ")) {
//           LiveMarketPanel::RenderNewsfeed();
//           ImGui::EndTabItem();
//       }
//       ImGui::EndTabBar();
//   }
//
// Revenue multiplier badge shown above tabs:
//   float mult = MarketEventBridge::Get().GetRevenueMultiplier();
//   ImVec4 mCol = mult >= 1.3f ? ImVec4(0.2f,0.9f,0.4f,1.f)
//              : mult <= 0.7f ? ImVec4(0.9f,0.3f,0.3f,1.f)
//              :                ImVec4(0.8f,0.8f,0.2f,1.f);
//   ImGui::SameLine();
//   ImGui::TextColored(mCol, "  Rev x%.2f", mult);
