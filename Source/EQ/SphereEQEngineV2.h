#pragma once
#include "SphereEQAnalyzer.h"
#include "SphereEQBandProcessor.h"
#include "SphereEQLinearPhase.h"
#include "SphereEQOversampler.h"
#include <array>
#include <atomic>
#include <vector>

namespace Sphere {

// ============================================================================
// Lock-free parameter snapshot for RT-safe updates
// ============================================================================
struct EQParameterSnapshot {
  std::array<EQBandParams, MAX_EQ_BANDS> bandParams;
  bool enabled = true;
  float outputGainLinear = 1.0f;
  EQPhaseMode globalPhaseMode = EQPhaseMode::MinimumPhase;
  LinearPhaseLength linearPhaseLength = LinearPhaseLength::Medium;
  SphereEQOversampler::Factor oversampleFactor =
      SphereEQOversampler::Factor::X2;
  EQCharacterMode globalCharacterMode = EQCharacterMode::Clean;

  // Active band indices for optimized iteration
  std::vector<int> activeBandIndices;
  std::vector<int> midModeBandIndices;
  std::vector<int> sideModeBandIndices;

  void rebuildActiveIndices() {
    activeBandIndices.clear();
    midModeBandIndices.clear();
    sideModeBandIndices.clear();

    for (int i = 0; i < MAX_EQ_BANDS; ++i) {
      if (!bandParams[i].bypass) {
        activeBandIndices.push_back(i);

        if (bandParams[i].stereoMode == EQStereoMode::Mid) {
          midModeBandIndices.push_back(i);
        } else if (bandParams[i].stereoMode == EQStereoMode::Side) {
          sideModeBandIndices.push_back(i);
        }
      }
    }
  }
};

// ============================================================================
// 24-Band Parametric EQ Engine with Phase Mode Support
// ============================================================================
class SphereEQEngineV2 {
public:
  SphereEQEngineV2() {
    // Initialize snapshots
    for (int i = 0; i < MAX_EQ_BANDS; ++i) {
      paramSnapshots[0].bandParams[i].bypass = true;
      paramSnapshots[1].bandParams[i].bypass = true;
    }
    currentSnapshot.store(0);
    pendingSnapshot.store(-1);
  }

  // ========================================================================
  // Prepare for playback
  // ========================================================================
  void prepare(double sampleRate, int maxBlockSize, int numChannels) {
    this->sampleRate = sampleRate;
    this->maxBlockSize = maxBlockSize;
    this->numChannels = juce::jlimit(1, 2, numChannels);

    // Prepare all band processors with sample rate
    for (int i = 0; i < MAX_EQ_BANDS; ++i) {
      bands[i].prepare(sampleRate, maxBlockSize);
    }

    // Prepare oversampler for Natural Phase mode
    oversampler.prepare(sampleRate, this->numChannels, maxBlockSize);

    // Prepare linear phase EQ
    linearPhaseEQ.prepare(sampleRate, maxBlockSize, LinearPhaseLength::Medium);

    // Prepare oversampled band processors (at 2x or 4x rate)
    for (int i = 0; i < MAX_EQ_BANDS; ++i) {
      oversampledBands[i].prepare(sampleRate * 2.0, maxBlockSize * 4);
    }

    // Allocate M/S working buffers
    midBuffer.resize(maxBlockSize * 4); // Extra for oversampling
    sideBuffer.resize(maxBlockSize * 4);

    // Setup output gain smoother
    outputGainSmoother.reset(sampleRate, 0.05);
    outputGainSmoother.setCurrentAndTargetValue(1.0f);

    prepared = true;

    DBG("EQ Engine V2 prepared: sampleRate=" + juce::String(sampleRate) +
        " blockSize=" + juce::String(maxBlockSize));

    inputAnalyzer.prepare(sampleRate);
    outputAnalyzer.prepare(sampleRate);
  }

  // ========================================================================
  // Reset state
  // ========================================================================
  void reset() {
    for (int i = 0; i < MAX_EQ_BANDS; ++i) {
      bands[i].reset();
      oversampledBands[i].reset();
    }
    oversampler.reset();
    linearPhaseEQ.reset();
    outputGainSmoother.setCurrentAndTargetValue(1.0f);
  }

  // ========================================================================
  // Process Audio Block
  // ========================================================================
  void processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midi) {
    // Handle parameter updates
    int pending = pendingSnapshot.load(std::memory_order_acquire);
    if (pending != -1) {
      currentSnapshot.store(pending, std::memory_order_release);
      pendingSnapshot.store(-1, std::memory_order_release);
    }

    const auto &snapshot =
        paramSnapshots[currentSnapshot.load(std::memory_order_relaxed)];

    if (!snapshot.enabled)
      return;

    juce::ScopedNoDenormals noDenormals;

    // Analyze Input
    if (buffer.getNumChannels() > 0) {
      const float *inL = buffer.getReadPointer(0);
      const float *inR =
          (buffer.getNumChannels() > 1) ? buffer.getReadPointer(1) : inL;
      for (int i = 0; i < buffer.getNumSamples(); ++i) {
        // Mix to mono for analysis
        inputAnalyzer.pushSample((inL[i] + inR[i]) * 0.5f);
      }
    }

    // Route to appropriate processing path based on phase mode
    switch (snapshot.globalPhaseMode) {
    case EQPhaseMode::MinimumPhase:
      processMinimumPhase(buffer, snapshot);
      break;

    case EQPhaseMode::NaturalPhase:
      processNaturalPhase(buffer, snapshot);
      break;

    case EQPhaseMode::LinearPhase:
      processLinearPhase(buffer, snapshot);
      break;
    }

    // Apply Analog Character Saturation (if not Clean)
    // For Natural Phase, saturation is applied inside the loop (oversampled).
    // For Min/Linear Phase, we need to apply it here, potentially
    // oversampled.
    if (snapshot.globalCharacterMode != EQCharacterMode::Clean &&
        snapshot.globalPhaseMode != EQPhaseMode::NaturalPhase) {
      // Use oversampler for saturation to avoid aliasing
      oversampler.process(buffer, [this, mode = snapshot.globalCharacterMode](
                                      juce::AudioBuffer<float> &buf) {
        applySaturation(buf, mode);
      });
    }

    applyOutputGain(buffer, snapshot.outputGainLinear);

    // Analyze Output
    if (buffer.getNumChannels() > 0) {
      const float *outL = buffer.getReadPointer(0);
      const float *outR =
          (buffer.getNumChannels() > 1) ? buffer.getReadPointer(1) : outL;
      for (int i = 0; i < buffer.getNumSamples(); ++i) {
        outputAnalyzer.pushSample((outL[i] + outR[i]) * 0.5f);
      }
    }
  }

  // ========================================================================
  // Phase Mode Settings
  // ========================================================================
  void setPhaseMode(EQPhaseMode mode) {
    int activeIdx = currentSnapshot.load(std::memory_order_relaxed);
    int writeIdx = 1 - activeIdx;
    paramSnapshots[writeIdx] = paramSnapshots[activeIdx];
    paramSnapshots[writeIdx].globalPhaseMode = mode;
    pendingSnapshot.store(writeIdx, std::memory_order_release);
  }

  EQPhaseMode getPhaseMode() const {
    return paramSnapshots[currentSnapshot.load(std::memory_order_relaxed)]
        .globalPhaseMode;
  }

  void setOversampleFactor(SphereEQOversampler::Factor factor) {
    int activeIdx = currentSnapshot.load(std::memory_order_relaxed);
    int writeIdx = 1 - activeIdx;
    paramSnapshots[writeIdx] = paramSnapshots[activeIdx];
    paramSnapshots[writeIdx].oversampleFactor = factor;
    pendingSnapshot.store(writeIdx, std::memory_order_release);
  }

  void setLinearPhaseLength(LinearPhaseLength length) {
    int activeIdx = currentSnapshot.load(std::memory_order_relaxed);
    int writeIdx = 1 - activeIdx;
    paramSnapshots[writeIdx] = paramSnapshots[activeIdx];
    paramSnapshots[writeIdx].linearPhaseLength = length;
    pendingSnapshot.store(writeIdx, std::memory_order_release);
  }

  void setGlobalCharacterMode(EQCharacterMode mode) {
    int activeIdx = currentSnapshot.load(std::memory_order_relaxed);
    int writeIdx = 1 - activeIdx;
    paramSnapshots[writeIdx] = paramSnapshots[activeIdx];
    paramSnapshots[writeIdx].globalCharacterMode = mode;
    pendingSnapshot.store(writeIdx, std::memory_order_release);
  }

  // ========================================================================
  // Latency Reporting (for host compensation)
  // ========================================================================
  int getLatencySamples() const {
    auto &snapshot =
        paramSnapshots[currentSnapshot.load(std::memory_order_relaxed)];

    int latency = 0;
    switch (snapshot.globalPhaseMode) {
    case EQPhaseMode::MinimumPhase:
      latency = 0; // No latency
      break;

    case EQPhaseMode::NaturalPhase:
      latency = oversampler.getLatencySamples();
      break;

    case EQPhaseMode::LinearPhase:
      latency = linearPhaseEQ.getLatencySamples();
      break;

    default:
      return 0;
    }

    // Add latency if saturation is enabled in non-Natural modes (since we use
    // oversampler)
    if (snapshot.globalCharacterMode != EQCharacterMode::Clean &&
        snapshot.globalPhaseMode != EQPhaseMode::NaturalPhase) {
      return latency + oversampler.getLatencySamples();
    }

    return latency;
  }

  // ========================================================================
  // Parameter updates (lock-free)
  // ========================================================================
  void setBandParameters(int bandIndex, const EQBandParams &params) {
    if (bandIndex < 0 || bandIndex >= MAX_EQ_BANDS)
      return;

    int pending = pendingSnapshot.load(std::memory_order_acquire);
    int activeIdx = currentSnapshot.load(std::memory_order_relaxed);
    int writeIdx = 1 - activeIdx;

    if (pending != writeIdx) {
      paramSnapshots[writeIdx] = paramSnapshots[activeIdx];
    }

    paramSnapshots[writeIdx].bandParams[bandIndex] = params;
    paramSnapshots[writeIdx].rebuildActiveIndices();

    if (prepared) {
      bands[bandIndex].setParametersFromSnapshot(params);
      oversampledBands[bandIndex].setParametersFromSnapshot(params);
    }

    pendingSnapshot.store(writeIdx, std::memory_order_release);
  }

  const EQBandParams &getBandParameters(int bandIndex) const {
    if (bandIndex < 0 || bandIndex >= MAX_EQ_BANDS) {
      static EQBandParams defaultParams;
      return defaultParams;
    }
    return paramSnapshots[currentSnapshot.load(std::memory_order_relaxed)]
        .bandParams[bandIndex];
  }

  // Convenience setters
  void setBandType(int bandIndex, EQFilterType type) {
    if (bandIndex < 0 || bandIndex >= MAX_EQ_BANDS)
      return;
    auto params = getBandParameters(bandIndex);
    params.type = type;
    setBandParameters(bandIndex, params);
  }

  void setBandFrequency(int bandIndex, double frequency) {
    if (bandIndex < 0 || bandIndex >= MAX_EQ_BANDS)
      return;
    auto params = getBandParameters(bandIndex);
    params.frequency = juce::jlimit(MIN_FREQUENCY, MAX_FREQUENCY, frequency);
    setBandParameters(bandIndex, params);
  }

  void setBandQ(int bandIndex, double q) {
    if (bandIndex < 0 || bandIndex >= MAX_EQ_BANDS)
      return;
    auto params = getBandParameters(bandIndex);
    params.q = juce::jlimit(MIN_Q, MAX_Q, q);
    setBandParameters(bandIndex, params);
  }

  void setBandGain(int bandIndex, double gainDb) {
    if (bandIndex < 0 || bandIndex >= MAX_EQ_BANDS)
      return;
    auto params = getBandParameters(bandIndex);
    params.gainDb = juce::jlimit(MIN_GAIN_DB, MAX_GAIN_DB, gainDb);
    setBandParameters(bandIndex, params);
  }

  void setBandSlope(int bandIndex, EQSlope slope) {
    if (bandIndex < 0 || bandIndex >= MAX_EQ_BANDS)
      return;
    auto params = getBandParameters(bandIndex);
    params.slope = slope;
    setBandParameters(bandIndex, params);
  }

  void setBandBypass(int bandIndex, bool bypass) {
    if (bandIndex < 0 || bandIndex >= MAX_EQ_BANDS)
      return;
    auto params = getBandParameters(bandIndex);
    params.bypass = bypass;
    setBandParameters(bandIndex, params);
  }

  void setBandStereoMode(int bandIndex, EQStereoMode mode) {
    if (bandIndex < 0 || bandIndex >= MAX_EQ_BANDS)
      return;
    auto params = getBandParameters(bandIndex);
    params.stereoMode = mode;
    setBandParameters(bandIndex, params);
  }

  void setBandDynamicMode(int bandIndex, EQDynamicMode mode) {
    if (bandIndex < 0 || bandIndex >= MAX_EQ_BANDS)
      return;
    auto params = getBandParameters(bandIndex);
    params.dynamicMode = mode;
    setBandParameters(bandIndex, params);
  }

  void setBandDynamicParams(int bandIndex, double threshold, double ratio,
                            double attack, double release, double range) {
    if (bandIndex < 0 || bandIndex >= MAX_EQ_BANDS)
      return;
    auto params = getBandParameters(bandIndex);
    params.dynamicThreshold = threshold;
    params.dynamicRatio = ratio;
    params.dynamicAttack = attack;
    params.dynamicRelease = release;
    params.dynamicRange = range;
    setBandParameters(bandIndex, params);
  }

  void setBandCharacter(int bandIndex, EQCharacterMode mode) {
    if (bandIndex < 0 || bandIndex >= MAX_EQ_BANDS)
      return;
    auto params = getBandParameters(bandIndex);
    params.characterMode = mode;
    setBandParameters(bandIndex, params);
  }

  void setEnabled(bool shouldEnable) {
    int activeIdx = currentSnapshot.load(std::memory_order_relaxed);
    int writeIdx = 1 - activeIdx;
    paramSnapshots[writeIdx] = paramSnapshots[activeIdx];
    paramSnapshots[writeIdx].enabled = shouldEnable;
    pendingSnapshot.store(writeIdx, std::memory_order_release);
  }

  bool isEnabled() const {
    return paramSnapshots[currentSnapshot.load(std::memory_order_relaxed)]
        .enabled;
  }

  void setOutputGain(double gainDb) {
    float linear = static_cast<float>(
        std::pow(10.0, juce::jlimit(-24.0, 24.0, gainDb) / 20.0));
    int activeIdx = currentSnapshot.load(std::memory_order_relaxed);
    int writeIdx = 1 - activeIdx;
    paramSnapshots[writeIdx] = paramSnapshots[activeIdx];
    paramSnapshots[writeIdx].outputGainLinear = linear;
    pendingSnapshot.store(writeIdx, std::memory_order_release);
  }

  // ========================================================================
  // Analysis
  // ========================================================================
  double getMagnitudeResponse(double frequency) const {
    auto &snapshot =
        paramSnapshots[currentSnapshot.load(std::memory_order_relaxed)];
    if (!snapshot.enabled)
      return 1.0;

    double totalMagnitude = 1.0;
    for (int idx : snapshot.activeBandIndices) {
      totalMagnitude *= bands[idx].getMagnitudeResponse(frequency);
    }
    return totalMagnitude * snapshot.outputGainLinear;
  }

  double getMagnitudeResponseDb(double frequency) const {
    double mag = getMagnitudeResponse(frequency);
    if (mag <= 0.0)
      return -100.0;
    return 20.0 * std::log10(mag);
  }

  int getActiveBandCount() const {
    return static_cast<int>(
        paramSnapshots[currentSnapshot.load()].activeBandIndices.size());
  }

  // Analyzer Access
  const SphereEQAnalyzer &getInputAnalyzer() const { return inputAnalyzer; }
  const SphereEQAnalyzer &getOutputAnalyzer() const { return outputAnalyzer; }

private:
  // ========================================================================
  // Saturation Helper
  // ========================================================================
  void applySaturation(juce::AudioBuffer<float> &buffer, EQCharacterMode mode) {
    if (mode == EQCharacterMode::Clean)
      return;

    const int numSamples = buffer.getNumSamples();
    const int numChans = buffer.getNumChannels();

    for (int ch = 0; ch < numChans; ++ch) {
      float *data = buffer.getWritePointer(ch);
      for (int i = 0; i < numSamples; ++i) {
        float x = data[i];
        // Simple input drive (boost slightly to drive saturation)
        x *= 1.1f;

        if (mode == EQCharacterMode::Subtle) {
          // Gentle tanh-like curve
          data[i] = std::tanh(x);
        } else if (mode == EQCharacterMode::Warm) {
          // Stronger, asymmetric
          // tanh(x) + 0.15 * x^2
          float sat = std::tanh(x);
          // Add even harmonics
          data[i] = sat + 0.15f * x * x;
        }

        // Soft clip to prevent explosion if gain is high
        if (data[i] > 1.5f)
          data[i] = 1.5f;
        if (data[i] < -1.5f)
          data[i] = -1.5f;
      }
    }
  }

  // ========================================================================
  // Minimum Phase Processing (Zero Latency IIR)
  // ========================================================================
  void processMinimumPhase(juce::AudioBuffer<float> &buffer,
                           const EQParameterSnapshot &snapshot) {
    const int numSamples = buffer.getNumSamples();
    const int numChans = buffer.getNumChannels();

    if (numChans == 0 || numSamples == 0)
      return;

    float *leftChannel = buffer.getWritePointer(0);
    float *rightChannel = (numChans > 1) ? buffer.getWritePointer(1) : nullptr;

    // Update filters
    for (int idx : snapshot.activeBandIndices) {
      bands[idx].updateIfNeeded();
    }

    // Process M/S bands
    if (!snapshot.midModeBandIndices.empty() ||
        !snapshot.sideModeBandIndices.empty()) {
      processMidSideBands(leftChannel, rightChannel, numSamples, snapshot,
                          bands);
    }

    // Process regular bands
    for (int idx : snapshot.activeBandIndices) {
      auto mode = snapshot.bandParams[idx].stereoMode;
      if (mode != EQStereoMode::Mid && mode != EQStereoMode::Side) {
        if (rightChannel) {
          bands[idx].processBlock(leftChannel, rightChannel, numSamples);
        } else {
          bands[idx].processBlock(leftChannel, leftChannel, numSamples);
        }
      }
    }
  }

  // ========================================================================
  // Natural Phase Processing (Oversampled IIR)
  // ========================================================================
  void processNaturalPhase(juce::AudioBuffer<float> &buffer,
                           const EQParameterSnapshot &snapshot) {
    // Process at oversampled rate for better analog matching
    oversampler.process(
        buffer, [this, &snapshot](juce::AudioBuffer<float> &oversampledBuffer) {
          const int numSamples = oversampledBuffer.getNumSamples();
          const int numChans = oversampledBuffer.getNumChannels();

          if (numChans == 0 || numSamples == 0)
            return;

          float *leftChannel = oversampledBuffer.getWritePointer(0);
          float *rightChannel =
              (numChans > 1) ? oversampledBuffer.getWritePointer(1) : nullptr;

          // Use oversampled band processors
          for (int idx : snapshot.activeBandIndices) {
            oversampledBands[idx].updateIfNeeded();
          }

          // Process M/S bands with oversampled processors
          if (!snapshot.midModeBandIndices.empty() ||
              !snapshot.sideModeBandIndices.empty()) {
            processMidSideBands(leftChannel, rightChannel, numSamples, snapshot,
                                oversampledBands);
          }

          // Process regular bands
          for (int idx : snapshot.activeBandIndices) {
            auto mode = snapshot.bandParams[idx].stereoMode;
            if (mode != EQStereoMode::Mid && mode != EQStereoMode::Side) {
              if (rightChannel) {
                oversampledBands[idx].processBlock(leftChannel, rightChannel,
                                                   numSamples);
              } else {
                oversampledBands[idx].processBlock(leftChannel, leftChannel,
                                                   numSamples);
              }
            }
          }

          // Apply saturation inside the oversampled loop
          if (snapshot.globalCharacterMode != EQCharacterMode::Clean) {
            applySaturation(oversampledBuffer, snapshot.globalCharacterMode);
          }
        });
  }

  // ========================================================================
  // Linear Phase Processing (FFT Convolution)
  // ========================================================================
  void processLinearPhase(juce::AudioBuffer<float> &buffer,
                          const EQParameterSnapshot & /*snapshot*/) {
    linearPhaseEQ.processBlock(buffer);
  }

  // ========================================================================
  // Shared M/S conversion and processing
  // ========================================================================
  template <typename BandArray>
  void processMidSideBands(float *left, float *right, int numSamples,
                           const EQParameterSnapshot &snapshot,
                           BandArray &bandProcessors) {
    if (!right)
      return;

    // Ensure buffer size
    if (midBuffer.size() < static_cast<size_t>(numSamples)) {
      midBuffer.resize(numSamples);
      sideBuffer.resize(numSamples);
    }

    // Convert L/R to M/S
    for (int i = 0; i < numSamples; ++i) {
      midBuffer[i] = (left[i] + right[i]) * 0.5f;
      sideBuffer[i] = (left[i] - right[i]) * 0.5f;
    }

    // Process all Mid bands
    for (int idx : snapshot.midModeBandIndices) {
      bandProcessors[idx].processMidBuffer(midBuffer.data(), numSamples);
    }

    // Process all Side bands
    for (int idx : snapshot.sideModeBandIndices) {
      bandProcessors[idx].processSideBuffer(sideBuffer.data(), numSamples);
    }

    // Convert M/S back to L/R
    for (int i = 0; i < numSamples; ++i) {
      left[i] = midBuffer[i] + sideBuffer[i];
      right[i] = midBuffer[i] - sideBuffer[i];
    }
  }

  // ========================================================================
  // Output gain with smoothing
  // ========================================================================
  void applyOutputGain(juce::AudioBuffer<float> &buffer, float targetGain) {
    outputGainSmoother.setTargetValue(targetGain);

    if (outputGainSmoother.isSmoothing()) {
      const int numSamples = buffer.getNumSamples();
      const int numChans = buffer.getNumChannels();

      for (int i = 0; i < numSamples; ++i) {
        float g = outputGainSmoother.getNextValue();
        for (int ch = 0; ch < numChans; ++ch) {
          buffer.getWritePointer(ch)[i] *= g;
        }
      }
    } else {
      float currentGain = outputGainSmoother.getCurrentValue();
      if (std::abs(currentGain - 1.0f) > 0.0001f) {
        buffer.applyGain(currentGain);
      }
    }
  }

  // Band processors for minimum phase
  std::array<EQBandProcessor, MAX_EQ_BANDS> bands;

  // Band processors for oversampled (natural phase) processing
  std::array<EQBandProcessor, MAX_EQ_BANDS> oversampledBands;

  // Oversampler for natural phase
  SphereEQOversampler oversampler;

  // Linear phase FFT convolver
  LinearPhaseEQ linearPhaseEQ;

  // Double-buffered parameter snapshots
  std::array<EQParameterSnapshot, 2> paramSnapshots;
  std::atomic<int> currentSnapshot{0};
  std::atomic<int> pendingSnapshot{-1};

  // M/S working buffers
  std::vector<float> midBuffer;
  std::vector<float> sideBuffer;

  // Output gain smoother
  juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear>
      outputGainSmoother;

  // State
  double sampleRate = 44100.0;
  int maxBlockSize = 0;
  int numChannels = 2;
  bool prepared = false;

  // Spectrum Analyzers
  SphereEQAnalyzer inputAnalyzer;
  SphereEQAnalyzer outputAnalyzer;
};

} // namespace Sphere
