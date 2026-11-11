#pragma once
#include <vector>
#include <cmath>
#include <algorithm>

enum class DrumType { None = 0, Kick, Snare, HiHat };

struct DrumTriggerResult {
    DrumType type;
    int velocity;
};

class PercussionEngine {
public:
    PercussionEngine() : cooldownSamples(0), sampleRate(44100.0) {}
    void prepare(double sr) { sampleRate = sr; }

    DrumTriggerResult analyzeBlock(const float* buffer, int numSamples, float sensitivityThreshold) {
        DrumTriggerResult res = { DrumType::None, 0 };
        if (cooldownSamples > 0) { cooldownSamples -= numSamples; return res; }

        float maxAmp = 0.0f;
        int zeroCrossings = 0;
        float prevSample = 0.0f;

        for (int i = 0; i < numSamples; ++i) {
            float s = buffer[i];
            float absS = std::abs(s);
            if (absS > maxAmp) maxAmp = absS;
            if ((s >= 0.0f && prevSample < 0.0f) || (s < 0.0f && prevSample >= 0.0f)) zeroCrossings++;
            prevSample = s;
        }

        if (maxAmp < sensitivityThreshold) return res;

        int velocity = std::clamp(static_cast<int>(maxAmp * 127.0f * 1.5f), 40, 127);
        float zcrRate = (float)zeroCrossings / (float)numSamples;

        if (zcrRate < 0.08f) {
            res = { DrumType::Kick, velocity };
            cooldownSamples = static_cast<int>(sampleRate * 0.12);
        } else if (zcrRate < 0.25f) {
            res = { DrumType::Snare, velocity };
            cooldownSamples = static_cast<int>(sampleRate * 0.10);
        } else {
            res = { DrumType::HiHat, velocity };
            cooldownSamples = static_cast<int>(sampleRate * 0.08);
        }
        return res;
    }
private:
    int cooldownSamples;
    double sampleRate;
};
