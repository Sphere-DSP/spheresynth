/*
  ==============================================================================

    SphereEQBand.cpp
    Created: 8 Dec 2025
    Author:  Sphere Synth AI

  ==============================================================================
*/

#include "SphereEQBand.h"

namespace Sphere
{

    SphereEQBand::SphereEQBand()
    {
    }

    SphereEQBand::~SphereEQBand()
    {
    }

    void SphereEQBand::prepare(double newSampleRate, int numChannels)
    {
        sampleRate = newSampleRate;
        createFilters(numChannels);

        for (auto& filter : filters)
        {
            filter->reset();
        }

        // Initialize dynamics
        // Note: DynamicSection used juce::dsp::ProcessSpec but actually only needs sample rate
        // We need to update SphereEQDynamicSection to not use dsp::ProcessSpec too.
        // Assuming we fixed that or will fix it.
        // Let's fix SphereEQDynamicSection first or assume prepare(double sampleRate) exists.
        
        dynamicSections.clear();
        for (int i = 0; i < numChannels; ++i)
        {
            auto ds = std::make_unique<SphereEQDynamicSection>();
            // Hack: convert to spec or change API. We will change API of DynamicSection too.
            ds->prepare(sampleRate); 
            ds->setParameters(params.threshold, params.dynamicRange);
            dynamicSections.push_back(std::move(ds));
        }

        updateCoefficients();
    }

    void SphereEQBand::reset()
    {
        for (auto& filter : filters)
            filter->reset();

        for (auto& ds : dynamicSections)
            ds->reset();
    }

    void SphereEQBand::createFilters(int numChannels)
    {
        filters.clear();
        for (int i = 0; i < numChannels; ++i)
        {
            filters.push_back(std::make_unique<juce::IIRFilter>());
        }
    }

    void SphereEQBand::setParameters(const BandParameters& newParams)
    {
        params = newParams;
        
        // Update dynamic sections parameters
        for (auto& ds : dynamicSections)
        {
            ds->setParameters(params.threshold, params.dynamicRange);
        }

        updateCoefficients();
    }

    void SphereEQBand::updateCoefficients()
    {
        float gainFactor = juce::Decibels::decibelsToGain((float)params.gain);
        coefficients = makeCoefficients(params.gain, gainFactor);

        for (auto& filter : filters)
        {
            filter->setCoefficients(coefficients);
        }
    }

    juce::IIRCoefficients SphereEQBand::makeCoefficients(double gainDB, float gainLinear) const
    {
        // juce::IIRCoefficients uses static factory methods
        // Frequencies are required.
        
        switch (params.type)
        {
            case FilterType::Bell:
                return juce::IIRCoefficients::makePeakFilter(sampleRate, params.frequency, params.q, gainLinear);
            
            case FilterType::LowShelf:
                return juce::IIRCoefficients::makeLowShelf(sampleRate, params.frequency, params.q, gainLinear);
            
            case FilterType::HighShelf:
                return juce::IIRCoefficients::makeHighShelf(sampleRate, params.frequency, params.q, gainLinear);
            
            case FilterType::LowCut:
                return juce::IIRCoefficients::makeHighPass(sampleRate, params.frequency, params.q);
            
            case FilterType::HighCut:
                return juce::IIRCoefficients::makeLowPass(sampleRate, params.frequency, params.q);
            
            case FilterType::Notch:
                return juce::IIRCoefficients::makeNotchFilter(sampleRate, params.frequency, params.q);
            
            case FilterType::BandPass:
                return juce::IIRCoefficients::makeBandPass(sampleRate, params.frequency, params.q);
            
            case FilterType::TiltShelf:
                return juce::IIRCoefficients::makeLowShelf(sampleRate, params.frequency, 0.3, gainLinear);
                
            default:
                return juce::IIRCoefficients::makePeakFilter(sampleRate, params.frequency, params.q, gainLinear);
        }
    }

    double SphereEQBand::processSample(int channel, double inputSample)
    {
        if (!params.active)
            return inputSample;

        // IIRFilter::processSingleSampleRaw works on float usually, but processSample takes float.
        // processSingleSampleRaw(float sample)
        
        // If not dynamic, just process
        if (!params.dynamicActive || channel >= (int)dynamicSections.size())
        {
            if (channel < (int)filters.size())
                return filters[channel]->processSingleSampleRaw((float)inputSample);
            return inputSample;
        }

        double gainOffset = dynamicSections[channel]->calculateGainOffset(inputSample);
        
        if (std::abs(gainOffset) > 0.01) 
        {
             double effectiveGainDB = params.gain + gainOffset;
             float gainFactor = juce::Decibels::decibelsToGain((float)effectiveGainDB);
             
             auto newCoeffs = makeCoefficients(effectiveGainDB, gainFactor);
             filters[channel]->setCoefficients(newCoeffs);
        }

        if (channel < (int)filters.size())
             return filters[channel]->processSingleSampleRaw((float)inputSample);
             
        return inputSample;
    }

    void SphereEQBand::process(juce::AudioBuffer<float>& buffer)
    {
         if (!params.active) return;
         
         int numSamples = buffer.getNumSamples();
         int numChannels = buffer.getNumChannels();
         
         // Ensure filters match channel count
         if ((int)filters.size() != numChannels)
         {
             // Should have been prepared, but safe guard
             return;
         }

         if (!params.dynamicActive)
         {
             for (int ch = 0; ch < numChannels; ++ch)
             {
                 filters[ch]->processSamples(buffer.getWritePointer(ch), numSamples);
             }
             return;
         }

         // Dynamic Processing with Chunking
         const int chunkSize = 32;

         for (int ch = 0; ch < numChannels; ++ch)
         {
             if (ch >= (int)dynamicSections.size()) continue;

             float* channelData = buffer.getWritePointer(ch);
             
             for (int i = 0; i < numSamples; i += chunkSize)
             {
                 int thisChunk = std::min(chunkSize, numSamples - i);
                 
                 // 1. Analyze
                 double sampleVal = (double)channelData[i];
                 double currentGainOffset = dynamicSections[ch]->calculateGainOffset(sampleVal);
                 
                 for (int k = 1; k < thisChunk; ++k)
                      dynamicSections[ch]->calculateGainOffset((double)channelData[i+k]);

                 // 2. Update Coeffs
                 double effectiveGain = params.gain + currentGainOffset;
                 float gainFactor = juce::Decibels::decibelsToGain((float)effectiveGain);
                 auto newCoeffs = makeCoefficients(effectiveGain, gainFactor);
                 filters[ch]->setCoefficients(newCoeffs);

                 // 3. Process Chunk
                 filters[ch]->processSamples(channelData + i, thisChunk);
             }
         }
    }

} // namespace Sphere
