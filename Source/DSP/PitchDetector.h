#pragma once
#include <vector>
#include <cmath>
#include <algorithm>

class PitchDetector {
public:
    PitchDetector(int bufferSize = 1024, double sampleRate = 44100.0)
        : bufferSize(bufferSize), sampleRate(sampleRate), threshold(0.15f) {
        yinBuffer.resize(bufferSize / 2, 0.0f);
    }
    void setSampleRate(double newSampleRate) { sampleRate = newSampleRate; }
    float getPitch(const float* audioData, int numSamples) {
        if (numSamples < bufferSize) return -1.0f;
        for (int tau = 0; tau < bufferSize / 2; ++tau) {
            yinBuffer[tau] = 0.0f;
            for (int i = 0; i < bufferSize / 2; ++i) {
                float delta = audioData[i] - audioData[i + tau];
                yinBuffer[tau] += delta * delta;
            }
        }
        yinBuffer[0] = 1.0f;
        float runningSum = 0.0f;
        for (int tau = 1; tau < bufferSize / 2; ++tau) {
            runningSum += yinBuffer[tau];
            yinBuffer[tau] = (yinBuffer[tau] * tau) / (runningSum + 1e-6f);
        }
        int tauFound = -1;
        for (int tau = 2; tau < bufferSize / 2; ++tau) {
            if (yinBuffer[tau] < threshold) {
                while (tau + 1 < bufferSize / 2 && yinBuffer[tau + 1] < yinBuffer[tau]) tau++;
                tauFound = tau;
                break;
            }
        }
        if (tauFound == -1) return -1.0f;
        float s0 = yinBuffer[std::max(0, tauFound - 1)];
        float s1 = yinBuffer[tauFound];
        float s2 = yinBuffer[std::min((int)yinBuffer.size() - 1, tauFound + 1)];
        float delta = (s2 - s0) / (2.0f * (2.0f * s1 - s2 - s0) + 1e-6f);
        return (float)sampleRate / ((float)tauFound + delta);
    }
private:
    int bufferSize;
    double sampleRate;
    float threshold;
    std::vector<float> yinBuffer;
};

struct MidiPitchResult {
    int midiNote;
    float centsOffset;
    bool isValid;
};

class MidiConverter {
public:
    static MidiPitchResult frequencyToMidi(float frequency) {
        MidiPitchResult result = { -1, 0.0f, false };
        if (frequency < 40.0f || frequency > 2000.0f) return result;
        float noteNumber = 69.0f + 12.0f * std::log2(frequency / 440.0f);
        int roundedNote = static_cast<int>(std::round(noteNumber));
        result.midiNote = std::clamp(roundedNote, 0, 127);
        result.centsOffset = (noteNumber - (float)roundedNote) * 100.0f;
        result.isValid = true;
        return result;
    }
};
