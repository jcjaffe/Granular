/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class GranularAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    GranularAudioProcessor();
    ~GranularAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void loadFile();
    void createGrains();
    void getADSRValues();
    void setADSRValues();
    void setOffset();


    //made public to be accessed by the gui class editor
    float nAttack = 0.0f;
    float nSustain = 0.0f;
    float nDecay = 0.0f;
    float nRelease = 0.0f;
    float nOffset = 0.0f;

    juce::ADSR::Parameters& getADSR() { return adsrParameters; }

private:
    //==============================================================================
    juce::Synthesiser mSynth;
    const int mVoices{ 3 };
    double mAttack = 0;
    double mRelease = 0;
    double length = 10;
    float** reader;
    float offsetSamples;
    



    juce::AudioFormatManager mFormatManager;
    juce::AudioFormatReader* mFormatReader{ nullptr };
    juce::MidiKeyboardState keyboardState;
    juce::AudioBuffer<float> sample;
    juce::ADSR::Parameters adsrParameters;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GranularAudioProcessor)
};
