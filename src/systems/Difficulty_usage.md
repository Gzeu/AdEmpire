# DifficultySystem — Integration Guide

## 1. Add to GameState.h (or main.cpp)
```cpp
Difficulty selectedDifficulty = Difficulty::Normal;
```

## 2. In MainMenu.cpp — add difficulty selector before "New Game" button:
```cpp
static int diffIdx = 1; // 0=Easy, 1=Normal, 2=Hard
const char* diffs[] = { "Easy", "Normal", "Hard" };
ImGui::Combo("Difficulty", &diffIdx, diffs, 3);
auto cfg = DifficultySystem::Get((Difficulty)diffIdx);
ImGui::TextColored(ImVec4(0.7f,0.7f,0.7f,1.f), "%s", cfg.description.c_str());

// In New Game block:
DifficultySystem::Apply((Difficulty)diffIdx, gs);
```

## 3. Hard mode event frequency:
```cpp
// In EventSystem::TryTriggerEvent — replace hardcoded 30:
float chance = DifficultySystem::Get(gs.difficulty).eventFrequency;
if ((float)(rand() % 100) / 100.f >= chance) return;
```
