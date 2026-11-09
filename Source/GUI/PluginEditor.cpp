#include "PluginEditor.h"
#include "../PluginProcessor.h"

OscilloscopeComponent::OscilloscopeComponent(Voxin2AudioProcessor& p) : processor(p) { startTimerHz(60); }
void OscilloscopeComponent::paint(juce::Graphics& g) {
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

Voxin2AudioProcessorEditor::Voxin2AudioProcessorEditor (Voxin2AudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), oscilloscope(p) {
    setSize (880, 520);
    addAndMakeVisible (oscilloscope);
    addAndMakeVisible (leftVuMeter);
    addAndMakeVisible (rightVuMeter);
    micGainKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    micGainKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(micGainKnob);
    thresholdKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    thresholdKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(thresholdKnob);
    titleLabel.setText("VOXIN 2", juce::dontSendNotification);
    titleLabel.setFont(juce::FontOptions(26.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, juce::Colour(0xffc5d1d9));
    titleLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(titleLabel);
    startTimerHz(30);
}

Voxin2AudioProcessorEditor::~Voxin2AudioProcessorEditor() {}

void Voxin2AudioProcessorEditor::paint (juce::Graphics& g) {
    g.fillAll (juce::Colour(0xff121417));
    g.setColour(juce::Colour(0xff22262c));
    g.fillRect(0, 0, 36, getHeight());
    g.fillRect(getWidth() - 36, 0, 36, getHeight());
    g.setColour(juce::Colour(0xff090a0c));
    for (int y = 30; y < getHeight(); y += 110) {
        g.fillEllipse(10, (float)y, 16, 16);
        g.fillEllipse(getWidth() - 26, (float)y, 16, 16);
    }
    g.setColour(juce::Colour(0xff1c2128));
    g.drawRoundedRectangle(42, 10, getWidth() - 84, getHeight() - 20, 8.0f, 2.0f);
    g.drawRoundedRectangle(50, 320, 370, 170, 6.0f, 1.5f);
    g.drawRoundedRectangle(440, 320, 380, 170, 6.0f, 1.5f);
    g.setColour(juce::Colours::grey);
    g.setFont(13.0f);
    g.drawText("PERCUSSION ENGINE", 60, 325, 200, 20, juce::Justification::left);
    g.drawText("FORMANT & MODULATION", 450, 325, 200, 20, juce::Justification::left);
    int note = audioProcessor.getLastDetectedMidi();
    if (note != -1) {
        g.setColour(juce::Colour(0xffffb703));
        g.drawText("DETECTED NOTE: " + juce::MidiMessage::getMidiNoteName(note, true, true, 3),
                   getWidth() / 2 - 100, 280, 200, 25, juce::Justification::centred);
    }
}

void Voxin2AudioProcessorEditor::resized() {
    leftVuMeter.setBounds(55, 25, 75, 55);
    rightVuMeter.setBounds(135, 25, 75, 55);
    titleLabel.setBounds(getWidth() / 2 - 80, 25, 160, 35);
    micGainKnob.setBounds(getWidth() - 175, 20, 50, 50);
    thresholdKnob.setBounds(getWidth() - 110, 20, 50, 50);
    oscilloscope.setBounds(50, 95, getWidth() - 100, 160);
}

void Voxin2AudioProcessorEditor::timerCallback() {
    float rms = audioProcessor.getCurrentRms();
    leftVuMeter.setLevel(rms * 2.2f);
    rightVuMeter.setLevel(rms * 2.0f);
    repaint();
}
