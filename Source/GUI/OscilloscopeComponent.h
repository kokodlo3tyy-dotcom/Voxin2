#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "../PluginProcessor.h"

class OscilloscopeComponent : public juce::Component, private juce::Timer {
public:
    OscilloscopeComponent(Voxin2AudioProcessor& p) : processor(p) { startTimerHz(60); }
    void paint(juce::Graphics& g) override {
        g.fillAll(juce::Colour(0xff090c10));
        g.setColour(juce::Colour(0xff12222a));
        for (int x = 0; x < getWidth(); x += 30) g.drawVerticalLine(x, 0.0f, (float)getHeight());
        for (int y = 0; y < getHeight(); y += 20) g.drawHorizontalLine(y, 0.0f, (float)getWidth());
        juce::Path wavePath;
        wavePath.startNewSubPath(0, getHeight() / 2.0f);
        for (int i = 0; i < getWidth(); ++i) {
            int sampleIdx = static_cast<int>((float)i / getWidth() * 512.0f);
            float sample = processor.getScopeSample(sampleIdx);
            wavePath.lineTo((float)i, (getHeight() / 2.0f) - (sample * (getHeight() * 0.45f)));
        }
        g.setColour(juce::Colour(0xff64ffda));
        g.strokePath(wavePath, juce::PathStrokeType(1.5f));
    }
private:
    void timerCallback() override { repaint(); }
    Voxin2AudioProcessor& processor;
};
