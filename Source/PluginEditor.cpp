//Copyright2023 Vishal Ahirwar. All rights reserved.
/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SEQAudioProcessorEditor::SEQAudioProcessorEditor (SEQAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
    peak_freq_slider_attachment(audioProcessor.audio_processor_value_tree_state,PEAK_FREQ,this->peak_freq_slider),
    peak_gain_slider_attachment(audioProcessor.audio_processor_value_tree_state,PEAK_GAIN,peak_gain_slider),
    peak_quality_slider_attachment(audioProcessor.audio_processor_value_tree_state,PEAK_QUALITY,peak_quality_slider),
    low_cut_freq_slider_attachment(audioProcessor.audio_processor_value_tree_state,LOW_CUT_FREQ,low_cut_freq_slider),
    high_cut_freq_slider_attachment(audioProcessor.audio_processor_value_tree_state,HIGH_CUT_FREQ,high_cut_freq_slider),
    low_cut_slope_slider_attachment(audioProcessor.audio_processor_value_tree_state,LOW_CUT_SLOPE,low_cut_slope_slider),
    high_cut_slope_slider_attachment(audioProcessor.audio_processor_value_tree_state,HIGH_CUT_SLOPE,high_cut_slope_slider)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    /*TODO : (0). Connect the sliders to parameters*/
    for (auto* component : get_components())
    {
        addAndMakeVisible(component);
    };

    setSize (800, 600);
}

SEQAudioProcessorEditor::~SEQAudioProcessorEditor()
{
}

//==============================================================================
void SEQAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::white);

    g.setColour (juce::Colours::black);
    g.setFont (21.0f);
    g.drawFittedText ("©2023 Vishal Ahirwar.", getLocalBounds(), juce::Justification::centredTop, 1);

}

void SEQAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto bound = getLocalBounds();
    auto response_area = bound.removeFromTop(bound.getHeight() * 0.33);
    auto low_cut_area = bound.removeFromLeft(bound.getWidth() * 0.33);
    auto high_cut_area = bound.removeFromRight(bound.getWidth() * 0.5);
    low_cut_freq_slider.setBounds(low_cut_area.removeFromTop(low_cut_area.getHeight()*0.5));
    high_cut_freq_slider.setBounds(high_cut_area.removeFromTop(high_cut_area.getHeight()*0.5));
    peak_freq_slider.setBounds(bound.removeFromTop(bound.getHeight() * 0.33));
    peak_gain_slider.setBounds(bound.removeFromTop(bound.getHeight() * 0.5));
    //low_cut_slope_slider.setBounds(low_cut_area.removeFromTop(low_cut_area.getHeight() * 0.33));
    //high_cut_slope_slider.setBounds(high_cut_area.removeFromTop(high_cut_area.getHeight() * 0.5));
    low_cut_slope_slider.setBounds(low_cut_area);
    high_cut_slope_slider.setBounds(high_cut_area);

    peak_quality_slider.setBounds(bound);

}

std::vector<CustomRotarySlider*> SEQAudioProcessorEditor::get_components() 
{
    return { &peak_freq_slider,&peak_gain_slider,&peak_quality_slider,&low_cut_freq_slider,&high_cut_freq_slider,&low_cut_slope_slider,&high_cut_slope_slider };
};

