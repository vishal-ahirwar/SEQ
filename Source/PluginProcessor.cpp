//Copyright2023 Vishal Ahirwar. All rights reserved.
/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


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
    auto chain_settings = get_chain_settings(this->audio_processor_value_tree_state);
    auto peak_coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate,chain_settings.peak_freq,chain_settings.peak_quality,juce::Decibels::decibelsToGain(chain_settings.peak_gain_in_decibels));
    *left_chain.get<(int)chain_positions::peak>().coefficients = *peak_coefficients;
    *right_chain.get<(int)chain_positions::peak>().coefficients = *peak_coefficients;

    auto cut_coefficients = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(chain_settings.low_cut_freq,sampleRate,2*((int)chain_settings.low_cut_slope+1));
    auto& left_low_cut = left_chain.get<(int)chain_positions::low_cut>();

    /*TODO : (1). high cut parameters coefficient needs to be implemented*/
    /*TODO : (2). code refactoring needs to be done!*/
    /*TODO : (3). GUI Implementation needs to be done!*/
    /*TODO : And that's it for this project*/

    left_low_cut.setBypassed<0>(true);
    left_low_cut.setBypassed<1>(true);
    left_low_cut.setBypassed<2>(true);
    left_low_cut.setBypassed<3>(true);
    switch (chain_settings.low_cut_slope)
    {
    case slope::slope_12:
    {
        *left_low_cut.get<0>().coefficients = *cut_coefficients[0];
        left_low_cut.setBypassed<0>(false);
        break;
    };
    case slope::slope_24:
    {
        *left_low_cut.get<0>().coefficients = *cut_coefficients[0];
        left_low_cut.setBypassed<0>(false);
        *left_low_cut.get<1>().coefficients = *cut_coefficients[1];
        left_low_cut.setBypassed<1>(false);
        break;
    };
    case slope::slope_36:
    {
        *left_low_cut.get<0>().coefficients = *cut_coefficients[0];
        left_low_cut.setBypassed<0>(false);
        *left_low_cut.get<1>().coefficients = *cut_coefficients[1];
        left_low_cut.setBypassed<1>(false);
        *left_low_cut.get<2>().coefficients = *cut_coefficients[2];
        left_low_cut.setBypassed<2>(false);
        break;

    };
    case slope::slope_48:
    {
        *left_low_cut.get<0>().coefficients = *cut_coefficients[0];
        left_low_cut.setBypassed<0>(false);
        *left_low_cut.get<1>().coefficients = *cut_coefficients[1];
        left_low_cut.setBypassed<1>(false);
        *left_low_cut.get<2>().coefficients = *cut_coefficients[2];
        left_low_cut.setBypassed<2>(false);
        *left_low_cut.get<3>().coefficients = *cut_coefficients[3];
        left_low_cut.setBypassed<3>(false);
        break;
    };
    }

    auto& right_low_cut = right_chain.get<(int)chain_positions::low_cut>();

    right_low_cut.setBypassed<0>(true);
    right_low_cut.setBypassed<1>(true);
    right_low_cut.setBypassed<2>(true);
    right_low_cut.setBypassed<3>(true);
    switch (chain_settings.low_cut_slope)
    {
    case slope::slope_12:
    {
        *right_low_cut.get<0>().coefficients = *cut_coefficients[0];
        right_low_cut.setBypassed<0>(false);
        break;
    };
    case slope::slope_24:
    {
        *right_low_cut.get<0>().coefficients = *cut_coefficients[0];
        right_low_cut.setBypassed<0>(false);
        *right_low_cut.get<1>().coefficients = *cut_coefficients[1];
        right_low_cut.setBypassed<1>(false);
        break;
    };
    case slope::slope_36:
    {
        *right_low_cut.get<0>().coefficients = *cut_coefficients[0];
        right_low_cut.setBypassed<0>(false);
        *right_low_cut.get<1>().coefficients = *cut_coefficients[1];
        right_low_cut.setBypassed<1>(false);
        *right_low_cut.get<2>().coefficients = *cut_coefficients[2];
        right_low_cut.setBypassed<2>(false);
        break;

    };
    case slope::slope_48:
    {
        *right_low_cut.get<0>().coefficients = *cut_coefficients[0];
        right_low_cut.setBypassed<0>(false);
        *right_low_cut.get<1>().coefficients = *cut_coefficients[1];
        right_low_cut.setBypassed<1>(false);
        *right_low_cut.get<2>().coefficients = *cut_coefficients[2];
        right_low_cut.setBypassed<2>(false);
        *right_low_cut.get<3>().coefficients = *cut_coefficients[3];
        right_low_cut.setBypassed<3>(false);
        break;
    };
    }
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
struct ChainSettings get_chain_settings(juce::AudioProcessorValueTreeState&_audio_processor_value_tree_state)
{
    ChainSettings setting;
    setting.low_cut_freq = _audio_processor_value_tree_state.getRawParameterValue(LOW_CUT_FREQ)->load();
    setting.high_cut_freq= _audio_processor_value_tree_state.getRawParameterValue(HIGH_CUT_FREQ)->load();
    //continue fill the struct
    //...
    setting.peak_freq= _audio_processor_value_tree_state.getRawParameterValue(PEAK_FREQ)->load();
    setting.peak_gain_in_decibels= _audio_processor_value_tree_state.getRawParameterValue(PEAK_GAIN)->load();
    setting.peak_quality= _audio_processor_value_tree_state.getRawParameterValue(PEAK_QUALITY)->load();
    setting.low_cut_slope= static_cast<slope>(_audio_processor_value_tree_state.getRawParameterValue(LOW_CUT_SLOPE)->load());
    setting.high_cut_slope=static_cast<slope>(_audio_processor_value_tree_state.getRawParameterValue(HIGH_CUT_SLOPE)->load());

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

    auto chain_settings = get_chain_settings(this->audio_processor_value_tree_state);
    auto peak_coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(), chain_settings.peak_freq, chain_settings.peak_quality, juce::Decibels::decibelsToGain(chain_settings.peak_gain_in_decibels));
    *left_chain.get<(int)chain_positions::peak>().coefficients = *peak_coefficients;
    *right_chain.get<(int)chain_positions::peak>().coefficients = *peak_coefficients;


    auto cut_coefficients = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(chain_settings.low_cut_freq, getSampleRate(), 2 * ((int)chain_settings.low_cut_slope + 1));
    auto& left_low_cut = left_chain.get<(int)chain_positions::low_cut>();

    left_low_cut.setBypassed<0>(true);
    left_low_cut.setBypassed<1>(true);
    left_low_cut.setBypassed<2>(true);
    left_low_cut.setBypassed<3>(true);
    switch (chain_settings.low_cut_slope)
    {
    case slope::slope_12:
    {
        *left_low_cut.get<0>().coefficients = *cut_coefficients[0];
        left_low_cut.setBypassed<0>(false);
        break;
    };
    case slope::slope_24:
    {
        *left_low_cut.get<0>().coefficients = *cut_coefficients[0];
        left_low_cut.setBypassed<0>(false);
        *left_low_cut.get<1>().coefficients = *cut_coefficients[1];
        left_low_cut.setBypassed<1>(false);
        break;
    };
    case slope::slope_36:
    {
        *left_low_cut.get<0>().coefficients = *cut_coefficients[0];
        left_low_cut.setBypassed<0>(false);
        *left_low_cut.get<1>().coefficients = *cut_coefficients[1];
        left_low_cut.setBypassed<1>(false);
        *left_low_cut.get<2>().coefficients = *cut_coefficients[2];
        left_low_cut.setBypassed<2>(false);
        break;

    };
    case slope::slope_48:
    {
        *left_low_cut.get<0>().coefficients = *cut_coefficients[0];
        left_low_cut.setBypassed<0>(false);
        *left_low_cut.get<1>().coefficients = *cut_coefficients[1];
        left_low_cut.setBypassed<1>(false);
        *left_low_cut.get<2>().coefficients = *cut_coefficients[2];
        left_low_cut.setBypassed<2>(false);
        *left_low_cut.get<3>().coefficients = *cut_coefficients[3];
        left_low_cut.setBypassed<3>(false);
        break;
    };
    }

    auto& right_low_cut = right_chain.get<(int)chain_positions::low_cut>();

    right_low_cut.setBypassed<0>(true);
    right_low_cut.setBypassed<1>(true);
    right_low_cut.setBypassed<2>(true);
    right_low_cut.setBypassed<3>(true);
    switch (chain_settings.low_cut_slope)
    {
    case slope::slope_12:
    {
        *right_low_cut.get<0>().coefficients = *cut_coefficients[0];
        right_low_cut.setBypassed<0>(false);
        break;
    };
    case slope::slope_24:
    {
        *right_low_cut.get<0>().coefficients = *cut_coefficients[0];
        right_low_cut.setBypassed<0>(false);
        *right_low_cut.get<1>().coefficients = *cut_coefficients[1];
        right_low_cut.setBypassed<1>(false);
        break;
    };
    case slope::slope_36:
    {
        *right_low_cut.get<0>().coefficients = *cut_coefficients[0];
        right_low_cut.setBypassed<0>(false);
        *right_low_cut.get<1>().coefficients = *cut_coefficients[1];
        right_low_cut.setBypassed<1>(false);
        *right_low_cut.get<2>().coefficients = *cut_coefficients[2];
        right_low_cut.setBypassed<2>(false);
        break;

    };
    case slope::slope_48:
    {
        *right_low_cut.get<0>().coefficients = *cut_coefficients[0];
        right_low_cut.setBypassed<0>(false);
        *right_low_cut.get<1>().coefficients = *cut_coefficients[1];
        right_low_cut.setBypassed<1>(false);
        *right_low_cut.get<2>().coefficients = *cut_coefficients[2];
        right_low_cut.setBypassed<2>(false);
        *right_low_cut.get<3>().coefficients = *cut_coefficients[3];
        right_low_cut.setBypassed<3>(false);
        break;
    };
    }

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
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SEQAudioProcessor::createEditor()
{
    return new SEQAudioProcessorEditor (*this);
}

//==============================================================================
void SEQAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::MemoryOutputStream memory_output_stream(destData, true);
    audio_processor_value_tree_state.state.writeToStream(memory_output_stream);

}

void SEQAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    auto temp_tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if (temp_tree.isValid())
    {
        audio_processor_value_tree_state.replaceState(temp_tree);
        /*TODO : Do refactoring and call the update_filter here!*/
    }
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