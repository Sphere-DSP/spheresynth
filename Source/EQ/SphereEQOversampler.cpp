/*
  ==============================================================================

    SphereEQOversampler.cpp
    Created: 8 Dec 2025
    Author:  Sphere Synth AI

  ==============================================================================
*/

#include "SphereEQOversampler.h"

namespace Sphere
{

    SphereEQOversampler::SphereEQOversampler()
    {
    }

    void SphereEQOversampler::prepare(double sampleRate, int numChannels, int blockSize)
    {
        // No-op
        juce::ignoreUnused(sampleRate, numChannels, blockSize);
    }

    void SphereEQOversampler::reset()
    {
        // No-op
    }

    void SphereEQOversampler::setOversamplingFactor(int factor)
    {
        currentFactor = factor;
    }

} // namespace Sphere
