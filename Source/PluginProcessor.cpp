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
        mSynth.addVoice(new juce::SamplerVoice);
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
        auto* read = buffer.getReadPointer(channel);
        auto* channelData = buffer.getWritePointer(channel);

    }
    
    //if the sample counter exceeds the length of the grain AND a voice is active, start next grain
    for (int i = 0; i < 3; i++) {
        if (mSynth.getVoice(i)->isKeyDown()) {
            sCounter += 480;
            if (sCounter > mLength) {
                startNextGrain();
            }
        }
    }

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
    //if no file is chosen, return
    else {
        return;
    }
    juce::BigInteger range;
    range.setRange(0, 128, true);

    //create initial grain
    createGrains();

}

void GranularAudioProcessor::createGrains() 
{
    mSynth.clearSounds();
    std::cout << "something wrong";
    juce::BigInteger range;
    range.setRange(0, 128, true);
    juce::SamplerSound* grain = new juce::SamplerSound("grain", *mFormatReader, range, 60, mAttack, mRelease, length);
    setOffset(grain);
    mSynth.addSound(grain);
    setADSRValues();
}

void GranularAudioProcessor::startNextGrain()
{
    juce::SynthesiserVoice* voice = mSynth.getVoice(voiceIndex);
    int note = voice->getCurrentlyPlayingNote();
    voice->stopNote(1, true);

    //generate a new grain
    mSynth.clearSounds();
    createGrains();

    //voice plays next available sound
    voice->startNote(note, 1, mSynth.getSound(0).get(), 0);

    //reset value of sample counter
    sCounter = 0;
}


void GranularAudioProcessor::setADSRValues() 
{
    //reads through loaded samplersounds, and sets their adsr values to the ones specified in GUI
    for (int i = 0; i < mSynth.getNumSounds(); i++) {

        if (auto sound = dynamic_cast<juce::SamplerSound*>(mSynth.getSound(i).get())) {
            sound->setEnvelopeParameters(adsrParameters);
        }
        mSynth.getSound(i);
    }

    //sets mLength value for every time adsr values are changed so voices play proper grain lengths
    mLength = (adsrParameters.attack + adsrParameters.decay) * 44100 + 481;
}

void GranularAudioProcessor::setOffset(juce::SamplerSound* grain)
{
    //generates the randomizes offset from start of audio sample
    auto* buffer = grain->getAudioData();
    int channels = buffer->getNumChannels();
    auto write = buffer->getArrayOfWritePointers();
    auto read = buffer->getArrayOfReadPointers();
    //sample rate 44.1khz
    double sampleRate = 44100;

    //produces a value between 0 and 1 to determine the number of offset samples
    double rDouble = juce::Random::getSystemRandom().nextDouble();
    double samples = nOffset * rDouble * sampleRate;
    int offset = (floor(samples));
    DBG("offset: " << offset);

    //if offset is 0, no change to sample audio
    if (offset == 0) {
        return;
    }
    for (int i = 0; i < channels; i++) {
        for (int j = 0; j < 300000; j++) {
            write[i][j] = read[i][j + offset];
        }
    }
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GranularAudioProcessor();
}
