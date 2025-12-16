/*
  ==============================================================================

    SphereEQAnalyzer.h
    Created: 8 Dec 2025
    Author:  Sphere Synth AI

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <vector>

namespace Sphere {
// Stub implementation as juce_dsp (and FFT) is missing
class SphereEQAnalyzer {
public:
  SphereEQAnalyzer() { magnitudes.resize(fftSize / 2, -100.0f); }

  ~SphereEQAnalyzer() = default;

  void prepare(double newSampleRate) { sampleRate = newSampleRate; }

  void pushBuffer(const juce::AudioBuffer<float> &buffer) {
    juce::ignoreUnused(buffer);
  }

  void pushSample(float sample) { juce::ignoreUnused(sample); }

  bool getPath(juce::Path &p, juce::Rectangle<float> bounds) {
    juce::ignoreUnused(bounds);
    p.clear();
    return false;
  }

  const std::vector<float> &getMagnitudes() const { return magnitudes; }

  enum { fftOrder = 11, fftSize = 1 << fftOrder };

private:
  double sampleRate = 44100.0;
  std::vector<float> magnitudes;
};

} // namespace Sphere
