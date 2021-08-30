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

    //prepares resources before receiving MIDI information
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;

    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    //called for continuously for every buffer, according to specified sample size
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

    //allows user to select audio file for synthesiser
    void loadFile();

    //creates sample grains as synthesiser voice objects
    void createGrains();

    //tells the voice
    void startNextGrain();

    //gets ADSR values to send to console
    void getADSRValues();

    //sets ADSR values
    void setADSRValues();

    //sets the randomized offset specified from the gui slider
    void setOffset(juce::SamplerSound* grain);
    void getLength();


    //made public to be accessed by the gui GranularAudioProcessorEditor class
    float nOffset = 0.0f;

    juce::ADSR::Parameters& getADSR() { return adsrParameters; }

private:
    //==============================================================================
    juce::Synthesiser mSynth;
    const int mVoices{ 3 };
    double mAttack = 0;
    double mRelease = 0;
    double length = 10;
    int voiceIndex = 0;
    int mLength;
    int sCounter = 0;

    juce::AudioFormatManager mFormatManager;
    juce::AudioFormatReader* mFormatReader{ nullptr };
    juce::MidiKeyboardState keyboardState;
    juce::AudioBuffer<float> sample;
    juce::ADSR::Parameters adsrParameters;

    juce::SamplerVoice mono;
    juce::SamplerVoice* pMono = &mono;
    juce::MidiMessage message;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GranularAudioProcessor)
};
