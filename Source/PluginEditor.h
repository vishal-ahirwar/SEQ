//Copyright2023 Vishal Ahirwar. All rights reserved.
/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"



class CustomRotarySlider :public juce::Slider
{
public:
    CustomRotarySlider() :juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,juce::Slider::TextEntryBoxPosition::NoTextBox) {setColour(juce::Slider::ColourIds::rotarySliderFillColourId,juce::Colours::aqua);};
private:
protected:

};
//==============================================================================
/**
*/
class SEQAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    SEQAudioProcessorEditor (SEQAudioProcessor&);
    ~SEQAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SEQAudioProcessor& audioProcessor;

    CustomRotarySlider peak_freq_slider, peak_gain_slider, peak_quality_slider, low_cut_freq_slider, high_cut_freq_slider,low_cut_slope_slider,high_cut_slope_slider;

    std::vector<CustomRotarySlider*> get_components();

    juce::AudioProcessorValueTreeState::SliderAttachment peak_freq_slider_attachment, peak_gain_slider_attachment, peak_quality_slider_attachment, low_cut_freq_slider_attachment, high_cut_freq_slider_attachment, low_cut_slope_slider_attachment, high_cut_slope_slider_attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SEQAudioProcessorEditor)
};
