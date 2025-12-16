/*
  ==============================================================================
    SphereEQOversampler.h
    Polyphase Half-Band Filter Oversampler for Natural Phase EQ

    Implements 2x and 4x oversampling using efficient half-band filters
    to reduce bilinear transform warping at high frequencies.
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <array>
#include <cmath>
#include <vector>

namespace Sphere {

// ============================================================================
// Half-Band Filter Coefficients (Kaiser window designed)
// These are symmetric FIR coefficients for 2x oversampling
// ============================================================================
namespace HalfBandCoeffs {
// 23-tap half-band filter (~80dB stopband attenuation)
constexpr int HALFBAND_LENGTH = 23;
constexpr int HALFBAND_CENTER = HALFBAND_LENGTH / 2;

// Only non-zero coefficients (every other coeff is 0 except center)
// Designed for 0.45 transition band
constexpr double coeffs23[12] = {0.0009172814793656,
                                 -0.0040093622099168,
                                 0.0117419510683846,
                                 -0.0278346501791932,
                                 0.0580828286329551,
                                 -0.1142768898169041,
                                 0.2941547987002074,
                                 0.5, // center tap
                                 -0.1142768898169041,
                                 0.0580828286329551,
                                 -0.0278346501791932,
                                 0.0117419510683846};

// Higher quality 47-tap filter (~120dB stopband)
constexpr int HALFBAND_HQ_LENGTH = 47;
constexpr int HALFBAND_HQ_CENTER = HALFBAND_HQ_LENGTH / 2;
} // namespace HalfBandCoeffs

// ============================================================================
// Polyphase Half-Band Filter
// ============================================================================
class HalfBandFilter {
public:
  HalfBandFilter() { reset(); }

  void reset() {
    std::fill(delayLine.begin(), delayLine.end(), 0.0);
    writeIndex = 0;
  }

  // Process single sample for upsampling (insert zeros, filter)
  // Returns two samples: interpolated and original
  std::pair<double, double> processUpsample(double input) {
    // Insert input into delay line
    delayLine[writeIndex] = input;

    // Calculate output using polyphase decomposition
    // For half-band, we can exploit symmetry
    double sum = 0.0;

    // Center tap
    int centerIdx =
        (writeIndex - HalfBandCoeffs::HALFBAND_CENTER + delayLine.size()) %
        delayLine.size();
    double centerSample = delayLine[centerIdx] * 0.5;

    // Symmetric taps (only odd indices have non-zero coeffs)
    for (int i = 0; i < 6; ++i) {
      int tapOffset = (i * 2) + 1;
      int idx1 = (writeIndex - HalfBandCoeffs::HALFBAND_CENTER + tapOffset +
                  delayLine.size()) %
                 delayLine.size();
      int idx2 = (writeIndex - HalfBandCoeffs::HALFBAND_CENTER - tapOffset +
                  delayLine.size()) %
                 delayLine.size();

      double coeff = HalfBandCoeffs::coeffs23[i];
      sum += (delayLine[idx1] + delayLine[idx2]) * coeff;
    }

    // Advance write position
    writeIndex = (writeIndex + 1) % delayLine.size();

    // Return interpolated sample and direct sample
    return {sum + centerSample, input};
  }

  // Process for downsampling (filter then decimate)
  double processDownsample(double sample1, double sample2) {
    // Process both samples through filter
    delayLine[writeIndex] = sample1;
    writeIndex = (writeIndex + 1) % delayLine.size();
    delayLine[writeIndex] = sample2;

    // Calculate filtered output
    double sum = 0.0;

    int centerIdx =
        (writeIndex - HalfBandCoeffs::HALFBAND_CENTER + delayLine.size()) %
        delayLine.size();
    double centerSample = delayLine[centerIdx] * 0.5;

    for (int i = 0; i < 6; ++i) {
      int tapOffset = (i * 2) + 1;
      int idx1 = (writeIndex - HalfBandCoeffs::HALFBAND_CENTER + tapOffset +
                  delayLine.size()) %
                 delayLine.size();
      int idx2 = (writeIndex - HalfBandCoeffs::HALFBAND_CENTER - tapOffset +
                  delayLine.size()) %
                 delayLine.size();

      double coeff = HalfBandCoeffs::coeffs23[i];
      sum += (delayLine[idx1] + delayLine[idx2]) * coeff;
    }

    writeIndex = (writeIndex + 1) % delayLine.size();

    return (sum + centerSample) * 2.0; // Gain compensation
  }

private:
  std::array<double, HalfBandCoeffs::HALFBAND_LENGTH> delayLine;
  int writeIndex = 0;
};

// ============================================================================
// Stereo Oversampler (2x or 4x)
// ============================================================================
class SphereEQOversampler {
public:
  enum class Factor { None = 1, X2 = 2, X4 = 4 };

  SphereEQOversampler() = default;
  ~SphereEQOversampler() = default;

  void prepare(double sampleRate, int numChannels, int maxBlockSize) {
    this->baseSampleRate = sampleRate;
    this->numChannels = numChannels;
    this->maxBlockSize = maxBlockSize;

    // Allocate upsampled buffers (max 4x)
    upsampledBuffer.setSize(numChannels, maxBlockSize * 4);
    tempBuffer.setSize(numChannels, maxBlockSize * 2);

    // Reset filters
    for (auto &stage : upsampleFilters) {
      for (auto &filter : stage) {
        filter.reset();
      }
    }
    for (auto &stage : downsampleFilters) {
      for (auto &filter : stage) {
        filter.reset();
      }
    }
  }

  void reset() {
    for (auto &stage : upsampleFilters) {
      for (auto &filter : stage) {
        filter.reset();
      }
    }
    for (auto &stage : downsampleFilters) {
      for (auto &filter : stage) {
        filter.reset();
      }
    }
  }

  void setOversamplingFactor(Factor factor) {
    if (currentFactor != factor) {
      currentFactor = factor;
      reset();
    }
  }

  Factor getOversamplingFactor() const { return currentFactor; }

  double getOversampledSampleRate() const {
    return baseSampleRate * static_cast<int>(currentFactor);
  }

  int getLatencySamples() const {
    // Half-band filter latency
    switch (currentFactor) {
    case Factor::X2:
      return HalfBandCoeffs::HALFBAND_CENTER;
    case Factor::X4:
      return HalfBandCoeffs::HALFBAND_CENTER * 2;
    default:
      return 0;
    }
  }

  // Process with callback at oversampled rate
  template <typename ProcessCallback>
  void process(juce::AudioBuffer<float> &buffer, ProcessCallback &&callback) {
    if (currentFactor == Factor::None) {
      callback(buffer);
      return;
    }

    const int numSamples = buffer.getNumSamples();
    const int channels = buffer.getNumChannels();
    const int oversampledLength = numSamples * static_cast<int>(currentFactor);

    // Ensure buffer is large enough
    if (upsampledBuffer.getNumSamples() < oversampledLength) {
      upsampledBuffer.setSize(channels, oversampledLength, false, false, true);
    }

    // Upsample
    if (currentFactor == Factor::X2) {
      upsample2x(buffer, upsampledBuffer, numSamples);
    } else if (currentFactor == Factor::X4) {
      // First stage: 2x
      // Use pre-allocated temp buffer
      // juce::AudioBuffer<float> temp2x(channels, numSamples * 2);
      // We need to ensure tempBuffer points to the right size, but setSize with
      // keepExisting content=false is cheap if capacity is enough Actually, we
      // can just use the pointers if we trust the size. But to be safe and use
      // JUCE API: tempBuffer is maxBlockSize * 2. numSamples * 2 <=
      // maxBlockSize * 2.

      // Create a wrapper for the current block size without allocation
      juce::AudioBuffer<float> temp2x(tempBuffer.getArrayOfWritePointers(),
                                      channels, numSamples * 2);

      upsample2x(buffer, temp2x, numSamples);
      // Second stage: 2x (total 4x)
      upsample2xSecondStage(temp2x, upsampledBuffer, numSamples * 2);
    }

    // Process at oversampled rate
    juce::AudioBuffer<float> processBuffer(
        upsampledBuffer.getArrayOfWritePointers(), channels, oversampledLength);
    callback(processBuffer);

    // Downsample
    if (currentFactor == Factor::X2) {
      downsample2x(upsampledBuffer, buffer, numSamples);
    } else if (currentFactor == Factor::X4) {
      // juce::AudioBuffer<float> temp2x(channels, numSamples * 2);
      juce::AudioBuffer<float> temp2x(tempBuffer.getArrayOfWritePointers(),
                                      channels, numSamples * 2);

      // First stage: 4x -> 2x
      downsample2xSecondStage(upsampledBuffer, temp2x, numSamples * 2);
      // Second stage: 2x -> 1x
      downsample2x(temp2x, buffer, numSamples);
    }
  }

private:
  void upsample2x(const juce::AudioBuffer<float> &input,
                  juce::AudioBuffer<float> &output, int numSamples) {
    for (int ch = 0; ch < input.getNumChannels(); ++ch) {
      const float *in = input.getReadPointer(ch);
      float *out = output.getWritePointer(ch);

      for (int i = 0; i < numSamples; ++i) {
        auto [interp, direct] = upsampleFilters[0][ch].processUpsample(in[i]);
        out[i * 2] = static_cast<float>(interp);
        out[i * 2 + 1] = static_cast<float>(direct);
      }
    }
  }

  void upsample2xSecondStage(const juce::AudioBuffer<float> &input,
                             juce::AudioBuffer<float> &output, int numSamples) {
    for (int ch = 0; ch < input.getNumChannels(); ++ch) {
      const float *in = input.getReadPointer(ch);
      float *out = output.getWritePointer(ch);

      for (int i = 0; i < numSamples; ++i) {
        auto [interp, direct] = upsampleFilters[1][ch].processUpsample(in[i]);
        out[i * 2] = static_cast<float>(interp);
        out[i * 2 + 1] = static_cast<float>(direct);
      }
    }
  }

  void downsample2x(const juce::AudioBuffer<float> &input,
                    juce::AudioBuffer<float> &output, int numSamples) {
    for (int ch = 0; ch < input.getNumChannels(); ++ch) {
      const float *in = input.getReadPointer(ch);
      float *out = output.getWritePointer(ch);

      for (int i = 0; i < numSamples; ++i) {
        out[i] = static_cast<float>(downsampleFilters[0][ch].processDownsample(
            in[i * 2], in[i * 2 + 1]));
      }
    }
  }

  void downsample2xSecondStage(const juce::AudioBuffer<float> &input,
                               juce::AudioBuffer<float> &output,
                               int numSamples) {
    for (int ch = 0; ch < input.getNumChannels(); ++ch) {
      const float *in = input.getReadPointer(ch);
      float *out = output.getWritePointer(ch);

      for (int i = 0; i < numSamples; ++i) {
        out[i] = static_cast<float>(downsampleFilters[1][ch].processDownsample(
            in[i * 2], in[i * 2 + 1]));
      }
    }
  }

  Factor currentFactor = Factor::None;
  double baseSampleRate = 44100.0;
  int numChannels = 2;
  int maxBlockSize = 512;

  juce::AudioBuffer<float> upsampledBuffer;
  juce::AudioBuffer<float> tempBuffer;

  // Two stages for 4x (each stage is 2x)
  // [stage][channel]
  std::array<std::array<HalfBandFilter, 2>, 2> upsampleFilters;
  std::array<std::array<HalfBandFilter, 2>, 2> downsampleFilters;
};

} // namespace Sphere
