/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
// Initialisieren der Editor Klasse, sowie Verknüpfung zwischen den UI Komponenten und dem APVTS
IR_ConvolutionAudioProcessorEditor::IR_ConvolutionAudioProcessorEditor (IR_ConvolutionAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
    roomWidthSliderAttach(audioProcessor.apvts, "RoomWidth", roomWidthSlider),
    roomLengthSliderAttach(audioProcessor.apvts,"RoomLength",roomLengthSlider),
    roomHeightSliderAttach(audioProcessor.apvts, "RoomHeight", roomHeightSlider),
    soundXSliderAttach(audioProcessor.apvts,"SoundX",soundXSlider),
    soundYSliderAttach(audioProcessor.apvts,"SoundY",soundYSlider),
    soundZSliderAttach(audioProcessor.apvts,"SoundZ",soundZSlider),
    receiverXSliderAttach(audioProcessor.apvts,"ReceiverX",receiverXSlider),
    receiverYSliderAttach(audioProcessor.apvts,"ReceiverY",receiverYSlider),
    receiverZSliderAttach(audioProcessor.apvts,"ReceiverZ",receiverZSlider),
    frontAttach(audioProcessor.apvts,"Front", frontBox),
    backAttach(audioProcessor.apvts,"Back",backBox),
    leftAttach(audioProcessor.apvts,"Left",leftBox),
    rightAttach(audioProcessor.apvts,"Right",rightBox),
    topAttach(audioProcessor.apvts, "Top", topBox),
    bottomAttach(audioProcessor.apvts, "Bottom", bottomBox)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    
    
/*
   audioProcessor.apvts.addParameterListener("RoomWidth", this);
   audioProcessor.apvts.addParameterListener("RoomLength", this);
   audioProcessor.apvts.addParameterListener("RoomHeight", this);
   audioProcessor.apvts.addParameterListener("SoundX", this);
   audioProcessor.apvts.addParameterListener("SoundY", this);
   audioProcessor.apvts.addParameterListener("SoundZ", this);
   audioProcessor.apvts.addParameterListener("ReceiverX", this);
   audioProcessor.apvts.addParameterListener("ReceiverY", this);
   audioProcessor.apvts.addParameterListener("ReceiverZ", this);
   audioProcessor.apvts.addParameterListener("Front",this);
   audioProcessor.apvts.addParameterListener("Back",this);
   audioProcessor.apvts.addParameterListener("Left",this);
   audioProcessor.apvts.addParameterListener("Right",this);
   audioProcessor.apvts.addParameterListener("Top",this);
   audioProcessor.apvts.addParameterListener("Bottom",this);
    */
    frontLabel.setText("Front:", juce::dontSendNotification);
    backLabel.setText("Back:", juce::dontSendNotification);
    leftLabel.setText("Left:", juce::dontSendNotification);
    rightLabel.setText("Right:", juce::dontSendNotification);
    topLabel.setText("Top:", juce::dontSendNotification);
    bottomLabel.setText("Bottom:", juce::dontSendNotification);
    headlineRoom.setText("Roomsize [m]",juce::dontSendNotification);
    headlineSound.setText("Sound Position [m]",juce::dontSendNotification);
    headlineReceiver.setText("Receiver Position [m]",juce::dontSendNotification);
    headlineMaterials.setText("Wall Materials",juce::dontSendNotification);
    
    
    // Headlines
    for (auto* headline : getHeadlines())
    {
        juce::Font font(20.0f);
        font.setBold(true);
        headline->setFont(font);
        addAndMakeVisible(headline);
        
    }
    
    // Labels mit Komponenten verknüpfen
    frontLabel.attachToComponent(&frontBox, true);
    backLabel.attachToComponent(&backBox,true);
    leftLabel.attachToComponent(&leftBox,true);
    rightLabel.attachToComponent(&rightBox,true);
    topLabel.attachToComponent(&topBox, true);
    bottomLabel.attachToComponent(&bottomBox, true);
    
    // Befüllen der Dropdown-Menüs
    for (auto* box : getComboBoxes())
    {
        int index = 1;
        
        for (const auto& pair : MATERIALS().abs)
        {
            juce::String materialname = pair.first;
            box->addItem(materialname, index);
            index++;
        }
        //box->setSelectedId(audioProcessor.apvts.get"Front"));
        box->addListener(this);
        addAndMakeVisible(box);
         
    }
    
    
    //frontBox.addItemList(audioProcessor.apvts.getParameter("Front")->getAllValueStrings(), 1);
    frontBox.setSelectedId(audioProcessor.apvts.getRawParameterValue("Front")->load(), juce::dontSendNotification);
    backBox.setSelectedId(audioProcessor.apvts.getRawParameterValue("Back")->load(), juce::dontSendNotification);
    leftBox.setSelectedId(audioProcessor.apvts.getRawParameterValue("Left")->load(), juce::dontSendNotification);
    rightBox.setSelectedId(audioProcessor.apvts.getRawParameterValue("Right")->load(), juce::dontSendNotification);
    topBox.setSelectedId(audioProcessor.apvts.getRawParameterValue("Top")->load(), juce::dontSendNotification);
    bottomBox.setSelectedId(audioProcessor.apvts.getRawParameterValue("Bottom")->load(), juce::dontSendNotification);
    
    // Slider
    for (auto* slider : getSliders())
    {
        slider->setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
        slider->setTextBoxStyle(juce::Slider::TextBoxLeft, false, 50, 25);
        addAndMakeVisible(slider);
        slider->addListener(this);
    }
    // Labels
    for (auto* label : getLabels())
    {
        addAndMakeVisible(label);
    }
        
    soundXLabel.setText("X:",juce::dontSendNotification);
    soundYLabel.setText("Y:",juce::dontSendNotification);
    soundZLabel.setText("Z:",juce::dontSendNotification);
    soundXLabel.attachToComponent(&soundXSlider, true);
    soundYLabel.attachToComponent(&soundYSlider, true);
    soundZLabel.attachToComponent(&soundZSlider, true);
    
    receiverXLabel.setText("X:",juce::dontSendNotification);
    receiverYLabel.setText("Y:",juce::dontSendNotification);
    receiverZLabel.setText("Z:",juce::dontSendNotification);
    receiverXLabel.attachToComponent(&receiverXSlider, true);
    receiverYLabel.attachToComponent(&receiverYSlider, true);
    receiverZLabel.attachToComponent(&receiverZSlider, true);
    
    roomWidthLabel.setText("Width:", juce::dontSendNotification);
    roomLengthLabel.setText("Length:", juce::dontSendNotification);
    roomHeightLabel.setText("Height:", juce::dontSendNotification);
    roomWidthLabel.attachToComponent(&roomWidthSlider, true);
    roomLengthLabel.attachToComponent(&roomLengthSlider, true);
    roomHeightLabel.attachToComponent(&roomHeightSlider, true);
    
    // Initiales Update der Slider
    sliderValueChanged(&roomWidthSlider);
    sliderValueChanged(&roomLengthSlider);
    sliderValueChanged(&roomHeightSlider);
 
    // Fenstergröße
    setSize (700, 425);

}


IR_ConvolutionAudioProcessorEditor::~IR_ConvolutionAudioProcessorEditor()
{

}

//==============================================================================
void IR_ConvolutionAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    
 
}

void IR_ConvolutionAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    auto bounds = getLocalBounds().reduced(12, 12);
    auto leftSide = bounds.removeFromLeft(getWidth()*0.5);
    auto rightSide = bounds.removeFromRight(getWidth()*0.5);
    
    auto left2 = leftSide;
    auto left3 = leftSide;
    auto headlineHeight = 20;
    
    //ROOM SIZE
    auto l1 = leftSide.removeFromTop(leftSide.getHeight()*0.33);
    headlineRoom.setBounds(l1.removeFromTop(headlineHeight));
    auto slider_bounds = l1.removeFromBottom(l1.getHeight()-headlineHeight);
    for (int i=0; i<3; i++)
    {
        getSliders().at(i)->setBounds(slider_bounds.getX()+50,slider_bounds.getY()+ i*(slider_bounds.getHeight()*1/3)-10,slider_bounds.getWidth()-50,slider_bounds.getHeight()/3);
    }
    
    //SOUND POS
    auto l2 = left2.removeFromTop(left2.getHeight()*2/3);
    l2 = l2.removeFromBottom(l2.getHeight()*0.5);
    headlineSound.setBounds(l2.removeFromTop(headlineHeight));
    slider_bounds = l2.removeFromBottom(l2.getHeight()-headlineHeight);
    for (int i=0; i<3; i++)
    {
        getSliders().at(i+3)->setBounds(slider_bounds.getX()+50,slider_bounds.getY()+ i*(slider_bounds.getHeight()*1/3)-10,slider_bounds.getWidth()-50,slider_bounds.getHeight()/3);
    }
    
    //RECEIVER POS
    auto l3 = left3.removeFromBottom(left3.getHeight()/3);
    headlineReceiver.setBounds(l3.removeFromTop(headlineHeight));
    slider_bounds = l3.removeFromBottom(l3.getHeight()-headlineHeight);
    for (int i=0; i<3; i++)
    {
        getSliders().at(i+6)->setBounds(slider_bounds.getX()+50,slider_bounds.getY()+ i*(slider_bounds.getHeight()*1/3)-10,slider_bounds.getWidth()-50,slider_bounds.getHeight()/3);
    }
    
    //MATERIALS
    auto hd_area = rightSide.removeFromTop(headlineHeight);
    auto box_area = rightSide.removeFromBottom(rightSide.getHeight()-hd_area.getHeight());
    headlineMaterials.setBounds(hd_area);

    for (int i=0; i<6; i++)
    {
        getComboBoxes().at(i)->setBounds(box_area.getX()+50,box_area.getY()+i*(box_area.getHeight()*1/10)-10,box_area.getWidth()-50,(box_area.getHeight()*1/6)-35);
    }
}

std::vector<juce::Slider*> IR_ConvolutionAudioProcessorEditor::getSliders()
{
    return {
        &roomWidthSlider,
        &roomLengthSlider,
        &roomHeightSlider,
        &soundXSlider,
        &soundYSlider,
        &soundZSlider,
        &receiverXSlider,
        &receiverYSlider,
        &receiverZSlider
    };
}

std::vector<juce::Label*> IR_ConvolutionAudioProcessorEditor::getLabels()
{
    return {
        &roomWidthLabel,
        &roomLengthLabel,
        &roomHeightLabel,
        &soundXLabel,
        &soundYLabel,
        &soundZLabel,
        &receiverXLabel,
        &receiverYLabel,
        &receiverZLabel,
        &frontLabel,
        &backLabel,
        &leftLabel,
        &rightLabel,
        &topLabel,
        &bottomLabel
    };
}

std::vector<juce::ComboBox*> IR_ConvolutionAudioProcessorEditor::getComboBoxes()
{
    return {
        &frontBox,
        &backBox,
        &leftBox,
        &rightBox,
        &topBox,
        &bottomBox
    };
}

std::vector<juce::Label*> IR_ConvolutionAudioProcessorEditor::getHeadlines()
{
    return {
        &headlineRoom,
        &headlineSound,
        &headlineReceiver,
        &headlineMaterials
    };
}
