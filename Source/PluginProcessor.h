#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "DSP/PitchDetector.h"
#include "DSP/ScaleQuantizer.h"
#include "DSP/PercussionEngine.h"
#include "DSP/FormantTracker.h"

class Voxin2AudioProcessor : public juce::AudioProcessor {
public:
    Voxin2AudioProcessor();
    ~Voxin2AudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "VOXIN 2"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return true; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    float getCurrentRms() const { return currentRms.load(); }
    float getScopeSample(int index) const { return scopeBuffer[index % 512]; }
    int getLastDetectedMidi() const { return lastMidiNote; }

    void setRootKey(int root) { scaleQuantizer.setRootKey(root); }
    void setScaleType(ScaleType type) { scaleQuantizer.setScaleType(type); }

    void setKickNote(int note) { kickMidiNote = note; }
    void setSnareNote(int note) { snareMidiNote = note; }
    void setHiHatNote(int note) { hiHatMidiNote = note; }
    int getKickNote() const { return kickMidiNote; }
    int getSnareNote() const { return snareMidiNote; }
    int getHiHatNote() const { return hiHatMidiNote; }

private:
    PitchDetector pitchDetector;
    ScaleQuantizer scaleQuantizer;
    PercussionEngine percussionEngine;
    FormantTracker formantTracker;

    std::atomic<float> currentRms { 0.0f };
    float scopeBuffer[512] = { 0.0f };
    int scopeWriteIndex = 0;
    int lastMidiNote = -1;

    int kickMidiNote = 36;
    int snareMidiNote = 38;
    int hiHatMidiNote = 42;

    juce::AudioProcessorValueTreeState parameters;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Voxin2AudioProcessor)
};
