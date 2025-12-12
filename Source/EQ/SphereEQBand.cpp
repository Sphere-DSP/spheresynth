/*
  ==============================================================================

    SphereEQBand.cpp
    Created: 8 Dec 2025
    Author:  Sphere Synth AI

  ==============================================================================
*/

#include "SphereEQBand.h"

namespace Sphere {

SphereEQBand::SphereEQBand() {}

SphereEQBand::~SphereEQBand() {}

void SphereEQBand::prepare(double newSampleRate, int numCh) {
  sampleRate = newSampleRate;
  numChannels = numCh;
  createFilters(numCh);

  for (auto &filter : filters) {
    filter->reset();
  }

  // Initialize dynamics
  dynamicSections.clear();
  for (int i = 0; i < numCh; ++i) {
    auto ds = std::make_unique<SphereEQDynamicSection>();
    ds->prepare(sampleRate);
    ds->setParameters(params.threshold, params.dynamicRange);
    dynamicSections.push_back(std::move(ds));
  }

  // Initialize spectral processors
  prepareSpectralProcessors();

  updateCoefficients();
}

void SphereEQBand::prepareSpectralProcessors() {
  spectralProcessors.clear();
  for (int i = 0; i < numChannels; ++i) {
    auto sp = std::make_unique<SpectralDynamicsProcessor>();
    sp->prepare(sampleRate, 4096, params.spectralQuality);
    sp->setParameters(params.frequency / 2.0, // Band low (half bandwidth)
                      params.frequency * 2.0, // Band high (double bandwidth)
                      params.threshold, params.ratio, params.attack,
                      params.release, params.spectralCompress);
    spectralProcessors.push_back(std::move(sp));
  }
}

void SphereEQBand::reset() {
  for (auto &filter : filters)
    filter->reset();

  for (auto &ds : dynamicSections)
    ds->reset();

  for (auto &sp : spectralProcessors)
    sp->reset();
}

void SphereEQBand::createFilters(int numCh) {
  filters.clear();
  for (int i = 0; i < numCh; ++i) {
    filters.push_back(std::make_unique<juce::IIRFilter>());
  }
}

void SphereEQBand::setParameters(const BandParameters &newParams) {
  bool spectralParamsChanged =
      params.spectralQuality != newParams.spectralQuality ||
      params.dynamicMode != newParams.dynamicMode;

  params = newParams;

  // Update dynamic sections parameters
  for (auto &ds : dynamicSections) {
    ds->setParameters(params.threshold, params.dynamicRange);
  }

  // Update spectral processors if needed
  if (spectralParamsChanged &&
      params.dynamicMode == DynamicMode::SpectralDynamics) {
    prepareSpectralProcessors();
  } else {
    for (auto &sp : spectralProcessors) {
      sp->setParameters(params.frequency / 2.0, params.frequency * 2.0,
                        params.threshold, params.ratio, params.attack,
                        params.release, params.spectralCompress);
    }
  }

  updateCoefficients();
}

int SphereEQBand::getLatencySamples() const {
  if (params.dynamicMode == DynamicMode::SpectralDynamics &&
      !spectralProcessors.empty()) {
    return spectralProcessors[0]->getLatencySamples();
  }
  return 0;
}

void SphereEQBand::updateCoefficients() {
  float gainFactor = juce::Decibels::decibelsToGain((float)params.gain);
  coefficients = makeCoefficients(params.gain, gainFactor);

  for (auto &filter : filters) {
    filter->setCoefficients(coefficients);
  }
}

juce::IIRCoefficients SphereEQBand::makeCoefficients(double gainDB,
                                                     float gainLinear) const {
  switch (params.type) {
  case FilterType::Bell:
    return juce::IIRCoefficients::makePeakFilter(sampleRate, params.frequency,
                                                 params.q, gainLinear);

  case FilterType::LowShelf:
    return juce::IIRCoefficients::makeLowShelf(sampleRate, params.frequency,
                                               params.q, gainLinear);

  case FilterType::HighShelf:
    return juce::IIRCoefficients::makeHighShelf(sampleRate, params.frequency,
                                                params.q, gainLinear);

  case FilterType::LowCut:
    return juce::IIRCoefficients::makeHighPass(sampleRate, params.frequency,
                                               params.q);

  case FilterType::HighCut:
    return juce::IIRCoefficients::makeLowPass(sampleRate, params.frequency,
                                              params.q);

  case FilterType::Notch:
    return juce::IIRCoefficients::makeNotchFilter(sampleRate, params.frequency,
                                                  params.q);

  case FilterType::BandPass:
    return juce::IIRCoefficients::makeBandPass(sampleRate, params.frequency,
                                               params.q);

  case FilterType::TiltShelf:
    return juce::IIRCoefficients::makeLowShelf(sampleRate, params.frequency,
                                               0.3, gainLinear);

  default:
    return juce::IIRCoefficients::makePeakFilter(sampleRate, params.frequency,
                                                 params.q, gainLinear);
  }
}

double SphereEQBand::processSample(int channel, double inputSample) {
  if (!params.active)
    return inputSample;

  // For spectral dynamics mode, must use block processing
  if (params.dynamicMode == DynamicMode::SpectralDynamics) {
    // Single sample not efficient for FFT, return unprocessed
    // Use process(buffer) for spectral mode
    if (channel < (int)filters.size())
      return filters[channel]->processSingleSampleRaw((float)inputSample);
    return inputSample;
  }

  // If not dynamic, just process
  if (!params.dynamicActive || params.dynamicMode == DynamicMode::Static ||
      channel >= (int)dynamicSections.size()) {
    if (channel < (int)filters.size())
      return filters[channel]->processSingleSampleRaw((float)inputSample);
    return inputSample;
  }

  double gainOffset =
      dynamicSections[channel]->calculateGainOffset(inputSample);

  if (std::abs(gainOffset) > 0.01) {
    double effectiveGainDB = params.gain + gainOffset;
    float gainFactor = juce::Decibels::decibelsToGain((float)effectiveGainDB);

    auto newCoeffs = makeCoefficients(effectiveGainDB, gainFactor);
    filters[channel]->setCoefficients(newCoeffs);
  }

  if (channel < (int)filters.size())
    return filters[channel]->processSingleSampleRaw((float)inputSample);

  return inputSample;
}

void SphereEQBand::process(juce::AudioBuffer<float> &buffer) {
  if (!params.active)
    return;

  int numSamples = buffer.getNumSamples();
  int numCh = buffer.getNumChannels();

  // Ensure filters match channel count
  if ((int)filters.size() != numCh) {
    return;
  }

  // ================================================================
  // Spectral Dynamics Mode
  // ================================================================
  if (params.dynamicMode == DynamicMode::SpectralDynamics) {
    for (int ch = 0; ch < numCh && ch < (int)spectralProcessors.size(); ++ch) {
      float *channelData = buffer.getWritePointer(ch);
      spectralProcessors[ch]->process(channelData, numSamples);
    }
    return;
  }

  // ================================================================
  // Static Mode (no dynamics)
  // ================================================================
  if (params.dynamicMode == DynamicMode::Static || !params.dynamicActive) {
    for (int ch = 0; ch < numCh; ++ch) {
      filters[ch]->processSamples(buffer.getWritePointer(ch), numSamples);
    }
    return;
  }

  // ================================================================
  // Traditional Dynamic EQ Mode
  // ================================================================
  const int chunkSize = 32;

  for (int ch = 0; ch < numCh; ++ch) {
    if (ch >= (int)dynamicSections.size())
      continue;

    float *channelData = buffer.getWritePointer(ch);

    for (int i = 0; i < numSamples; i += chunkSize) {
      int thisChunk = std::min(chunkSize, numSamples - i);

      // 1. Analyze
      double sampleVal = (double)channelData[i];
      double currentGainOffset =
          dynamicSections[ch]->calculateGainOffset(sampleVal);

      for (int k = 1; k < thisChunk; ++k)
        dynamicSections[ch]->calculateGainOffset((double)channelData[i + k]);

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
