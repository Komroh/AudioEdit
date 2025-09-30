#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 600);
    formatManager.registerBasicFormats();
    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2); 
    }
   
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{

    juce::File myFile("C:\\Users\\pacau\\Documents\\first_ableton Project\\Space_Explorer.flac");
    audioSrc = getAudioBufferFromFile(myFile);
    deltaTime = 1/sampleRate;
    
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!
    bufferToFill.clearActiveBufferRegion();

    audioSrc->getNextAudioBlock(bufferToFill);
    auto buffer = bufferToFill.buffer;
    auto numChannels = buffer->getNumChannels();
    auto numSamples = buffer->getNumSamples();
    juce::AudioBuffer<float> outputBuffer(*buffer);
    float meanAboveThld = 0.0;
    auto threshold = 0.62;

    for (auto chan = 0; chan < numChannels; chan++)
    {
        auto rPtr = buffer->getReadPointer(chan);

        for (auto sample = 0; sample < numSamples; sample++)
        {
            if (abs(rPtr[sample]) >= threshold)
                meanAboveThld += abs(rPtr[sample]) / numSamples;
        }
    }

    auto level = 0.1f;
    if (time >= std::numeric_limits<double>::max())
    {
        time = 0.0;
    }

    auto monoBuff = std::make_unique<float[]>(numSamples);
    for (auto sample = 0; sample < numSamples; ++sample)
    {
        auto sinVal = level * sin(2 * juce::MathConstants<double>::pi * frequency * time + phase);
        monoBuff[sample] = sinVal;
        time += deltaTime;
    }
    
    meanAboveThld += prevMean / 2;
    prevMean = meanAboveThld;
    if (meanAboveThld > 0.0002)
    {

    
        for (auto chan = 0; chan < numChannels; ++chan)
        {
            auto wPtr =  buffer->getWritePointer(chan, bufferToFill.startSample);
       
            for (auto sample = 0; sample < numSamples; ++sample)
            {                wPtr[sample] += monoBuff[sample];

            }
        }
    }
}

std::unique_ptr<juce::AudioFormatReaderSource> MainComponent::getAudioBufferFromFile(juce::File file)
{
    std::cout << file.getFileName() << std::endl;
    auto reader = formatManager.createReaderFor(file);
    juce::AudioBuffer<float> audioBuffer;
    auto newSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
    return newSource;
}



void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}
