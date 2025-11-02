#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"

class PlayerGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener,
    public juce::Timer
{
public:
    PlayerGUI();
    ~PlayerGUI() override;

    void resized() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();
    void paint(juce::Graphics& g) override;
    void timerCallback() override;

private:
    PlayerAudio playerAudio;

    // GUI elements
    juce::TextButton loadButton{ "Load Files" };
    juce::TextButton playButton{ "Play" };
    juce::TextButton pauseButton{ "||" };
    juce::TextButton goToStartButton{ "<|" };
    juce::TextButton goToEndButton{ "|>" };
    juce::TextButton stopButton{ "Stop" };
    juce::TextButton loopButton{ "Loop" };
    juce::TextButton muteButton{ "Mute" };

    juce::Slider volumeSlider;
    juce::Slider speedSlider;

    bool isMuted = false;
    float lastVolume = 0.5f;

    std::unique_ptr<juce::FileChooser> fileChooser;

    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};