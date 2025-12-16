/*
  ==============================================================================

    SphereEQEngine.cpp
    Created: 8 Dec 2025
    Author:  Sphere Synth AI

  ==============================================================================
*/

#include "SphereEQEngine.h"

namespace Sphere {

SphereEQEngine::SphereEQEngine() {
  // Create bands
  for (int i = 0; i < MaxBands; ++i) {
    bands.push_back(std::make_unique<SphereEQBand>());

    // Default params: Inactive
    BandParameters params;
    params.active = false;
    bands.back()->setParameters(params);
  }

  // Default first band active for testing
  BandParameters p0;
  p0.active = true;
  p0.type = FilterType::LowCut;
  p0.frequency = 30.0;
  bands[0]->setParameters(p0);
}

SphereEQEngine::~SphereEQEngine() {}

void SphereEQEngine::prepare(double newSampleRate, int samplesPerBlock,
                             int numChannels) {
  sampleRate = newSampleRate;

  oversampler.prepare(sampleRate, numChannels, samplesPerBlock);
  analyzer.prepare(sampleRate);

  for (auto &band : bands) {
    band->prepare(sampleRate, numChannels);
  }
}

void SphereEQEngine::reset() {
  oversampler.reset();
  for (auto &band : bands) {
    band->reset();
  }
}

void SphereEQEngine::setBandParameters(int bandIndex,
                                       const BandParameters &params) {
  if (bandIndex >= 0 && bandIndex < (int)bands.size()) {
    bands[bandIndex]->setParameters(params);
  }
}

const BandParameters &SphereEQEngine::getBandParameters(int bandIndex) const {
  static BandParameters dummy;
  if (bandIndex >= 0 && bandIndex < (int)bands.size()) {
    return bands[bandIndex]->getParameters();
  }
  return dummy;
}

void SphereEQEngine::setOversampling(int factor) {
  oversampler.setOversamplingFactor(factor);
}

void SphereEQEngine::setAnalyzerEnabled(bool enabled) {
  analyzerEnabled = enabled;
}

void SphereEQEngine::processBlock(juce::AudioBuffer<float> &buffer,
                                  juce::MidiBuffer &midiMessages) {
  juce::ignoreUnused(midiMessages);

  // Process through Oversampler wrapper
  oversampler.processBuffer(
      buffer, [this](juce::AudioBuffer<float> &buf) { processInternal(buf); });

  // Analysis
  if (analyzerEnabled) {
    analyzer.pushBuffer(buffer);
  }
}

void SphereEQEngine::processInternal(juce::AudioBuffer<float> &buffer) {
  // Serial processing through bands
  for (auto &band : bands) {
    band->process(buffer);
  }
}

} // namespace Sphere
