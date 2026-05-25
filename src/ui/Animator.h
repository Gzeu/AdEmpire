#pragma once
#include <unordered_map>
#include <string>
#include <cmath>

// Lerp-based animated float values
class Animator {
public:
    static Animator& Get() { static Animator a; return a; }

    // Register a value to animate toward a target
    void SetTarget(const std::string& key, float target) {
        if (values.find(key) == values.end())
            values[key] = target; // first frame: snap
        targets[key] = target;
    }

    // Call each frame to advance all animations
    void Tick(float dt) {
        constexpr float SPEED = 8.f;
        for (auto& [k, v] : values) {
            float t = targets.count(k) ? targets[k] : v;
            v += (t - v) * std::min(SPEED * dt, 1.f);
        }
    }

    float Get(const std::string& key, float fallback = 0.f) {
        auto it = values.find(key);
        return it != values.end() ? it->second : fallback;
    }

private:
    std::unordered_map<std::string, float> values;
    std::unordered_map<std::string, float> targets;
};
