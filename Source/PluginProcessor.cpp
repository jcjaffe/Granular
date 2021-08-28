/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/
#pragma once
#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GranularAudioProcessor::GranularAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    mFormatManager.registerBasicFormats();
    for (int i = 0; i < mVoices; i++) {
        mSynth.addVoice(new juce::SamplerVoice());
    }
}

GranularAudioProcessor::~GranularAudioProcessor()
{
    mFormatReader = nullptr;
}

//==============================================================================
const juce::String GranularAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GranularAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool GranularAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool GranularAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double GranularAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GranularAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int GranularAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GranularAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String GranularAudioProcessor::getProgramName (int index)
{
    return {};
}

void GranularAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
//processes before note is played
void GranularAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    mSynth.setCurrentPlaybackSampleRate(sampleRate);

    //sets adsr values for synth voice
    setADSRValues();
}

void GranularAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GranularAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void GranularAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        //DBG("number of samples is " << buffer.getNumSamples());
        // ..do something to the data...
        auto* read = buffer.getReadPointer(channel);
        auto* channelData = buffer.getWritePointer(channel);
        //int arrSize = *(&read + 1) - read;
        int limit = 480;
        for (int i = 0; i < limit; i++) {
            //channelData[i] = 0;
        }

    }
    getADSRValues();
    // AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
    // LOOOOOOOOOOOK HEEEEEERRRRREEEEEE
    /*if (mSynth.getNumSounds() < 2) {
        this->createGrains();
    }*/
    mSynth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

//==============================================================================
bool GranularAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* GranularAudioProcessor::createEditor()
{
    return new GranularAudioProcessorEditor (*this);
}

//==============================================================================
void GranularAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void GranularAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

void GranularAudioProcessor::loadFile() 
{
    juce::FileChooser chooser{ "Please select a file" };

    if (chooser.browseForFileToOpen()) {
        auto file = chooser.getResult();
        mFormatReader = mFormatManager.createReaderFor(file);
    }
    juce::BigInteger range;
    range.setRange(0, 128, true);
    //mSynth.addSound(new juce::SamplerSound("Sample", *mFormatReader, range, 60, attack, release, length));
    //juce::SamplerSound* grain = new juce::SamplerSound("Sample", *mFormatReader, range, 60, attack, release, length);
    //mSynth.addSound(grain);

    
    juce::SamplerSound* total = new juce::SamplerSound("sample", *mFormatReader, range, 60, 0, 0, 20);
    auto* buffer = total->getAudioData();
    


    this->createGrains();
}

void GranularAudioProcessor::createGrains() 
{
    std::cout << "something wrong";
    juce::BigInteger range;
    range.setRange(0, 128, true);
    juce::SamplerSound* grain = new juce::SamplerSound("grain", *mFormatReader, range, 60, mAttack, mRelease, length);
    auto* buffer = grain->getAudioData();
    int channels = buffer->getNumChannels();
    auto write = buffer->getArrayOfWritePointers();
    auto read = buffer->getArrayOfReadPointers();
    //modify grains, adsr can be calculated
    for (int i = 0; i < channels; i++) {
        for (int j = 0; j < 100000; j++) {
            write[i][j] = read[i][j + 80000];
        }
    }
    //juce::ADSR::Parameters x(nAttack, nDecay, nSustain, nRelease);
    //adsr.setParameters(mParameters);
    mSynth.addSound(grain);
}


void GranularAudioProcessor::getADSRValues() 
{
    DBG ("Attack: " << nAttack << " Decay: " << nDecay << " Sustain: " << nSustain << " Release: " << nRelease);
}

void GranularAudioProcessor::setADSRValues() 
{
    for (int i = 0; i < mSynth.getNumSounds(); i++) {

        if (auto sound = dynamic_cast<juce::SamplerSound*>(mSynth.getSound(i).get())) {
            sound->setEnvelopeParameters(adsrParameters);
        }
        mSynth.getSound(i);
    }
}

void GranularAudioProcessor::setOffset() 
{

}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GranularAudioProcessor();
}
