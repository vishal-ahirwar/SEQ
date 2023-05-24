//Copyright2023 Vishal Ahirwar. All rights reserved.
/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
struct ChainSettings
{
    float peak_freq{ 0 }, peak_gain_in_decibels{ 0 }, peak_quality{ 1.f };
    float low_cut_freq{ 0 }, high_cut_freq{ 0 }, low_cut_slope{}, high_cut_slope{};
};

struct ChainSettings get_chain_settings(juce::AudioProcessorValueTreeState&);

//==============================================================================
/**
*/
class SEQAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    SEQAudioProcessor();
    ~SEQAudioProcessor() override;

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
    //
    //Juce requires All  apvt parameter at compile time
    static juce::AudioProcessorValueTreeState::ParameterLayout create_parameter_layout();//return
    juce::AudioProcessorValueTreeState audio_processor_value_tree_state{ *this, nullptr, "Parameters", SEQAudioProcessor::create_parameter_layout() };
private:

    using filter = juce::dsp::IIR::Filter<float>;
    using cut_filter = juce::dsp::ProcessorChain<filter, filter, filter, filter>;
    using mono_chain = juce::dsp::ProcessorChain<cut_filter, filter, cut_filter>;
    mono_chain left_chain, right_chain;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SEQAudioProcessor)
};
