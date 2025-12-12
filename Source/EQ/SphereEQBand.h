/*
  ==============================================================================

    SphereEQBand.h
    Created: 8 Dec 2025
    Author:  Sphere Synth AI

  ==============================================================================
*/

#pragma once

#include "SphereEQDynamicSection.h"
#include "SphereEQTypes.h"
#include "SphereSpectralDynamics.h"
#include <JuceHeader.h>

namespace Sphere {

enum class FilterType {
  Bell,
  LowShelf,
  HighShelf,
  LowCut,
  HighCut,
  Notch,
  BandPass,
  TiltShelf
};

// Dynamic mode for band processing
enum class DynamicMode {
  Static,          // No dynamics
  DynamicEQ,       // Traditional sidechain-based dynamic EQ
  SpectralDynamics // FFT-based per-bin processing
};

struct BandParameters {
  bool active = true;
  FilterType type = FilterType::Bell;
  double frequency = 1000.0;
  double q = 0.707;
  double gain = 0.0; // In Decibels

  // Dynamics parameters
  bool dynamicActive = false;
  double dynamicRange = 0.0; // +/- dB
  double threshold = -20.0;  // dB
  double attack = 10.0;      // ms
  double release = 100.0;    // ms
  double ratio = 2.0;

  // Dynamic mode selection
  DynamicMode dynamicMode = DynamicMode::Static;
  SpectralQuality spectralQuality = SpectralQuality::Normal;
  bool spectralCompress = true; // true = compress, false = expand
};

class SphereEQBand {
public:
  SphereEQBand();
  ~SphereEQBand();

  void prepare(double sampleRate, int numChannels);
  void reset();

  void setParameters(const BandParameters &newParams);
  const BandParameters &getParameters() const { return params; }

  // Process a single sample (mono)
  double processSample(int channel, double inputSample);

  // Process a block of samples (stereo/multichannel)
  void process(juce::AudioBuffer<float> &buffer);

  void updateCoefficients();

  // Get latency for spectral mode
  int getLatencySamples() const;

private:
  BandParameters params;
  double sampleRate = 44100.0;
  int numChannels = 2;

  // Using juce::IIRFilter (legacy module, available in juce_audio_basics)
  std::vector<std::unique_ptr<juce::IIRFilter>> filters;
  juce::IIRCoefficients coefficients;

  std::vector<std::unique_ptr<SphereEQDynamicSection>> dynamicSections;

  // Spectral dynamics processor (per channel)
  std::vector<std::unique_ptr<SpectralDynamicsProcessor>> spectralProcessors;

  void createFilters(int numChannels);
  void prepareSpectralProcessors();

  // Helper to create coeffs
  juce::IIRCoefficients makeCoefficients(double gainDB, float gainLinear) const;
};

} // namespace Sphere
