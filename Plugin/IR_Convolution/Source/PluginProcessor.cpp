/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================

IR_ConvolutionAudioProcessor::IR_ConvolutionAudioProcessor()
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

IR_ConvolutionAudioProcessor::~IR_ConvolutionAudioProcessor()
{
}

//==============================================================================
const juce::String IR_ConvolutionAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool IR_ConvolutionAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool IR_ConvolutionAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool IR_ConvolutionAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double IR_ConvolutionAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int IR_ConvolutionAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int IR_ConvolutionAudioProcessor::getCurrentProgram()
{
    return 0;
}

void IR_ConvolutionAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String IR_ConvolutionAudioProcessor::getProgramName (int index)
{
    return {};
}

void IR_ConvolutionAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void IR_ConvolutionAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumOutputChannels();
    
    irLoader.reset();
    irLoader.prepare(spec);
    
}

void IR_ConvolutionAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool IR_ConvolutionAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void IR_ConvolutionAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    juce::dsp::AudioBlock<float> block {buffer};
    
    if (irLoader.getCurrentIRSize() > 0){
        irLoader.process(juce::dsp::ProcessContextReplacing<float>(block));
    }
    
    
}

//==============================================================================
bool IR_ConvolutionAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* IR_ConvolutionAudioProcessor::createEditor()
{
    return new IR_ConvolutionAudioProcessorEditor (*this);
}

//==============================================================================
void IR_ConvolutionAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classesjuce apvts
    // as intermediaries to make it easy to save and load complex data.
    
    juce::MemoryOutputStream mos(destData,true);
    apvts.state.writeToStream(mos);
}

void IR_ConvolutionAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if(tree.isValid())
    {
        apvts.replaceState(tree);
        updateRIR();
    }
    
}

double IR_ConvolutionAudioProcessor::getRealPosValue(double normalizedValue,double maxValue)
{
    // Berechnet den wahren Wert aus dem normalisierten
    double minValue = 0.01;
    return (normalizedValue * (maxValue - minValue)) + minValue;
}
std::string IR_ConvolutionAudioProcessor::getNameFromIndex(int index)
{
    // gibt Namen des gewählten Materials als String zurück
    int j = 1;
    for (const auto& pair : MATERIALS().abs)
    {
        if(index == j)
        {
            return pair.first;
        }
        j++;
    }
    return 0;
}

void IR_ConvolutionAudioProcessor::updateRIR(){
    // hier wird eine neue Impulsantwort erzeugt
    // Vektor mit allen Materialien
    std::vector<std::string> materials = {getNameFromIndex(apvts.getRawParameterValue("Front")->load()),
                                          getNameFromIndex(apvts.getRawParameterValue("Back")->load()),
                                          getNameFromIndex(apvts.getRawParameterValue("Left")->load()),
                                          getNameFromIndex(apvts.getRawParameterValue("Right")->load()),
                                          getNameFromIndex(apvts.getRawParameterValue("Top")->load()),
                                          getNameFromIndex(apvts.getRawParameterValue("Bottom")->load())};
    
    // Raumdimensionen
    float roomWidth = static_cast<double>(apvts.getRawParameterValue("RoomWidth")->load());
    float roomLength = static_cast<double>(apvts.getRawParameterValue("RoomLength")->load());
    float roomHeight = static_cast<double>(apvts.getRawParameterValue("RoomHeight")->load());
    
    // neuer Audiobuffer wird mit Impulsantwort befüllt
    juce::AudioBuffer<float> ir_buffer = RIRGenerator().generate(roomWidth,
                                                                 roomLength,
                                                                 roomHeight,
                                                                 getRealPosValue(apvts.getRawParameterValue("SoundX")->load(), roomWidth),
                                                                 getRealPosValue(apvts.getRawParameterValue("SoundY")->load(), roomLength),
                                                                 getRealPosValue(apvts.getRawParameterValue("SoundZ")->load(), roomHeight),
                                                                 getRealPosValue(apvts.getRawParameterValue("ReceiverX")->load(), roomWidth),
                                                                 getRealPosValue(apvts.getRawParameterValue("ReceiverY")->load(), roomLength),
                                                                 getRealPosValue(apvts.getRawParameterValue("ReceiverZ")->load(), roomHeight),
                                                                 materials,getSampleRate());
    // Convolution Engine lädt neue Impulsantwort
    irLoader.loadImpulseResponse(std::move(ir_buffer),
                                                getSampleRate(),
                                                juce::dsp::Convolution::Stereo::yes,
                                                juce::dsp::Convolution::Trim::no,
                                                juce::dsp::Convolution::Normalise::yes);
}



juce::AudioProcessorValueTreeState::ParameterLayout IR_ConvolutionAudioProcessor::createParameterLayout()
{
    // Hier wird das Parameter Layout festgelegt
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("RoomWidth",1),
                                                           "RoomWidth",
                                                           juce::NormalisableRange<float>(1.f,25.f,0.01f),3.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("RoomLength",1),
                                                           "RoomLength",
                                                           juce::NormalisableRange<float>(1.f,25.f,0.01f),3.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("RoomHeight",1),
                                                           "RoomHeight",
                                                           juce::NormalisableRange<float>(1.f,25.f,0.01f),3.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("SoundX",1),
                                                           "SoundX",
                                                           juce::NormalisableRange<float>(0.0f,1.f,0.001f),0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("SoundY",1),
                                                           "SoundY",
                                                           juce::NormalisableRange<float>(0.0f,1.f,0.001f),0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("SoundZ",1),
                                                           "SoundZ",
                                                           juce::NormalisableRange<float>(0.0f,1.f,0.001f),0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("ReceiverX",1),
                                                           "ReceiverX",
                                                           juce::NormalisableRange<float>(0.0f,1.f,0.001f),0.25f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("ReceiverY",1),
                                                           "ReceiverY",
                                                           juce::NormalisableRange<float>(0.0f,1.f,0.001f),0.25f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("ReceiverZ",1),
                                                           "ReceiverZ",
                                                           juce::NormalisableRange<float>(0.0f,1.f,0.001f),0.25f));
    
    layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("Front",1), "Front", 1, MATERIALS().abs.size(), 1));
    layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("Back",1), "Back", 1, MATERIALS().abs.size(), 1));
    layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("Left",1), "Left", 1, MATERIALS().abs.size(), 1));
    layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("Right",1), "Right", 1, MATERIALS().abs.size(), 1));
    layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("Top",1), "Top", 1, MATERIALS().abs.size(), 1));
    layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("Bottom",1), "Bottom", 1, MATERIALS().abs.size(), 1));
    
    return layout;
}
//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new IR_ConvolutionAudioProcessor();
}
