#pragma once
#include <vector>
#include <cmath>
#include <algorithm>

enum class ScaleType { Chromatic = 0, Major, Minor };

class ScaleQuantizer {
public:
    ScaleQuantizer() : rootKey(0), currentScale(ScaleType::Minor) {}
    void setRootKey(int rootNote) { rootKey = rootNote % 12; }
    void setScaleType(ScaleType type) { currentScale = type; }
    int quantize(int midiNote) {
        if (currentScale == ScaleType::Chromatic || midiNote < 0) return midiNote;
        int noteInOctave = midiNote % 12;
        int octave = midiNote / 12;
        const auto& intervals = (currentScale == ScaleType::Major) ? std::vector<int>{0, 2, 4, 5, 7, 9, 11} : std::vector<int>{0, 2, 3, 5, 7, 8, 10};
        int closestNote = intervals[0];
        int minDistance = 999;
        for (int interval : intervals) {
            int scalePitch = (rootKey + interval) % 12;
            int dist = std::abs(noteInOctave - scalePitch);
            if (dist > 6) dist = 12 - dist;
            if (dist < minDistance) { minDistance = dist; closestNote = scalePitch; }
        }
        return (octave * 12) + closestNote;
    }
private:
    int rootKey;
    ScaleType currentScale;
};
