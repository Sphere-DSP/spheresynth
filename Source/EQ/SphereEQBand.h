/*
  ==============================================================================

    SphereEQBand.h
    Created: 8 Dec 2025
    Author:  Sphere Synth AI

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SphereEQDynamicSection.h"

namespace Sphere
{

    enum class FilterType
    {
        Bell,
        LowShelf,
        HighShelf,
        LowCut,
        HighCut,
        Notch,
        BandPass,
        TiltShelf
    };

    struct BandParameters
    {
        bool active = true;
        FilterType type = FilterType::Bell;
        double frequency = 1000.0;
        double q = 0.707;
        double gain = 0.0; // In Decibels
        
        // Dynamics parameters
        bool dynamicActive = false;
        double dynamicRange = 0.0; // +/- dB
        double threshold = -20.0; // dB
    };

    class SphereEQBand
    {
    public:
        SphereEQBand();
        ~SphereEQBand();

        void prepare(double sampleRate, int numChannels);
        void reset();

        void setParameters(const BandParameters& newParams);
        const BandParameters& getParameters() const { return params; }

        // Process a single sample (mono)
        double processSample(int channel, double inputSample);
        
        // Process a block of samples (stereo/multichannel)
        void process(juce::AudioBuffer<float>& buffer);

        void updateCoefficients();

    private:
        BandParameters params;
        double sampleRate = 44100.0;
        
        // Using juce::IIRFilter (legacy module, available in juce_audio_basics)
        std::vector<std::unique_ptr<juce::IIRFilter>> filters;
        juce::IIRCoefficients coefficients;
        
        std::vector<std::unique_ptr<SphereEQDynamicSection>> dynamicSections;

        void createFilters(int numChannels);
        
        // Helper to create coeffs
        juce::IIRCoefficients makeCoefficients(double gainDB, float gainLinear) const;
    };

} // namespace Sphere
