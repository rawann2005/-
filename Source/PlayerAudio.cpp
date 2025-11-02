#include "PlayerAudio.h"

PlayerAudio::PlayerAudio()
{
    formatManager.registerBasicFormats();
}

PlayerAudio::~PlayerAudio()
{
    transportSource.setSource(nullptr);
}

void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    // غير هنا - دايماً حضّر الـ transportSource أولاً
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);

    // بعدين لو في resampleSource حضّره
    if (resampleSource != nullptr)
        resampleSource->prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    // غير هنا - ابدأ بالـ resampleSource لو موجود
    if (resampleSource != nullptr)
        resampleSource->getNextAudioBlock(bufferToFill);
    else
        transportSource.getNextAudioBlock(bufferToFill);
}

void PlayerAudio::releaseResources()
{
    // غير هنا - دايماً حرر الـ transportSource أولاً
    transportSource.releaseResources();

    if (resampleSource != nullptr)
        resampleSource->releaseResources();
}

bool PlayerAudio::loadFile(const juce::File& file)
{
    if (auto* reader = formatManager.createReaderFor(file))
    {
        transportSource.stop();
        transportSource.setSource(nullptr);
        readerSource.reset();

        readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);

        // غير هنا - اربط الـ transportSource أولاً
        transportSource.setSource(readerSource.get(), 0, nullptr, reader->sampleRate);

        // بعدين أنشئ الـ resampleSource
        resampleSource = std::make_unique<juce::ResamplingAudioSource>(&transportSource, false, 2);

        // حضّر الـ resampleSource بالـ sample rate الصحيح
        resampleSource->prepareToPlay(512, reader->sampleRate);

        // ضبط السرعة الافتراضية
        resampleSource->setResamplingRatio(1.0);

        return true;
    }
    return false;
}

// 🔥 هذه هي الدالة الوحيدة اللي اختلفت كلياً
void PlayerAudio::setSpeed(double speed)
{
    // غير السرعة من غير ما توقف الأغنية أو تبدأ من الأول
    if (resampleSource != nullptr && speed >= 0.5 && speed <= 2.0)
    {
        // غير السرعة مباشرة - الأغنية هتكمل من حيث وقفت
        resampleSource->setResamplingRatio(speed);
    }
}

// باقي الدوال زي ما هي تماماً بدون تغيير
void PlayerAudio::play()
{
    transportSource.start();
}

void PlayerAudio::pause()
{
    transportSource.stop();
}

void PlayerAudio::stop()
{
    transportSource.stop();
    transportSource.setPosition(0.0);
}

void PlayerAudio::goToStart()
{
    transportSource.setPosition(0.0);
}

void PlayerAudio::goToEnd() {
    double length = transportSource.getLengthInSeconds();
    transportSource.setPosition(length - 0.01);
}

void PlayerAudio::setGain(float gain)
{
    transportSource.setGain(gain);
}

void PlayerAudio::setPosition(double pos)
{
    transportSource.setPosition(pos);
}

double PlayerAudio::getPosition() const
{
    return transportSource.getCurrentPosition();
}

double PlayerAudio::getLength() const
{
    return transportSource.getLengthInSeconds();
}

void PlayerAudio::setLooping(bool shouldLoop)
{
    looping = shouldLoop;
    if (readerSource != nullptr)
        readerSource->setLooping(shouldLoop);
}

bool PlayerAudio::isLooping() const
{
    return looping;
}

void PlayerAudio::checkForLoop()
{
    if (looping && transportSource.hasStreamFinished())
    {
        transportSource.setPosition(0.0);
        transportSource.start();
    }
}