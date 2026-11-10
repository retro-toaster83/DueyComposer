#pragma once
#include <JuceHeader.h>

class AppModel
{
public:
    AppModel() : valueTree("AudioAppRoot") 
    {
        // Example: Add a child to the tree so we have something to undo/redo
        valueTree.setProperty("demoProperty", "Initial Value", &undoManager);
    }

    juce::ValueTree& getValueTree() { return valueTree; }
    juce::UndoManager& getUndoManager() { return undoManager; }

    void createNew()
    {
        valueTree = juce::ValueTree("AudioAppRoot");
        undoManager.clearUndoHistory();
        currentFile = juce::File();
    }

    void save()
    {
        if (currentFile.existsAsFile())
            saveToFile(currentFile);
    }

    void saveAs(const juce::File& file)
    {
        currentFile = file;
        saveToFile(currentFile);
    }

    // Example function to demonstrate undoable tree changes
    void modifyDemoProperty()
    {
        undoManager.beginNewTransaction();
        auto newVal = "Modified " + juce::String(juce::Random::getSystemRandom().nextInt(100));
        valueTree.setProperty("demoProperty", newVal, &undoManager);
    }

private:
    void saveToFile(const juce::File& file)
    {
        // Convert ValueTree to XML and save
        if (auto xml = valueTree.createXml())
            xml->writeTo(file);
    }

    juce::ValueTree valueTree;
    juce::UndoManager undoManager;
    juce::File currentFile;
};