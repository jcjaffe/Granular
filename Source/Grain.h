#pragma once
#include <JuceHeader.h>
class Grain {
public:
    void setADSR();
    void process(juce::AudioSampleBuffer& currentBlock, juce::AudioSampleBuffer& fileBuffer, int numChannels, int blockNumSamples, int fileNumSamples, long long int time);
private:
    int startTime;
};