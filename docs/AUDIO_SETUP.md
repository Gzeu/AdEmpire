# Audio Setup (OpenAL)

## Install

```bash
# Ubuntu/Debian
sudo apt install libopenal-dev

# macOS
brew install openal-soft

# Windows
# Download from https://www.openal.org/downloads/
```

## Enable in code

1. Open `src/audio/AudioSystem.h`
2. Uncomment `#define AUDIO_ENABLED`
3. Add to `CMakeLists.txt`:

```cmake
find_package(OpenAL REQUIRED)
target_link_libraries(AdEmpire PRIVATE OpenAL::OpenAL)
```

## Add sound files

Place `.wav` files in `assets/audio/`:

```
assets/audio/
├── click.wav           # UI button click
├── launch.wav          # Campaign launched
├── client_won.wav      # Client signed
├── client_lost.wav     # Client lost
├── month_tick.wav      # Month advanced
├── achievement.wav     # Achievement unlocked
├── event.wav           # Market event triggered
├── victory.wav         # Win screen
└── gameover.wav        # Game over screen
```

4. Uncomment the `loadWav()` calls in `AudioSystem::LoadSounds()`.
