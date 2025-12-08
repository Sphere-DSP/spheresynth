/*
  ==============================================================================

    SphereEQOversampler.h
    Created: 8 Dec 2025
    Author:  Sphere Synth AI

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace Sphere
{

    // Stub implementation as juce_dsp is missing
    class SphereEQOversampler
    {
    public:
        SphereEQOversampler();
        ~SphereEQOversampler() = default;

        void prepare(double sampleRate, int numChannels, int blockSize);
        void reset();
        
        void setOversamplingFactor(int factor); 

        // Simplified process wrapper
        template <typename ProcessContext, typename ProcessFn>
        void process(ProcessContext& context, ProcessFn&& processFn)
        {
            // Just call the function directly (bypass)
            processFn(context);
        }
        
        // Overload for AudioBuffer
        template <typename ProcessFn>
        void processBuffer(juce::AudioBuffer<float>& buffer, ProcessFn&& processFn)
        {
             processFn(buffer);
        }

    private:
        int currentFactor = 1;
    };

} // namespace Sphere
