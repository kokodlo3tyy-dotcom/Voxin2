#pragma once
#include <vector>
#include <cmath>
#include <algorithm>

enum class ScaleType {
    Chromatic = 0,
    Major,
    Minor,
    Bayati,
    Rast,
    Sikah,
    Saba,
    Hijaz,
    Kurd
};

struct QuantizedPitchResult {
    int midiNote;
    float centsOffset;
};

struct ScaleDegree {
    int semitone;
    float microOffset;
};

class ScaleQuantizer {
public:
    ScaleQuantizer() : rootKey(2), currentScale(ScaleType::Bayati) {}

    void setRootKey(int rootNote) { rootKey = rootNote % 12; }
    void setScaleType(ScaleType type) { currentScale = type; }

    QuantizedPitchResult quantize(int rawMidiNote, float rawCentsOffset) {
        QuantizedPitchResult res;
        res.midiNote = rawMidiNote;
        res.centsOffset = rawCentsOffset;

        if (currentScale == ScaleType::Chromatic || rawMidiNote < 0)
            return res;

        int noteInOctave = rawMidiNote % 12;
        int octave = rawMidiNote / 12;
        const auto& degrees = getScaleDegrees(currentScale);

        float minDistance = 999.0f;
        ScaleDegree bestDegree = { 0, 0.0f };
        float inputPitch = (float)noteInOctave + (rawCentsOffset / 100.0f);

        for (const auto& deg : degrees) {
            float targetPitch = (float)((rootKey + deg.semitone) % 12) + (deg.microOffset / 100.0f);
            float dist = std::abs(inputPitch - targetPitch);
            if (dist > 6.0f) dist = 12.0f - dist;
            if (dist < minDistance) {
                minDistance = dist;
                bestDegree = deg;
            }
        }

        res.midiNote = (octave * 12) + ((rootKey + bestDegree.semitone) % 12);
        res.centsOffset = bestDegree.microOffset;
        return res;
    }

private:
    int rootKey;
    ScaleType currentScale;

    static const std::vector<ScaleDegree>& getScaleDegrees(ScaleType type) {
        static const std::vector<ScaleDegree> bayati = {
            {0, 0.0f}, {2, -50.0f}, {3, 0.0f}, {5, 0.0f}, {7, 0.0f}, {8, 0.0f}, {10, 0.0f}
        };
        static const std::vector<ScaleDegree> rast = {
            {0, 0.0f}, {2, 0.0f}, {4, -50.0f}, {5, 0.0f}, {7, 0.0f}, {9, 0.0f}, {11, -50.0f}
        };
        static const std::vector<ScaleDegree> sikah = {
            {0, -50.0f}, {1, 0.0f}, {3, 0.0f}, {5, 0.0f}, {7, -50.0f}, {8, 0.0f}, {10, 0.0f}
        };
        static const std::vector<ScaleDegree> saba = {
            {0, 0.0f}, {2, -50.0f}, {3, 0.0f}, {4, 0.0f}, {7, 0.0f}, {8, 0.0f}, {10, 0.0f}
        };
        static const std::vector<ScaleDegree> hijaz = {
            {0, 0.0f}, {1, 0.0f}, {4, 0.0f}, {5, 0.0f}, {7, 0.0f}, {8, 0.0f}, {10, 0.0f}
        };
        static const std::vector<ScaleDegree> kurd = {
            {0, 0.0f}, {1, 0.0f}, {3, 0.0f}, {5, 0.0f}, {7, 0.0f}, {8, 0.0f}, {10, 0.0f}
        };
        static const std::vector<ScaleDegree> major = {
            {0, 0.0f}, {2, 0.0f}, {4, 0.0f}, {5, 0.0f}, {7, 0.0f}, {9, 0.0f}, {11, 0.0f}
        };
        static const std::vector<ScaleDegree> minor = {
            {0, 0.0f}, {2, 0.0f}, {3, 0.0f}, {5, 0.0f}, {7, 0.0f}, {8, 0.0f}, {10, 0.0f}
        };

        switch (type) {
            case ScaleType::Bayati: return bayati;
            case ScaleType::Rast:   return rast;
            case ScaleType::Sikah:  return sikah;
            case ScaleType::Saba:   return saba;
            case ScaleType::Hijaz:  return hijaz;
            case ScaleType::Kurd:   return kurd;
            case ScaleType::Major:  return major;
            case ScaleType::Minor:  return minor;
            default:                return bayati;
        }
    }
};
