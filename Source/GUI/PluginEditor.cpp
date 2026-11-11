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

    const char* notes[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    for (int i = 0; i < 12; ++i) rootKeySelector.addItem(notes[i], i + 1);
    rootKeySelector.setSelectedId(3);
    rootKeySelector.onChange = [this]() {
        audioProcessor.setRootKey(rootKeySelector.getSelectedId() - 1);
    };
    addAndMakeVisible(rootKeySelector);

    maqamLabel.setText("MAQAM / SCALE SELECTOR", juce::dontSendNotification);
    maqamLabel.setFont(12.0f);
    maqamLabel.setColour(juce::Label::textColourId, juce::Colour(0xffffb703));
    addAndMakeVisible(maqamLabel);

    maqamSelector.addItem("Bayati (بياتي)", 1);
    maqamSelector.addItem("Rast (راست)", 2);
    maqamSelector.addItem("Sikah (سيكاه)", 3);
    maqamSelector.addItem("Saba (صبا)", 4);
    maqamSelector.addItem("Hijaz (حجاز)", 5);
    maqamSelector.addItem("Kurd (كرد)", 6);
    maqamSelector.addItem("Major (عجم / غربي)", 7);
    maqamSelector.addItem("Minor (نهاوند / غربي)", 8);
    maqamSelector.addItem("Chromatic (حر / كامل)", 9);
    maqamSelector.setSelectedId(1);
    maqamSelector.onChange = [this]() {
        int id = maqamSelector.getSelectedId();
        if (id == 1) audioProcessor.setScaleType(ScaleType::Bayati);
        else if (id == 2) audioProcessor.setScaleType(ScaleType::Rast);
        else if (id == 3) audioProcessor.setScaleType(ScaleType::Sikah);
        else if (id == 4) audioProcessor.setScaleType(ScaleType::Saba);
        else if (id == 5) audioProcessor.setScaleType(ScaleType::Hijaz);
        else if (id == 6) audioProcessor.setScaleType(ScaleType::Kurd);
        else if (id == 7) audioProcessor.setScaleType(ScaleType::Major);
        else if (id == 8) audioProcessor.setScaleType(ScaleType::Minor);
        else if (id == 9) audioProcessor.setScaleType(ScaleType::Chromatic);
    };
    addAndMakeVisible(maqamSelector);

    auto setupDrumBox = [](juce::ComboBox& box, juce::Label& lbl, const juce::String& text, int defaultNote) {
        lbl.setText(text, juce::dontSendNotification);
        lbl.setFont(12.0f);
        lbl.setColour(juce::Label::textColourId, juce::Colours::cyan);
        for (int i = 24; i <= 60; ++i) {
            box.addItem(juce::MidiMessage::getMidiNoteName(i, true, true, 3) + " (" + juce::String(i) + ")", i);
        }
        box.setSelectedId(defaultNote);
    };

    setupDrumBox(kickNoteSelector, kickLabel, "KICK (P/B)", audioProcessor.getKickNote());
    kickNoteSelector.onChange = [this]() { audioProcessor.setKickNote(kickNoteSelector.getSelectedId()); };
    addAndMakeVisible(kickLabel);
    addAndMakeVisible(kickNoteSelector);

    setupDrumBox(snareNoteSelector, snareLabel, "SNARE (K/T)", audioProcessor.getSnareNote());
    snareNoteSelector.onChange = [this]() { audioProcessor.setSnareNote(snareNoteSelector.getSelectedId()); };
    addAndMakeVisible(snareLabel);
    addAndMakeVisible(snareNoteSelector);

    setupDrumBox(hiHatNoteSelector, hiHatLabel, "HI-HAT (TS)", audioProcessor.getHiHatNote());
    hiHatNoteSelector.onChange = [this]() { audioProcessor.setHiHatNote(hiHatNoteSelector.getSelectedId()); };
    addAndMakeVisible(hiHatLabel);
    addAndMakeVisible(hiHatNoteSelector);

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
    g.drawRoundedRectangle(50, 310, 370, 180, 6.0f, 1.5f);
    g.drawRoundedRectangle(440, 310, 380, 180, 6.0f, 1.5f);

    g.setColour(juce::Colours::grey);
    g.setFont(13.0f);
    g.drawText("PERCUSSION ENGINE (BEATBOX TRIGGER)", 60, 315, 300, 20, juce::Justification::left);
    g.drawText("ORIENTAL MAQAM & MICROTONAL TUNER", 450, 315, 350, 20, juce::Justification::left);

    int note = audioProcessor.getLastDetectedMidi();
    if (note != -1) {
        g.setColour(juce::Colour(0xffffb703));
        g.drawText("TRANSMITTING MIDI: " + juce::MidiMessage::getMidiNoteName(note, true, true, 3),
                   getWidth() / 2 - 150, 270, 300, 25, juce::Justification::centred);
    }
}

void Voxin2AudioProcessorEditor::resized() {
    leftVuMeter.setBounds(55, 25, 75, 55);
    rightVuMeter.setBounds(135, 25, 75, 55);
    titleLabel.setBounds(getWidth() / 2 - 80, 25, 160, 35);
    micGainKnob.setBounds(getWidth() - 175, 20, 50, 50);
    thresholdKnob.setBounds(getWidth() - 110, 20, 50, 50);
    oscilloscope.setBounds(50, 95, getWidth() - 100, 160);

    kickLabel.setBounds(65, 350, 90, 20);
    kickNoteSelector.setBounds(65, 375, 100, 26);
    snareLabel.setBounds(185, 350, 90, 20);
    snareNoteSelector.setBounds(185, 375, 100, 26);
    hiHatLabel.setBounds(305, 350, 90, 20);
    hiHatNoteSelector.setBounds(305, 375, 100, 26);

    maqamLabel.setBounds(460, 350, 220, 20);
    rootKeySelector.setBounds(460, 375, 70, 28);
    maqamSelector.setBounds(540, 375, 200, 28);
}

void Voxin2AudioProcessorEditor::timerCallback() {
    float rms = audioProcessor.getCurrentRms();
    leftVuMeter.setLevel(rms * 2.2f);
    rightVuMeter.setLevel(rms * 2.0f);
    repaint();
}
