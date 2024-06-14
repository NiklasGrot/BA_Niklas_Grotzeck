/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "IrSim.h"

//==============================================================================
/**
*/
class IR_ConvolutionAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                                   juce::Slider::Listener,
                                                   juce::ComboBox::Listener
                                                   //juce::AudioProcessorValueTreeState::Listener
                                        
{
public:
    IR_ConvolutionAudioProcessorEditor (IR_ConvolutionAudioProcessor&);
    ~IR_ConvolutionAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    std::vector<juce::Slider*> getSliders();
    std::vector<juce::Label*> getLabels();
    std::vector<juce::ComboBox*> getComboBoxes();
    std::vector<juce::Label*> getHeadlines();
    

    template<typename SliderType1, typename SliderType2>
    void updateSliderRange(SliderType1& modifiedSlider, SliderType2& targetSlider,std::string parameterId)
    {
        /*
         die Funktion erhält einen Raumgrößen-Slider einer Achse und den korrespondierenden Slider einer Position
         Anhand des Raumgrößen-Sliders ergibt sich ein maximal Wert den die Position erreichen kann. Der Position-Slider ist normalisiert auf 0 bis 1.
         Damit im Textfeld neben dem Slider, aber der wahre Wert erscheint wird diese Funktion verwendet
         */
        double maxValue = modifiedSlider.getValue() - 0.1;
        double minValue = 0.01;
        
        // Umrechnung von normalisiertem Wert zu Text
        targetSlider.textFromValueFunction = [minValue,maxValue](double value)
        {
            double newVal = (value * (maxValue - minValue)) + minValue;
            return juce::String(newVal);
        };
        // Umrechnung von Textwert zu normalisiertem Wert
        targetSlider.valueFromTextFunction = [minValue,maxValue](const juce::String &text)
        {
            double val = text.getDoubleValue();
            return (val - minValue) / (maxValue - minValue);
        };
        targetSlider.updateText();
  
    }
    
    void sliderValueChanged(juce::Slider* slider) override
    {
        // Funktion wird aufgerufen, wenn sich eine Slider verändert hat.
        if (slider == &roomWidthSlider || slider == &soundXSlider || slider == &receiverXSlider)
        {
            updateSliderRange(roomWidthSlider, soundXSlider,"RoomWidth");
            updateSliderRange(roomWidthSlider, receiverXSlider,"RoomWidth");
           
        }
        
        if (slider == &roomLengthSlider || slider == &soundYSlider || slider == &receiverYSlider)
        {
            updateSliderRange(roomLengthSlider, soundYSlider,"RoomLength");
            updateSliderRange(roomLengthSlider, receiverYSlider,"RoomLength");
            
        }
        if (slider == &roomHeightSlider || slider == &soundZSlider || slider == &receiverZSlider)
        {
            updateSliderRange(roomHeightSlider, soundZSlider,"RoomHeight");
            updateSliderRange(roomHeightSlider, receiverZSlider,"RoomHeight");
        }
        // Update der Impulsantwort
        audioProcessor.updateRIR();
        
    };
    
    void comboBoxChanged(juce::ComboBox* box) override
    {
        // Update der Impulsantwort
        audioProcessor.updateRIR();
    };
    
    /*void parameterChanged(const juce::String &parameterID,float newValue) override
    {
        audioProcessor.updateRIR();
    };*/
    /*
    {
    void updateRIR()
    {
        
        std::vector<std::string> materials = {frontBox.getText().toStdString(),
                                              backBox.getText().toStdString(),
                                              leftBox.getText().toStdString(),
                                              rightBox.getText().toStdString(),
                                              topBox.getText().toStdString(),
                                              bottomBox.getText().toStdString()};
        
        
        juce::AudioBuffer<float> ir_buffer = RIRGenerator().generate(audioProcessor.apvts.getRawParameterValue("RoomWidth")->load(),roomLengthSlider.getValue(),roomHeightSlider.getValue(),
                                                                     soundXSlider.getValue(),soundYSlider.getValue(),soundZSlider.getValue(),
                                                                     receiverXSlider.getValue(),receiverYSlider.getValue(),receiverZSlider.getValue(),
                                                                     materials);
        audioProcessor.irLoader.loadImpulseResponse(std::move(ir_buffer),
                                                    44100,
                                                    juce::dsp::Convolution::Stereo::yes,
                                                    juce::dsp::Convolution::Trim::no,
                                                    juce::dsp::Convolution::Normalise::yes);
        
    };
    */
    
    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    IR_ConvolutionAudioProcessor& audioProcessor;
    
    juce::Slider roomWidthSlider,
    roomLengthSlider,
    roomHeightSlider,
    soundXSlider,
    soundYSlider,
    soundZSlider,
    receiverXSlider,
    receiverYSlider,
    receiverZSlider;
    
    juce::Label roomWidthLabel,
    roomLengthLabel,
    roomHeightLabel,
    soundXLabel,
    soundYLabel,
    soundZLabel,
    receiverXLabel,
    receiverYLabel,
    receiverZLabel,
    frontLabel,
    backLabel,
    leftLabel,
    rightLabel,
    topLabel,
    bottomLabel,
    headlineRoom,
    headlineSound,
    headlineReceiver,
    headlineMaterials;
    
    juce::ComboBox frontBox,
    backBox,
    leftBox,
    rightBox,
    topBox,
    bottomBox;
    
    using APVTS = juce::AudioProcessorValueTreeState;
    using Slider_Attachement = APVTS::SliderAttachment;
    using ComboBox_Attachment = APVTS::ComboBoxAttachment;
    
    Slider_Attachement roomWidthSliderAttach,
    roomLengthSliderAttach,
    roomHeightSliderAttach,
    soundXSliderAttach,
    soundYSliderAttach,
    soundZSliderAttach,
    receiverXSliderAttach,
    receiverYSliderAttach,
    receiverZSliderAttach;

    ComboBox_Attachment frontAttach,
    backAttach,
    leftAttach,
    rightAttach,
    topAttach,
    bottomAttach;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (IR_ConvolutionAudioProcessorEditor)
};
