#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class VuMeterComponent : public juce::Component {
public:
    void setLevel(float newLevel) { level = newLevel; repaint(); }
    void paint(juce::Graphics& g) override {
        auto bounds = getLocalBounds().toFloat();
        g.setColour(juce::Colour(0xffe6d5ac));
        g.fillRoundedRectangle(bounds, 6.0f);
        g.setColour(juce::Colour(0xff221b14));
        g.drawRoundedRectangle(bounds, 6.0f, 2.0f);
        float angle = juce::jmap(std::clamp(level, 0.0f, 1.0f), -0.7f, 0.7f);
        juce::Point<float> pivot(bounds.getCentreX(), bounds.getBottom() + 10.0f);
        juce::Point<float> needleEnd(pivot.x + std::sin(angle) * (bounds.getHeight() * 0.95f),
                                     pivot.y - std::cos(angle) * (bounds.getHeight() * 0.95f));
        g.setColour(juce::Colour(0xff1a110a));
        g.drawLine(pivot.x, pivot.y, needleEnd.x, needleEnd.y, 1.8f);
    }
private:
    float level = 0.0f;
};
