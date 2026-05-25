#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <cstring>

// ── OpenAL Audio System ──────────────────────────────────────────────────────
// v0.8: Fully implemented with procedural WAV generation (no external assets)
// Requires: sudo apt install libopenal-dev
// CMake:    find_package(OpenAL REQUIRED) + target_link_libraries(... OpenAL::OpenAL)
//
// Enable by defining AUDIO_ENABLED (done in CMakeLists.txt via option)
// ─────────────────────────────────────────────────────────────────────────────

// #define AUDIO_ENABLED  // Controlled by CMake option -DAUDIO=ON

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
        if (!device) return;
        context = alcCreateContext(device, nullptr);
        alcMakeContextCurrent(context);
        GenerateProceduralSounds();
        m_ready = true;
#endif
    }

    void Play(SoundEvent ev) {
#ifdef AUDIO_ENABLED
        if (!m_ready || !enabled) return;
        auto it = sounds.find(ev);
        if (it == sounds.end()) return;
        ALuint src;
        alGenSources(1, &src);
        alSourcei(src,  AL_BUFFER, it->second);
        alSourcef(src,  AL_GAIN,   masterVolume);
        alSourcePlay(src);
        activeSources.push_back(src);
#else
        (void)ev;
#endif
    }

    void Tick() {
#ifdef AUDIO_ENABLED
        activeSources.erase(
            std::remove_if(activeSources.begin(), activeSources.end(),
                [](ALuint s) {
                    ALint state;
                    alGetSourcei(s, AL_SOURCE_STATE, &state);
                    if (state != AL_PLAYING) { alDeleteSources(1, &s); return true; }
                    return false;
                }),
            activeSources.end());
#endif
    }

    void Shutdown() {
#ifdef AUDIO_ENABLED
        for (auto& [k, v] : sounds) alDeleteBuffers(1, &v);
        for (auto s : activeSources)  alDeleteSources(1, &s);
        alcMakeContextCurrent(nullptr);
        if (context) alcDestroyContext(context);
        if (device)  alcCloseDevice(device);
#endif
    }

    float masterVolume = 0.75f;
    bool  enabled      = true;

private:
    // ── Procedural WAV generators ────────────────────────────────────────────
    // All sounds are synthesized at runtime — zero external asset files needed.

    struct WavSamples { std::vector<int16_t> pcm; int sampleRate = 44100; };

    static WavSamples MakeTone(float freq, float dur, float attack = 0.01f,
                               float decay = 0.05f, float gain = 0.6f,
                               bool descend = false) {
        int sr   = 44100;
        int len  = (int)(dur * sr);
        WavSamples w; w.sampleRate = sr; w.pcm.resize(len);
        for (int i = 0; i < len; ++i) {
            float t  = (float)i / sr;
            float f  = descend ? freq * (1.0f - 0.4f * t / dur) : freq;
            float env = 1.0f;
            if (t < attack) env = t / attack;
            else if (t > dur - decay) env = (dur - t) / decay;
            float sample = gain * env * std::sin(2.0f * 3.14159f * f * t);
            w.pcm[i] = (int16_t)(sample * 32767.0f);
        }
        return w;
    }

    static WavSamples MakeChord(std::vector<float> freqs, float dur,
                                float gain = 0.5f) {
        int sr  = 44100;
        int len = (int)(dur * sr);
        WavSamples w; w.sampleRate = sr; w.pcm.resize(len, 0);
        float perNote = gain / (float)freqs.size();
        for (float f : freqs) {
            for (int i = 0; i < len; ++i) {
                float t   = (float)i / sr;
                float env = 1.0f;
                float atk = 0.01f, dcy = 0.08f;
                if (t < atk) env = t / atk;
                else if (t > dur - dcy) env = (dur - t) / dcy;
                float val = perNote * env * std::sin(2.0f * 3.14159f * f * t);
                int32_t s = (int32_t)w.pcm[i] + (int32_t)(val * 32767.0f);
                w.pcm[i]  = (int16_t)std::max(-32767, std::min(32767, s));
            }
        }
        return w;
    }

    static WavSamples MakeNoise(float dur, float gain = 0.3f) {
        int sr  = 44100;
        int len = (int)(dur * sr);
        WavSamples w; w.sampleRate = sr; w.pcm.resize(len);
        srand(42);
        for (int i = 0; i < len; ++i) {
            float t   = (float)i / sr;
            float env = 1.0f;
            if (t > dur - 0.05f) env = (dur - t) / 0.05f;
            float n   = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
            w.pcm[i]  = (int16_t)(n * gain * env * 32767.0f);
        }
        return w;
    }

#ifdef AUDIO_ENABLED
    ALuint UploadBuffer(const WavSamples& w) {
        ALuint buf;
        alGenBuffers(1, &buf);
        alBufferData(buf, AL_FORMAT_MONO16,
                     w.pcm.data(),
                     (ALsizei)(w.pcm.size() * sizeof(int16_t)),
                     w.sampleRate);
        return buf;
    }

    void GenerateProceduralSounds() {
        // ButtonClick    — short high tick
        sounds[SoundEvent::ButtonClick]        = UploadBuffer(MakeTone(1200.0f, 0.04f, 0.005f, 0.03f, 0.4f));
        // Notification   — gentle double-ping
        {
            auto a = MakeTone(880.0f, 0.12f); auto b = MakeTone(1100.0f, 0.12f);
            WavSamples n; n.pcm.resize(a.pcm.size() + b.pcm.size() + 2205);
            std::copy(a.pcm.begin(), a.pcm.end(), n.pcm.begin());
            std::copy(b.pcm.begin(), b.pcm.end(), n.pcm.begin() + a.pcm.size() + 2205);
            sounds[SoundEvent::Notification]   = UploadBuffer(n);
        }
        // MonthAdvanced  — short rising sweep
        sounds[SoundEvent::MonthAdvanced]      = UploadBuffer(MakeTone(440.0f, 0.15f, 0.01f, 0.06f, 0.5f));
        // CampaignLaunched — punchy mid chord
        sounds[SoundEvent::CampaignLaunched]   = UploadBuffer(MakeChord({523.25f, 659.25f, 783.99f}, 0.25f, 0.55f));
        // ClientWon       — bright ascending arpeggio
        sounds[SoundEvent::ClientWon]          = UploadBuffer(MakeChord({523.25f, 659.25f, 783.99f, 1046.5f}, 0.35f, 0.5f));
        // ClientLost      — low descending tone
        sounds[SoundEvent::ClientLost]         = UploadBuffer(MakeTone(220.0f, 0.4f, 0.01f, 0.15f, 0.55f, true));
        // EventTriggered  — dramatic low hit
        sounds[SoundEvent::EventTriggered]     = UploadBuffer(MakeChord({110.0f, 138.6f}, 0.3f, 0.45f));
        // AchievementUnlocked — fanfare arpeggio
        sounds[SoundEvent::AchievementUnlocked]= UploadBuffer(MakeChord({523.25f, 659.25f, 783.99f, 1046.5f, 1318.5f}, 0.55f, 0.45f));
        // GameOver        — descending minor chord with noise
        {
            auto ch = MakeChord({220.0f, 261.6f, 311.1f}, 0.6f, 0.4f);
            auto ns = MakeNoise(0.25f, 0.2f);
            for (size_t i = 0; i < ns.pcm.size() && i < ch.pcm.size(); ++i) {
                int32_t s = (int32_t)ch.pcm[i] + ns.pcm[i];
                ch.pcm[i] = (int16_t)std::max(-32767, std::min(32767, s));
            }
            sounds[SoundEvent::GameOver]       = UploadBuffer(ch);
        }
        // Victory         — triumphant major fanfare
        sounds[SoundEvent::Victory]            = UploadBuffer(MakeChord({523.25f, 659.25f, 783.99f, 1046.5f, 1318.5f, 1568.0f}, 1.0f, 0.45f));
    }

    bool                                    m_ready  = false;
    ALCdevice*                              device   = nullptr;
    ALCcontext*                             context  = nullptr;
    std::unordered_map<SoundEvent, ALuint>  sounds;
    std::vector<ALuint>                     activeSources;
#endif
};

// Convenience macro (no-op when audio disabled)
#ifdef AUDIO_ENABLED
#  define SOUND(ev) AudioSystem::Get().Play(SoundEvent::ev)
#else
#  define SOUND(ev) do {} while(0)
#endif
