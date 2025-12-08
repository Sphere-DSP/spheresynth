/*
  ==============================================================================

    SphereEQEngine.h
    Created: 8 Dec 2025
    Author:  Sphere Synth AI

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SphereEQBand.h"
#include "SphereEQOversampler.h"
#include "SphereEQAnalyzer.h"

namespace Sphere
{

    class SphereEQEngine
    {
    public:
        SphereEQEngine();
        ~SphereEQEngine();

        // Updated prepare API
        void prepare(double sampleRate, int samplesPerBlock, int numChannels);
        void reset();

        // Parameter Management
        void setBandParameters(int bandIndex, const BandParameters& params);
        const BandParameters& getBandParameters(int bandIndex) const;
        
        void setOversampling(int factor); 
        void setAnalyzerEnabled(bool enabled);
        
        // Processing
        void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages);

        // Accessors
        SphereEQAnalyzer& getAnalyzer() { return analyzer; }
        
        static const int MaxBands = 8;

    private:
        double sampleRate = 44100.0;
        
        // Components
        std::vector<std::unique_ptr<SphereEQBand>> bands;
        SphereEQOversampler oversampler;
        SphereEQAnalyzer analyzer;
        
        bool analyzerEnabled = true;

        void processInternal(juce::AudioBuffer<float>& buffer);
    };

} // namespace Sphere
