cmake_minimum_required(VERSION 3.22)
project(Voxin2 VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.4
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(Voxin2
    COMPANY_NAME "VoxinAudio"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    IS_MIDI_EFFECT FALSE
    PLUGIN_MANUFACTURER_CODE Voxn
    PLUGIN_CODE Vx02
    FORMATS VST3 Standalone
    PRODUCT_NAME "VOXIN 2"
)

target_sources(Voxin2 PRIVATE
    Source/PluginProcessor.cpp
    Source/GUI/PluginEditor.cpp
)

target_include_directories(Voxin2 PRIVATE
    Source
    Source/DSP
    Source/GUI
)

target_compile_definitions(Voxin2 PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(Voxin2 PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)
