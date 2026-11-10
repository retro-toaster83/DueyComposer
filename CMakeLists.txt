
cmake_minimum_required(VERSION 3.22)
project(DueyComposer VERSION 1.0.0)

# Add JUCE (Make sure you have cloned JUCE into a subdirectory named 'JUCE')
add_subdirectory(JUCE)

juce_add_gui_app(DueyComposer
    PRODUCT_NAME "Audio App"
    VERSION 1.0.0
)

target_sources(DueyComposer PRIVATE
    Source/Main.cpp
    Source/MainWindow.h
    Source/MainComponent.h
    Source/AppModel.h
)

# Link necessary JUCE modules
target_link_libraries(DueyComposer PRIVATE
    juce::juce_core
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_processors
    juce::juce_audio_utils
)

juce_generate_juce_header(DueyComposer)
