/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class GranularAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Slider::Listener
{
public:
    GranularAudioProcessorEditor (GranularAudioProcessor&);
    ~GranularAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void setMidiInput();
    void sliderValueChanged(juce::Slider* slider) override;

private:
    juce::TextButton mLoadButton{ "Load" };
    juce::ComboBox midiInput;
    juce::Label midiInputListLabel;
    int lastInputIndex = 0;
    GranularAudioProcessor& audioProcessor;
    juce::Slider mAttackSlider, mDecaySlider, mSustainSlider, mReleaseSlider, mRandomOffsetSlider;
    juce::Label maLabel, mdLabel, msLabel, mrLabel, mroLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GranularAudioProcessorEditor)
};
