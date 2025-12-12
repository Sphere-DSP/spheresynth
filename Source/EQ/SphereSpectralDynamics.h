/*
  ==============================================================================
    SphereSpectralDynamics.h
    FFT-based Spectral Dynamics Processor

    Features:
    - Per-bin dynamic processing (compression/expansion)
    - Overlap-add reconstruction
    - Quality settings for CPU/latency trade-off

    Created: 10 Dec 2025
    Author:  Sphere Synth AI
  ==============================================================================
*/

#pragma once
#include "SphereEQTypes.h"
#include <JuceHeader.h>
#include <cmath>
#include <complex>
#include <vector>

namespace Sphere {

// ============================================================================
// Spectral Dynamics Processor
// FFT-based per-bin dynamic processing
// ============================================================================
class SpectralDynamicsProcessor {
public:
  SpectralDynamicsProcessor() = default;
  ~SpectralDynamicsProcessor() = default;

  // ========================================================================
  // Initialize processor
  // ========================================================================
  void prepare(double sampleRate, int maxBlockSize, SpectralQuality quality) {
    this->sampleRate = sampleRate;
    this->quality = quality;

    // Set FFT size based on quality
    if (quality == SpectralQuality::High) {
      fftOrder = 12;  // 4096
      hopSize = 1024; // 75% overlap
    } else {
      fftOrder = 11;  // 2048
      hopSize = 1024; // 50% overlap
    }

    fftSize = 1 << fftOrder;

    // Initialize FFT
    fft = std::make_unique<juce::dsp::FFT>(fftOrder);

    // Initialize buffers
    inputBuffer.resize(fftSize * 2, 0.0f);
    outputBuffer.resize(fftSize * 2, 0.0f);
    fftData.resize(fftSize * 2, 0.0f);
    window.resize(fftSize, 0.0f);
    binEnvelopes.resize(fftSize / 2 + 1, 0.0f);

    // Overlap-add buffer
    overlapBuffer.resize(fftSize, 0.0f);

    // Create Hanning window
    for (int i = 0; i < fftSize; ++i) {
      window[i] =
          0.5f * (1.0f - std::cos(2.0f * juce::MathConstants<float>::pi * i /
                                  (fftSize - 1)));
    }

    // Calculate window gain compensation
    float windowSum = 0.0f;
    for (int i = 0; i < fftSize; i += hopSize) {
      windowSum += window[i] * window[i];
    }
    windowGain = 1.0f / std::sqrt(windowSum / (fftSize / hopSize));

    inputWritePos = 0;
    outputReadPos = 0;
    samplesUntilNextFFT = hopSize;

    reset();
  }

  // ========================================================================
  // Reset state
  // ========================================================================
  void reset() {
    std::fill(inputBuffer.begin(), inputBuffer.end(), 0.0f);
    std::fill(outputBuffer.begin(), outputBuffer.end(), 0.0f);
    std::fill(overlapBuffer.begin(), overlapBuffer.end(), 0.0f);
    std::fill(binEnvelopes.begin(), binEnvelopes.end(), 0.0f);
    inputWritePos = 0;
    outputReadPos = 0;
    samplesUntilNextFFT = hopSize;
  }

  // ========================================================================
  // Set band parameters
  // ========================================================================
  void setParameters(double freqLow, double freqHigh, double threshold,
                     double ratio, double attack, double release,
                     bool isCompressor = true) {
    this->freqLow = freqLow;
    this->freqHigh = freqHigh;
    this->thresholdDb = threshold;
    this->ratio = ratio;
    this->isCompressor = isCompressor;

    // Calculate time constants
    if (sampleRate > 0) {
      attackCoeff = std::exp(-1.0 / (attack * 0.001 * sampleRate / hopSize));
      releaseCoeff = std::exp(-1.0 / (release * 0.001 * sampleRate / hopSize));
    }

    // Calculate bin range
    double binWidth = sampleRate / fftSize;
    binLow = static_cast<int>(std::max(1.0, freqLow / binWidth));
    binHigh = static_cast<int>(
        std::min(static_cast<double>(fftSize / 2), freqHigh / binWidth));
  }

  // ========================================================================
  // Process audio block
  // ========================================================================
  void process(float *data, int numSamples) {
    for (int i = 0; i < numSamples; ++i) {
      // Write input sample to circular buffer
      inputBuffer[inputWritePos] = data[i];
      inputWritePos = (inputWritePos + 1) % fftSize;

      // Output from overlap buffer
      data[i] = overlapBuffer[outputReadPos];
      overlapBuffer[outputReadPos] = 0.0f; // Clear for next overlap
      outputReadPos = (outputReadPos + 1) % fftSize;

      // Check if it's time for FFT
      samplesUntilNextFFT--;
      if (samplesUntilNextFFT <= 0) {
        processFFTFrame();
        samplesUntilNextFFT = hopSize;
      }
    }
  }

  // ========================================================================
  // Get latency in samples
  // ========================================================================
  int getLatencySamples() const { return fftSize; }

private:
  // ========================================================================
  // Process one FFT frame
  // ========================================================================
  void processFFTFrame() {
    // Copy input to FFT buffer with window
    for (int i = 0; i < fftSize; ++i) {
      int idx = (inputWritePos + i) % fftSize;
      fftData[i] = inputBuffer[idx] * window[i];
    }

    // Clear imaginary part
    for (int i = fftSize; i < fftSize * 2; ++i) {
      fftData[i] = 0.0f;
    }

    // Forward FFT
    fft->performRealOnlyForwardTransform(fftData.data());

    // Process frequency bins within band range
    for (int bin = binLow; bin <= binHigh && bin <= fftSize / 2; ++bin) {
      int realIdx = bin * 2;
      int imagIdx = bin * 2 + 1;

      float real = fftData[realIdx];
      float imag = fftData[imagIdx];

      // Calculate magnitude
      float magnitude = std::sqrt(real * real + imag * imag);
      float magnitudeDb = 20.0f * std::log10(magnitude + 1e-10f);

      // Envelope follower for this bin
      float targetEnv = magnitudeDb;
      if (targetEnv > binEnvelopes[bin]) {
        binEnvelopes[bin] =
            attackCoeff * binEnvelopes[bin] + (1.0f - attackCoeff) * targetEnv;
      } else {
        binEnvelopes[bin] = releaseCoeff * binEnvelopes[bin] +
                            (1.0f - releaseCoeff) * targetEnv;
      }

      // Compute gain
      float envDb = binEnvelopes[bin];
      float gainDb = 0.0f;

      if (isCompressor) {
        // Compression: reduce gain when above threshold
        if (envDb > thresholdDb) {
          float excess = envDb - thresholdDb;
          float reduction = excess * (1.0f - 1.0f / ratio);
          gainDb = -reduction;
        }
      } else {
        // Expansion: increase gain when above threshold
        if (envDb > thresholdDb) {
          float excess = envDb - thresholdDb;
          float boost = excess * (ratio - 1.0f);
          gainDb = boost;
        }
      }

      // Limit gain change
      gainDb = std::max(-24.0f, std::min(24.0f, gainDb));

      // Apply gain
      float gainLinear = std::pow(10.0f, gainDb / 20.0f);
      fftData[realIdx] *= gainLinear;
      fftData[imagIdx] *= gainLinear;
    }

    // Inverse FFT
    fft->performRealOnlyInverseTransform(fftData.data());

    // Overlap-add with window
    for (int i = 0; i < fftSize; ++i) {
      int outIdx = (outputReadPos + i) % fftSize;
      overlapBuffer[outIdx] += fftData[i] * window[i] * windowGain;
    }
  }

  // FFT components
  std::unique_ptr<juce::dsp::FFT> fft;
  int fftOrder = 11;
  int fftSize = 2048;
  int hopSize = 1024;

  // Buffers
  std::vector<float> inputBuffer;
  std::vector<float> outputBuffer;
  std::vector<float> fftData;
  std::vector<float> window;
  std::vector<float> overlapBuffer;
  std::vector<float> binEnvelopes;

  // Positions
  int inputWritePos = 0;
  int outputReadPos = 0;
  int samplesUntilNextFFT = 0;

  // Parameters
  double sampleRate = 44100.0;
  SpectralQuality quality = SpectralQuality::Normal;
  double freqLow = 20.0;
  double freqHigh = 20000.0;
  double thresholdDb = -20.0;
  double ratio = 2.0;
  float attackCoeff = 0.9f;
  float releaseCoeff = 0.99f;
  bool isCompressor = true;
  int binLow = 1;
  int binHigh = 1024;
  float windowGain = 1.0f;
};

} // namespace Sphere
