# ToastSystem — Integration Guide

## Setup in main loop (main.cpp)
```cpp
// In game loop, before ImGui::Render():
float dt = ImGui::GetIO().DeltaTime;
ToastSystem::Get().Update(dt);
ToastSystem::Get().Render(); // draws over everything else
```

## Trigger toasts from anywhere:
```cpp
#include "ui/ToastSystem.h"

// Client won:
ToastSystem::Get().Push("Client won: " + client.name, ToastType::Success);

// Campaign completed:
ToastSystem::Get().Push("Campaign \"" + c.name + "\" completed! Fee: $" + ..., ToastType::Info);

// Budget warning:
ToastSystem::Get().Push("Warning: Budget below $2,000!", ToastType::Warning);

// Rival poached client:
ToastSystem::Get().Push("MediaBlaze stole " + client.name + "!", ToastType::Error);

// Goal completed:
ToastSystem::Get().Push("Q1 Goal achieved! +$" + reward, ToastType::Success);

// Achievement:
ToastSystem::Get().Push("Achievement: First Million!", ToastType::Success);
```
