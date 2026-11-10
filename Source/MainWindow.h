#pragma once
#include <JuceHeader.h>
#include "AppModel.h"
#include "MainComponent.h"

class MainWindow : public juce::DocumentWindow,
                   public juce::MenuBarModel,
                   public juce::ApplicationCommandTarget
{
public:
    enum CommandIDs
    {
        cmdNew = 1,
        cmdSave,
        cmdSaveAs,
        cmdClose,
        cmdSettings,
        cmdUndo,
        cmdRedo
    };

    MainWindow(juce::String name) : DocumentWindow(name, 
        juce::Desktop::getInstance().getDefaultLookAndFeel().findColour(ResizableWindow::backgroundColourId),
        DocumentWindow::allButtons)
    {
        setUsingNativeTitleBar(true);

        // 1. Initialize Audio & Plugins
        deviceManager.initialiseWithDefaultDevices(2, 2);
        juce::addDefaultFormatsToManager(formatManager);

        // 2. Setup Commands
        commandManager.registerAllCommandsForTarget(this);
        addKeyListener(commandManager.getKeyMappings());

        // 3. Setup UI
        mainComponent = std::make_unique<MainComponent>(appModel);
        setContentNonOwned(mainComponent.get(), true);
        
        // 4. Setup Menu
        setMenuBar(this);

        centreWithSize(getWidth(), getHeight());
        setVisible(true);
    }

    ~MainWindow() noexcept override
    {
        setMenuBar(nullptr);
    }

    void closeButtonPressed() override
    {
        juce::JUCEApplication::getInstance()->systemRequestedQuit();
    }

    // --- ApplicationCommandTarget Implementation ---
    juce::ApplicationCommandTarget* getNextCommandTarget() override { return nullptr; }

    void getAllCommands(juce::Array<juce::CommandID>& commands) override
    {
        commands.addArray({ cmdNew, cmdSave, cmdSaveAs, cmdClose, cmdSettings, cmdUndo, cmdRedo });
    }

    void getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result) override
    {
        switch (commandID)
        {
            case cmdNew:      result.setInfo("New", "Creates a new file", "File", 0); result.addDefaultKeypress('n', juce::ModifierKeys::commandModifier); break;
            case cmdSave:     result.setInfo("Save", "Saves current file", "File", 0); result.addDefaultKeypress('s', juce::ModifierKeys::commandModifier); break;
            case cmdSaveAs:   result.setInfo("Save As...", "Saves as new file", "File", 0); result.addDefaultKeypress('s', juce::ModifierKeys::commandModifier | juce::ModifierKeys::shiftModifier); break;
            case cmdClose:    result.setInfo("Close", "Closes app", "File", 0); result.addDefaultKeypress('w', juce::ModifierKeys::commandModifier); break;
            case cmdSettings: result.setInfo("Settings...", "Audio & Plugin Settings", "Options", 0); result.addDefaultKeypress(',', juce::ModifierKeys::commandModifier); break;
            
            case cmdUndo:     
                result.setInfo("Undo", "Undo last action", "Edit", 0); 
                result.addDefaultKeypress('z', juce::ModifierKeys::commandModifier); 
                result.setActive(appModel.getUndoManager().canUndo());
                break;
                
            case cmdRedo:     
                result.setInfo("Redo", "Redo last action", "Edit", 0); 
                result.addDefaultKeypress('z', juce::ModifierKeys::commandModifier | juce::ModifierKeys::shiftModifier); 
                result.setActive(appModel.getUndoManager().canRedo());
                break;
        }
    }

    bool perform(const InvocationInfo& info) override
    {
        switch (info.commandID)
        {
            case cmdNew:      appModel.createNew(); break;
            case cmdSave:     appModel.save(); break;
            case cmdSaveAs:   appModel.saveAs(juce::File::getSpecialLocation(juce::File::userDesktopDirectory).getChildFile("test.xml")); break;
            case cmdClose:    closeButtonPressed(); break;
            case cmdSettings: showSettingsDialog(); break;
            case cmdUndo:     appModel.getUndoManager().undo(); break;
            case cmdRedo:     appModel.getUndoManager().redo(); break;
            default: return false;
        }
        return true;
    }

    // --- MenuBarModel Implementation ---
    juce::StringArray getMenuBarNames() override { return { "File", "Edit", "Options" }; }
    
    juce::PopupMenu getMenuForIndex(int topLevelMenuIndex, const juce::String&) override
    {
        juce::PopupMenu menu;
        if (topLevelMenuIndex == 0) // File
        {
            menu.addCommandItem(&commandManager, cmdNew);
            menu.addCommandItem(&commandManager, cmdSave);
            menu.addCommandItem(&commandManager, cmdSaveAs);
            menu.addSeparator();
            menu.addCommandItem(&commandManager, cmdClose);
        }
        else if (topLevelMenuIndex == 1) // Edit
        {
            menu.addCommandItem(&commandManager, cmdUndo);
            menu.addCommandItem(&commandManager, cmdRedo);
        }
        else if (topLevelMenuIndex == 2) // Options
        {
            menu.addCommandItem(&commandManager, cmdSettings);
        }
        return menu;
    }
    
    void menuItemSelected(int, int) override {} // Not needed when using ApplicationCommandManager

private:
    void showSettingsDialog()
    {
        auto* tabbedComp = new juce::TabbedComponent(juce::TabbedButtonBar::TabsAtTop);
        
        // 1. Audio Settings Tab
        auto* audioSetup = new juce::AudioDeviceSelectorComponent(deviceManager, 0, 256, 0, 256, false, false, true, false);
        tabbedComp->addTab("Audio Devices", juce::Colours::darkgrey, audioSetup, true);

        // 2. Plugin List Tab
        auto* pluginSetup = new juce::PluginListComponent(formatManager, knownPluginList, juce::File(), nullptr);
        tabbedComp->addTab("Plugins", juce::Colours::darkgrey, pluginSetup, true);

        tabbedComp->setSize(600, 500);

        juce::DialogWindow::LaunchOptions options;
        options.content.setOwned(tabbedComp);
        options.dialogTitle = "Settings";
        options.componentToCentreAround = this;
        options.escapeKeyTriggersCloseButton = true;
        options.useNativeTitleBar = false;
        options.resizable = true;

        settingsWindow = options.launchAsync();
    }

    AppModel appModel;
    std::unique_ptr<MainComponent> mainComponent;
    
    juce::ApplicationCommandManager commandManager;
    
    juce::AudioDeviceManager deviceManager;
    juce::AudioPluginFormatManager formatManager;
    juce::KnownPluginList knownPluginList;

    juce::Component::SafePointer<juce::DialogWindow> settingsWindow;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
};