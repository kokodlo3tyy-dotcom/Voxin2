#pragma once
#include <cmath>
#include <algorithm>

struct FormantCoordinates { float x; float y; int cc1Value; };

class FormantTracker {
public:
    FormantCoordinates process(const float* buffer, int numSamples) {
        float centroid = 0.0f, totalEnergy = 1e-6f;
        for (int i = 0; i < numSamples; ++i) {
            float energy = buffer[i] * buffer[i];
            centroid += (float)i * energy;
            totalEnergy += energy;
        }
        float norm = (centroid / totalEnergy) / (float)numSamples;
        FormantCoordinates coords;
        coords.x = std::clamp((norm * 2.0f) - 1.0f, -1.0f, 1.0f);
        coords.y = std::clamp(std::sin(norm * 3.14159f), -1.0f, 1.0f);
        coords.cc1Value = static_cast<int>((coords.x + 1.0f) * 0.5f * 127.0f);
        return coords;
    }
};
