/*
  ==============================================================================

    SphereEQAnalyzer.h
    Created: 8 Dec 2025
    Author:  Sphere Synth AI

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace Sphere
{
    // Stub implementation as juce_dsp (and FFT) is missing
    class SphereEQAnalyzer
    {
    public:
        SphereEQAnalyzer();
        ~SphereEQAnalyzer() = default;

        void prepare(double sampleRate);
        
        void pushBuffer(const juce::AudioBuffer<float>& buffer);

        void pushSample(float sample);

        bool getPath(juce::Path& p, juce::Rectangle<float> bounds);
        
        const std::vector<float>& getMagnitudes() const { return magnitudes; }

        enum
        {
            fftOrder = 11,
            fftSize  = 1 << fftOrder
        };

    private:
        double sampleRate = 44100.0;
        std::vector<float> magnitudes;
    };

} // namespace Sphere
