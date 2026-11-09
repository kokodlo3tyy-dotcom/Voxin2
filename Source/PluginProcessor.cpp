#include "PluginProcessor.h"
#include "GUI/PluginEditor.h"

Voxin2AudioProcessor::Voxin2AudioProcessor()
    : AudioProcessor (BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                      .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "PARAMS", {
          std::make_unique<juce::AudioParameterFloat>("micGain", "Mic Gain", 0.0f, 2.0f, 1.0f),
          std::make_unique<juce::AudioParameterFloat>("threshold", "Threshold", 0.01f, 0.5f, 0.04f)
      }) {}

Voxin2AudioProcessor::~Voxin2AudioProcessor() {}
void Voxin2AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock) {
    pitchDetector.setSampleRate(sampleRate);
    percussionEngine.prepare(sampleRate);
}
void Voxin2AudioProcessor::releaseResources() {}
void Voxin2AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
    juce::ScopedNoDenormals noDenormals;
    int numSamples = buffer.getNumSamples();
    float gain = *parameters.getRawParameterValue("micGain");
    buffer.applyGain(gain);
    const float* channelData = buffer.getReadPointer(0);
    float rms = buffer.getRMSLevel(0, 0, numSamples);
    currentRms.store(rms);
    for (int i = 0; i < numSamples; ++i) {
        scopeBuffer[scopeWriteIndex] = channelData[i];
        scopeWriteIndex = (scopeWriteIndex + 1) % 512;
    }
    float gateThreshold = *parameters.getRawParameterValue("threshold");
    auto drum = percussionEngine.analyzeBlock(channelData, numSamples, gateThreshold * 1.5f);
    if (drum.type != DrumType::None) {
        midiMessages.addEvent(juce::MidiMessage::noteOn(10, drum.midiNote, (juce::uint8)drum.velocity), 0);
        midiMessages.addEvent(juce::MidiMessage::noteOff(10, drum.midiNote), 100);
    }
    if (rms > gateThreshold) {
        float pitchHz = pitchDetector.getPitch(channelData, numSamples);
        auto midiRes = MidiConverter::frequencyToMidi(pitchHz);
        if (midiRes.isValid) {
            int quantizedNote = scaleQuantizer.quantize(midiRes.midiNote);
            if (quantizedNote != lastMidiNote) {
                if (lastMidiNote != -1) midiMessages.addEvent(juce::MidiMessage::noteOff(1, lastMidiNote), 0);
                int vel = std::clamp(static_cast<int>(rms * 250.0f), 30, 127);
                midiMessages.addEvent(juce::MidiMessage::noteOn(1, quantizedNote, (juce::uint8)vel), 0);
                lastMidiNote = quantizedNote;
            }
            int bend = 8192 + static_cast<int>((midiRes.centsOffset / 100.0f) * 4096.0f);
            midiMessages.addEvent(juce::MidiMessage::pitchWheel(1, std::clamp(bend, 0, 16383)), 0);
        }
        auto vowels = formantTracker.process(channelData, numSamples);
        midiMessages.addEvent(juce::MidiMessage::controllerEvent(1, 1, vowels.cc1Value), 0);
    } else {
        if (lastMidiNote != -1) {
            midiMessages.addEvent(juce::MidiMessage::noteOff(1, lastMidiNote), 0);
            lastMidiNote = -1;
        }
    }
}
juce::AudioProcessorEditor* Voxin2AudioProcessor::createEditor() { return new Voxin2AudioProcessorEditor (*this); }
void Voxin2AudioProcessor::getStateInformation (juce::MemoryBlock& destData) {
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}
void Voxin2AudioProcessor::setStateInformation (const void* data, int sizeInBytes) {
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr && xmlState->hasTagName (parameters.state.getType()))
        parameters.replaceState (juce::ValueTree::fromXml (*xmlState));
}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new Voxin2AudioProcessor(); }
