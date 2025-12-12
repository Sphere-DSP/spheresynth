/*
  ==============================================================================

    SphereEQAnalyzer.cpp
    Created: 8 Dec 2025
    Author:  Sphere Synth AI

  ==============================================================================
*/

#include "SphereEQAnalyzer.h"

namespace Sphere {

SphereEQAnalyzer::SphereEQAnalyzer() {
  magnitudes.resize(fftSize / 2, -100.0f);
}

void SphereEQAnalyzer::prepare(double newSampleRate) {
  sampleRate = newSampleRate;
}

void SphereEQAnalyzer::pushBuffer(const juce::AudioBuffer<float> &buffer) {
  // No-op
  juce::ignoreUnused(buffer);
}

void SphereEQAnalyzer::pushSample(float sample) {
  // No-op
  juce::ignoreUnused(sample);
}

bool SphereEQAnalyzer::getPath(juce::Path &p, juce::Rectangle<float> bounds) {
  // No-op
  p.clear();
  return false;
}

} // namespace Sphere
