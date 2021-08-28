/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GranularAudioProcessorEditor::GranularAudioProcessorEditor (GranularAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    mLoadButton.onClick = [&]() { audioProcessor.loadFile(); };
    addAndMakeVisible(mLoadButton);

    //rotary sliders to set ADSR values

    auto fSize = 12.0f;
    int tWidth = 40;

    mAttackSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    mAttackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, tWidth, 20);
    mAttackSlider.setRange(0.0f, 5.0f, 0.01f);
    mAttackSlider.addListener(this);
    addAndMakeVisible(mAttackSlider);
    maLabel.setFont(fSize);
    maLabel.setText("Attack", juce::NotificationType::dontSendNotification);
    maLabel.setJustificationType(juce::Justification::centredTop);
    maLabel.attachToComponent(&mAttackSlider, false);

    mDecaySlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    mDecaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, tWidth, 20);
    mDecaySlider.setRange(0.0f, 5.0f, 0.01f);
    mDecaySlider.addListener(this);
    addAndMakeVisible(mDecaySlider);
    mdLabel.setFont(fSize);
    mdLabel.setText("Decay", juce::NotificationType::dontSendNotification);
    mdLabel.setJustificationType(juce::Justification::centredTop);
    mdLabel.attachToComponent(&mDecaySlider, false);

    mSustainSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    mSustainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, tWidth, 20);
    mSustainSlider.setRange(0.0f, 1.0f, 0.01f);
    mSustainSlider.addListener(this);
    addAndMakeVisible(mSustainSlider);
    msLabel.setFont(fSize);
    msLabel.setText("Sustain", juce::NotificationType::dontSendNotification);
    msLabel.setJustificationType(juce::Justification::centredTop);
    msLabel.attachToComponent(&mSustainSlider, false);

    mReleaseSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    mReleaseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, tWidth, 20);
    mReleaseSlider.setRange(0.0f, 5.0f, 0.01f);
    mReleaseSlider.addListener(this);
    addAndMakeVisible(mReleaseSlider);
    mrLabel.setFont(fSize);
    mrLabel.setText("Release", juce::NotificationType::dontSendNotification);
    mrLabel.setJustificationType(juce::Justification::centredTop);
    mrLabel.attachToComponent(&mReleaseSlider, false);

    //slider to set randomized offset

    mRandomOffsetSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    mRandomOffsetSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, tWidth, 20);
    mRandomOffsetSlider.setRange(0.0f, 3.0f, 0.01f);
    mRandomOffsetSlider.addListener(this);
    addAndMakeVisible(mRandomOffsetSlider);
    mroLabel.setFont(fSize);
    mroLabel.setText("Randomized Offset", juce::NotificationType::dontSendNotification);
    mroLabel.setJustificationType(juce::Justification::centredTop);
    mroLabel.attachToComponent(&mRandomOffsetSlider, false);

    setSize (500, 400);
}

GranularAudioProcessorEditor::~GranularAudioProcessorEditor()
{
}

//==============================================================================
void GranularAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);


}

void GranularAudioProcessorEditor::resized()
{
    //button to load sample, set in middle of gui
    mLoadButton.setBounds(getWidth() / 2 - 50, getHeight() / 2 - 50, 100, 20);


    //set slider locations on gui
    const auto sX = 0.02f;
    const auto sY = 0.6f;
    const auto dialWidth = 0.19f;
    const auto dialHeight = 0.25f;

    mAttackSlider.setBoundsRelative(sX, sY, dialWidth, dialHeight);
    mDecaySlider.setBoundsRelative(sX + dialWidth, sY, dialWidth, dialHeight);
    mSustainSlider.setBoundsRelative(sX + (dialWidth * 2), sY, dialWidth, dialHeight);
    mReleaseSlider.setBoundsRelative(sX + (dialWidth * 3), sY, dialWidth, dialHeight);
    mRandomOffsetSlider.setBoundsRelative(sX + (dialWidth * 4), sY, dialWidth, dialHeight);
}

void GranularAudioProcessorEditor::setMidiInput()
{
    auto list = juce::MidiInput::getAvailableDevices();
}


//returns
void GranularAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &mAttackSlider) {
        audioProcessor.getADSR().attack = mAttackSlider.getValue();
    }
    if (slider == &mDecaySlider) {
        audioProcessor.getADSR().decay = mDecaySlider.getValue();
    }
    if (slider == &mSustainSlider) {
        audioProcessor.getADSR().sustain = mSustainSlider.getValue();
    }
    if (slider == &mReleaseSlider) {
        audioProcessor.getADSR().release = mReleaseSlider.getValue();
    }
    if (slider == &mRandomOffsetSlider) {
        audioProcessor.nOffset = mRandomOffsetSlider.getValue();
    }
    audioProcessor.setADSRValues();
    audioProcessor.setOffset();
}