#pragma once
#include <string>

// OpenAL Audio System
// To activate: install libopenal-dev and uncomment #define AUDIO_ENABLED
// sudo apt install libopenal-dev
// Add to CMakeLists.txt: find_package(OpenAL REQUIRED) and target_link_libraries(... OpenAL::OpenAL)

// #define AUDIO_ENABLED

#ifdef AUDIO_ENABLED
#include <AL/al.h>
#include <AL/alc.h>
#endif

enum class SoundEvent {
    CampaignLaunched,
    ClientWon,
    ClientLost,
    MonthAdvanced,
    AchievementUnlocked,
    EventTriggered,
    Victory,
    GameOver,
    ButtonClick,
    Notification
};

class AudioSystem {
public:
    static AudioSystem& Get() { static AudioSystem a; return a; }

    void Init() {
#ifdef AUDIO_ENABLED
        device  = alcOpenDevice(nullptr);
        context = alcCreateContext(device, nullptr);
        alcMakeContextCurrent(context);
        LoadSounds();
#endif
    }

    void Play(SoundEvent ev) {
#ifdef AUDIO_ENABLED
        // Map event to buffer & play
        auto it = sounds.find(ev);
        if (it == sounds.end()) return;
        ALuint src;
        alGenSources(1, &src);
        alSourcei(src, AL_BUFFER, it->second);
        alSourcePlay(src);
        // Cleanup handled in Tick()
        activeSources.push_back(src);
#else
        (void)ev;
#endif
    }

    void Tick() {
#ifdef AUDIO_ENABLED
        // Remove finished sources
        activeSources.erase(
            std::remove_if(activeSources.begin(), activeSources.end(),
                [](ALuint s){
                    ALint state;
                    alGetSourcei(s, AL_SOURCE_STATE, &state);
                    if (state != AL_PLAYING) { alDeleteSources(1, &s); return true; }
                    return false;
                }), activeSources.end());
#endif
    }

    void Shutdown() {
#ifdef AUDIO_ENABLED
        for (auto& [k,v] : sounds) alDeleteBuffers(1, &v);
        alcDestroyContext(context);
        alcCloseDevice(device);
#endif
    }

    float masterVolume = 0.8f;
    bool  enabled = true;

private:
    void LoadSounds() {
#ifdef AUDIO_ENABLED
        // Load .wav files from assets/audio/
        // loadWav(SoundEvent::ButtonClick, "assets/audio/click.wav");
        // loadWav(SoundEvent::CampaignLaunched, "assets/audio/launch.wav");
        // etc.
#endif
    }

#ifdef AUDIO_ENABLED
    ALCdevice*  device  = nullptr;
    ALCcontext* context = nullptr;
    std::unordered_map<SoundEvent, ALuint> sounds;
    std::vector<ALuint> activeSources;
#endif
};
