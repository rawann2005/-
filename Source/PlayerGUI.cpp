#include "PlayerGUI.h"

PlayerGUI::PlayerGUI()
{
    // إعداد الأزرار
    for (auto* btn : { &loadButton, &playButton, &pauseButton, &goToStartButton,
                      &goToEndButton, &stopButton, &loopButton, &muteButton })
    {
        addAndMakeVisible(btn);
        btn->addListener(this);
        btn->setColour(juce::TextButton::buttonColourId, juce::Colour(0xff26c6da));
        btn->setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    }

    // إعداد منزلق الصوت
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    volumeSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 50, 20);
    addAndMakeVisible(volumeSlider);

    // إعداد منزلق السرعة - نفس VolumeSlider بالضبط
    speedSlider.setRange(0.5, 2.0, 0.1);
    speedSlider.setValue(1.0);
    speedSlider.addListener(this);
    speedSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 50, 20);
    speedSlider.setTextValueSuffix("x"); // إضافة x بعد الرقم
    addAndMakeVisible(speedSlider);

    // بدء التايمر للتحقق من التكرار
    startTimer(100);
}

PlayerGUI::~PlayerGUI() {}

void PlayerGUI::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    playerAudio.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerGUI::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    playerAudio.getNextAudioBlock(bufferToFill);
}

void PlayerGUI::releaseResources()
{
    playerAudio.releaseResources();
}

void PlayerGUI::resized()
{
    auto area = getLocalBounds();

    // مساحة للعنوان
    auto header = area.removeFromTop(40);

    // إعداد أحجام ومواقع الأزرار
    int buttonWidth = 80;
    int buttonHeight = 30;
    int y = 50;

    loadButton.setBounds(10, y, buttonWidth, buttonHeight);
    playButton.setBounds(100, y, buttonWidth, buttonHeight);
    goToStartButton.setBounds(190, y, buttonWidth, buttonHeight);
    pauseButton.setBounds(280, y, buttonWidth, buttonHeight);
    goToEndButton.setBounds(370, y, buttonWidth, buttonHeight);
    stopButton.setBounds(460, y, buttonWidth, buttonHeight);
    loopButton.setBounds(550, y, buttonWidth, buttonHeight);
    muteButton.setBounds(640, y, buttonWidth, buttonHeight);

    // إعداد مواقع المنزلقات - نفس التنسيق
    volumeSlider.setBounds(10, 90, getWidth() - 20, 25);
    speedSlider.setBounds(10, 130, getWidth() - 20, 25);
}

void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (button == &loadButton)
    {
        // فتح ملف صوت
        fileChooser = std::make_unique<juce::FileChooser>(
            "Select an audio file...",
            juce::File{},
            "*.wav;*.mp3;*.aiff;*.flac");

        fileChooser->launchAsync(
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& fc)
            {
                auto file = fc.getResult();
                if (file.existsAsFile())
                {
                    playerAudio.loadFile(file);
                }
            });
    }
    else if (button == &playButton)
    {
        playerAudio.play();
    }
    else if (button == &pauseButton) {
        playerAudio.pause();
    }
    else if (button == &goToStartButton) {
        playerAudio.goToStart();
    }
    else if (button == &goToEndButton) {
        playerAudio.goToEnd();
    }
    else if (button == &stopButton)
    {
        playerAudio.stop();
    }
    else if (button == &loopButton)
    {
        bool currentLooping = playerAudio.isLooping();
        playerAudio.setLooping(!currentLooping);
        loopButton.setButtonText(playerAudio.isLooping() ? "Loop ON" : "Loop OFF");
    }
    else if (button == &muteButton)
    {
        if (!isMuted)
        {
            lastVolume = (float)volumeSlider.getValue();
            playerAudio.setGain(0.0f);
            isMuted = true;
            muteButton.setButtonText("Unmute");
        }
        else
        {
            playerAudio.setGain(lastVolume);
            isMuted = false;
            muteButton.setButtonText("Mute");
        }
        volumeSlider.setValue(isMuted ? 0.0f : lastVolume, juce::dontSendNotification);
    }
}

void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &speedSlider)
    {
        // تغيير السرعة
        playerAudio.setSpeed(speedSlider.getValue());
    }

    if (slider == &volumeSlider)
    {
        playerAudio.setGain((float)volumeSlider.getValue());

        if (isMuted && volumeSlider.getValue() > 0.0f)
        {
            isMuted = false;
            muteButton.setButtonText("Mute");
            lastVolume = (float)volumeSlider.getValue();
        }
    }
}

void PlayerGUI::timerCallback()
{
    playerAudio.checkForLoop();
}

void PlayerGUI::paint(juce::Graphics& g)
{
    // خلفية المكون
    g.fillAll(juce::Colour(0xff1a2536));

    // العنوان
    g.setColour(juce::Colour(0xff4fc3f7));
    g.setFont(juce::Font(20.0f, juce::Font::bold));
    g.drawText("Audio Player", getLocalBounds().removeFromTop(40),
        juce::Justification::centred, true);

    // تسمية المنزلقات
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(14.0f, juce::Font::bold));


    // إطار
    g.setColour(juce::Colour(0xff26c6da));
    g.drawRect(getLocalBounds(), 1);
}