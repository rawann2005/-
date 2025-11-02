//i love u


#include "MainComponent.h"

MainComponent::MainComponent()
{

    addAndMakeVisible(player1);

    setSize(1000, 500);
    setAudioChannels(0, 2);
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}
//صاشف

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    player1.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{

    player1.getNextAudioBlock(bufferToFill);

}

void MainComponent::releaseResources()
{
    player1.releaseResources();
}


void MainComponent::resized()
{
    player1.setBounds(20, 20, getWidth() - 40, 300);
}