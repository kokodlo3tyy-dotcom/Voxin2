#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "../PluginProcessor.h"
#include "OscilloscopeComponent.h"
#include "VuMeterComponent.h"

class Voxin2AudioProcessorEditor : public juce::AudioProcessorEditor, private juce::Timer {
public:
    Voxin2AudioProcessorEditor (Voxin2AudioProcessor&);
    ~Voxin2AudioProcessorEditor() override;
    void paint (juce::Graphics&) override;
    void resized() override;
private:
    void timerCallback() override;
    Voxin2AudioProcessor& audioProcessor;
    OscilloscopeComponent oscilloscope;
    VuMeterComponent leftVuMeter;
    VuMeterComponent rightVuMeter;

    juce::Slider micGainKnob;
    juce::Slider thresholdKnob;
    juce::Label titleLabel;

    juce::ComboBox rootKeySelector;
    juce::ComboBox maqamSelector;
    juce::Label maqamLabel;

    juce::ComboBox kickNoteSelector;
    juce::ComboBox snareNoteSelector;
    juce::ComboBox hiHatNoteSelector;
    juce::Label kickLabel, snareLabel, hiHatLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Voxin2AudioProcessorEditor)
};
