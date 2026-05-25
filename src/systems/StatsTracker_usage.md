# StatsTracker — Integration Guide

Call these from `main.cpp` / `Simulation.cpp` at the right moments:

```cpp
// After each campaign processes its month:
StatsTracker::Get().RecordCampaignRevenue(c.channel, c.revenue, c.budget);

// After Simulation::AdvanceMonth():
StatsTracker::Get().RecordMonth(gs);

// After a pitch attempt (success or fail):
StatsTracker::Get().RecordPitch(client.industry, won);
```

Render in the navbar / panel:
```cpp
ReportPanel::Render(gs);
```

On "Next Month" button:
```cpp
ReportPanel::GenerateMonthlyReport(gs);
```
