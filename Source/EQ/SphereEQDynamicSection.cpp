/*
  ==============================================================================

    SphereEQDynamicSection.cpp
    Created: 8 Dec 2025
    Author:  Sphere Synth AI

  ==============================================================================
*/

#include "SphereEQDynamicSection.h"

namespace Sphere
{

    SphereEQDynamicSection::SphereEQDynamicSection()
    {
        updateTimeConstants();
    }

    void SphereEQDynamicSection::prepare(double newSampleRate)
    {
        sampleRate = newSampleRate;
        updateTimeConstants();
        reset();
    }

    void SphereEQDynamicSection::reset()
    {
        envelope = 0.0;
    }

    void SphereEQDynamicSection::setParameters(double thresholddB, double rangedB)
    {
        threshold = thresholddB;
        range = rangedB;
    }

    void SphereEQDynamicSection::setAttackRelease(double attackMs, double releaseMs)
    {
        (void)attackMs;
        (void)releaseMs;
        updateTimeConstants();
    }

    void SphereEQDynamicSection::updateTimeConstants()
    {
        attackCoeff = std::exp(-1.0 / ( (attackTimeMs / 1000.0) * sampleRate));
        releaseCoeff = std::exp(-1.0 / ( (releaseTimeMs / 1000.0) * sampleRate));
    }

    double SphereEQDynamicSection::calculateGainOffset(double controlSignal)
    {
        double inputAbs = std::abs(controlSignal);

        if (inputAbs > envelope)
            envelope = attackCoeff * envelope + (1.0 - attackCoeff) * inputAbs;
        else
            envelope = releaseCoeff * envelope + (1.0 - releaseCoeff) * inputAbs;

        double envDB = juce::Decibels::gainToDecibels(envelope + 1e-9);
        double gainOffset = 0.0;

        if (envDB > threshold)
        {
            double over = envDB - threshold;
            double scale = 0.5;
            double proposedChange = over * scale;

            if (range > 0)
            {
                if (proposedChange > range) proposedChange = range;
                gainOffset = proposedChange;
            }
            else
            {
                proposedChange = -proposedChange;
                if (proposedChange < range) proposedChange = range;
                gainOffset = proposedChange;
            }
        }
        
        return gainOffset;
    }

} // namespace Sphere
