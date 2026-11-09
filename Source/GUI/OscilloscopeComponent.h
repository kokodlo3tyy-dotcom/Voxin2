#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class Voxin2AudioProcessor;

class OscilloscopeComponent : public juce::Component, private juce::Timer {
public:
    OscilloscopeComponent(Voxin2AudioProcessor& p);
    void paint(juce::Graphics& g) override;
private:
    void timerCallback() override { repaint(); }
    Voxin2AudioProcessor& processor;
};
