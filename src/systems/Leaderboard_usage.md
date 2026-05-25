# Leaderboard — Integration Guide

## Load on startup (main.cpp before game loop):
```cpp
Leaderboard::Get().Load();
```

## Submit on Victory or Game Over:
```cpp
// In win/lose overlay — before showing next screen:
LeaderboardPanel::SubmitScore(gs, DifficultySystem::Label(selectedDifficulty));
```

## Show leaderboard panel (add to navbar):
```cpp
static bool showLeaderboard = false;
if (ImGui::Button(" Leaderboard ")) showLeaderboard = !showLeaderboard;
LeaderboardPanel::Render(gs, showLeaderboard);
```

## Persistent save file:
- Saved to `leaderboard.json` in the working directory
- Human-readable JSON, can be manually edited or reset
