/*
  ==============================================================================

    SphereEQDynamicSection.h
    Created: 8 Dec 2025
    Author:  Sphere Synth AI

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace Sphere
{

    /**
     * Handles the dynamics processing logic for a single EQ band.
     * Calculates the dynamic gain offset based on input signal level.
     */
    class SphereEQDynamicSection
    {
    public:
        SphereEQDynamicSection();
        ~SphereEQDynamicSection() = default;

        // Removed dsp::ProcessSpec dependency
        void prepare(double sampleRate);
        void reset();

        void setParameters(double thresholddB, double rangedB);
        
        void setAttackRelease(double attackMs, double releaseMs);

        double calculateGainOffset(double controlSignal);

    private:
        double sampleRate = 44100.0;
        
        // Parameters
        double threshold = 0.0;
        double range = 0.0;

        // Envelope Follower
        double envelope = 0.0;
        double attackCoeff = 0.0;
        double releaseCoeff = 0.0;

        // Constants
        const double attackTimeMs = 20.0;
        const double releaseTimeMs = 200.0; 

        void updateTimeConstants();
    };

} // namespace Sphere
