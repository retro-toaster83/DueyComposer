#pragma once
#include <JuceHeader.h>
#include "AppModel.h"

class MainComponent : public juce::Component, public juce::ValueTree::Listener
{
public:
    MainComponent(AppModel& m) : model(m)
    {
        model.getValueTree().addListener(this);
        
        infoLabel.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(infoLabel);
        updateLabel();

        // Button to test undo/redo
        testButton.setButtonText("Change ValueTree Property");
        testButton.onClick = [this] { model.modifyDemoProperty(); };
        addAndMakeVisible(testButton);

        setSize(800, 600);
    }

    ~MainComponent() override { model.getValueTree().removeListener(this); }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    }

    void resized() override
    {
        auto bounds = getLocalBounds().reduced(20);
        infoLabel.setBounds(bounds.removeFromTop(50));
        testButton.setBounds(bounds.removeFromTop(40).withSizeKeepingCentre(200, 40));
    }

    // ValueTree::Listener callbacks
    void valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier&) override { updateLabel(); }
    void valueTreeChildAdded(juce::ValueTree&, juce::ValueTree&) override {}
    void valueTreeChildRemoved(juce::ValueTree&, juce::ValueTree&, int) override {}
    void valueTreeChildOrderChanged(juce::ValueTree&, int, int) override {}
    void valueTreeParentChanged(juce::ValueTree&) override {}

private:
    void updateLabel()
    {
        juce::String val = model.getValueTree().getProperty("demoProperty").toString();
        infoLabel.setText("Current Value: " + val, juce::dontSendNotification);
    }

    AppModel& model;
    juce::Label infoLabel;
    juce::TextButton testButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};