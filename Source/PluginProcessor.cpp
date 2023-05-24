//Copyright2023 Vishal Ahirwar. All rights reserved.
/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#define LOW_CUT_FREQ "LowCut Freq"
#define HIGH_CUT_FREQ "HighCut Freq"
#define PEAK_FREQ "Peak Freq"
#define PEAK_QUALITY "Peak Quality"
#define PEAK_GAIN "Peak Gain"
#define LOW_CUT_SLOPE "LowCut Slope"
#define HIGH_CUT_SLOPE "HighCut Slope"

//==============================================================================
SEQAudioProcessor::SEQAudioProcessor()
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
}

SEQAudioProcessor::~SEQAudioProcessor()
{
}

//==============================================================================
const juce::String SEQAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SEQAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SEQAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SEQAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SEQAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SEQAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SEQAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SEQAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SEQAudioProcessor::getProgramName (int index)
{
    return {};
}

void SEQAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SEQAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 1;
    spec.sampleRate = sampleRate;
    left_chain.prepare(spec);
    right_chain.prepare(spec);
};


void SEQAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SEQAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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
struct ChainSettings get_chain_settings(juce::AudioProcessorValueTreeState&_audio_processor_value_stree_state)
{
    ChainSettings setting;
    setting.low_cut_freq = _audio_processor_value_stree_state.getRawParameterValue(LOW_CUT_FREQ)->load();
    setting.high_cut_freq= _audio_processor_value_stree_state.getRawParameterValue(HIGH_CUT_FREQ)->load();
    //continue fill the struct
    //...

    return setting;
};

void SEQAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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

    juce::dsp::AudioBlock<float>block(buffer);
    auto left_block = block.getSingleChannelBlock(0);
    auto right_block = block.getSingleChannelBlock(1);
    juce::dsp::ProcessContextReplacing<float>left_context(left_block);
    juce::dsp::ProcessContextReplacing<float>right_context(right_block);
    this->left_chain.process(left_context);
    this->right_chain.process(right_context);
}

//==============================================================================
bool SEQAudioProcessor::hasEditor() const
{
    return false; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SEQAudioProcessor::createEditor()
{
    //return new SEQAudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor(this);
}

//==============================================================================
void SEQAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SEQAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SEQAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout SEQAudioProcessor::create_parameter_layout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        LOW_CUT_FREQ,
        LOW_CUT_FREQ,
        juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 1.f), 20.f
        )
    );

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        HIGH_CUT_FREQ,
        HIGH_CUT_FREQ,
        juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 1.f), 20000.f
        )
    );
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        PEAK_FREQ,
        PEAK_FREQ,
        juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 1.f), 750.f
        )
    );
    layout.add(std::make_unique<juce::AudioParameterFloat>(PEAK_GAIN,
        PEAK_GAIN,
        juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f), 0.f
        )
    );
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        PEAK_QUALITY,
        PEAK_QUALITY,
        juce::NormalisableRange<float>(-0.1f, 10.f, 0.05f, 1.f), 1.f
        )
    );
    juce::StringArray string_array;
    for (uint16_t i = 0; i < 4; ++i)
    {
        juce::String string;
        string << (12 + i * 12);
        string << "db/Oct";
        string_array.add(string);
    };
    layout.add(std::make_unique<juce::AudioParameterChoice>(LOW_CUT_SLOPE,LOW_CUT_SLOPE , string_array, 0));
    layout.add(std::make_unique<juce::AudioParameterChoice>(HIGH_CUT_SLOPE, HIGH_CUT_SLOPE, string_array, 0));
    return layout;
}