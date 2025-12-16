#pragma once
#include "SphereEQBiquad.h"
#include "SphereEQDynamic.h"

namespace Sphere {

// ============================================================================
// Fast math approximations for RT-safe processing
// ============================================================================
namespace FastMath {
inline float fastLog2(float x) {
  union {
    float f;
    uint32_t i;
  } vx = {x};
  float y = static_cast<float>(vx.i);
  y *= 1.1920928955078125e-7f;
  return y - 126.94269504f;
}

inline float fastLog10(float x) { return fastLog2(x) * 0.3010299957f; }

inline float fastPow2(float p) {
  float clipp = (p < -126.0f) ? -126.0f : p;
  union {
    uint32_t i;
    float f;
  } v;
  v.i = static_cast<uint32_t>((1 << 23) * (clipp + 126.94269504f));
  return v.f;
}

inline float fastPow10(float x) { return fastPow2(x * 3.321928095f); }

inline float dbToLinearFast(float db) { return fastPow10(db * 0.05f); }

inline float linearToDbFast(float linear) { return 20.0f * fastLog10(linear); }
} // namespace FastMath

// ============================================================================
// Single EQ Band Processor with Advanced Dynamic EQ
// ============================================================================
class EQBandProcessor {
public:
  EQBandProcessor() = default;

  void prepare(double sampleRate, int maxBlockSize) {
    this->sampleRate = sampleRate;
    this->maxBlockSize = maxBlockSize;

    // Prepare dynamic processor
    dynamicProcessor.prepare(sampleRate, maxBlockSize);

    // Prepare work buffers
    workBufferL.resize(maxBlockSize);
    workBufferR.resize(maxBlockSize);

    // Gain smoother for character processing
    gainSmoother.reset(sampleRate, 0.02);

    reset();
    updateFilters();
  }

  void reset() {
    filterL.reset();
    filterR.reset();
    dynamicProcessor.reset();
    gainSmoother.setCurrentAndTargetValue(1.0f);
  }

  void setParametersFromSnapshot(const EQBandParams &newParams) {
    params = newParams;
    updateFilters();
    dynamicProcessor.setParameters(params);

    DBG(juce::String("Band updated: type=") +
        juce::String(static_cast<int>(params.type)) +
        " freq=" + juce::String(params.frequency) +
        " gain=" + juce::String(params.gainDb) +
        " dynamic=" + juce::String(static_cast<int>(params.dynamicMode)) +
        " bypass=" + juce::String(params.bypass ? 1 : 0) +
        " numStages=" + juce::String(filterL.getNumStages()));
  }

  void updateIfNeeded() {
    // Currently all updates happen immediately in setParametersFromSnapshot
  }

  const EQBandParams &getParameters() const { return params; }
  bool isBypassed() const { return params.bypass; }
  EQStereoMode getStereoMode() const { return params.stereoMode; }

  // Get current gain reduction from dynamic section (for metering)
  float getGainReductionDb() const {
    return dynamicProcessor.getGainReductionDb();
  }

  // Optimized stereo processing
  void processBlock(float *leftChannel, float *rightChannel, int numSamples) {
    if (params.bypass || numSamples == 0)
      return;

    juce::ScopedNoDenormals noDenormals;

    switch (params.stereoMode) {
    case EQStereoMode::Stereo:
      processStandardStereo(leftChannel, rightChannel, numSamples);
      break;
    case EQStereoMode::Left:
      processLeftOnly(leftChannel, numSamples);
      break;
    case EQStereoMode::Right:
      processRightOnly(rightChannel, numSamples);
      break;
    case EQStereoMode::Mid:
    case EQStereoMode::Side:
      processStandardStereo(leftChannel, rightChannel, numSamples);
      break;
    }
  }

  void processMidBuffer(float *mid, int numSamples) {
    if (params.bypass || numSamples == 0)
      return;

    // Apply static EQ filter
    filterL.processBlock(mid, numSamples);

    // Apply dynamic processing if enabled
    if (params.dynamicMode != EQDynamicMode::Off) {
      dynamicProcessor.processMono(mid, numSamples);
    }

    if (params.characterMode != EQCharacterMode::Clean) {
      applyCharacter(mid, numSamples);
    }
  }

  void processSideBuffer(float *side, int numSamples) {
    if (params.bypass || numSamples == 0)
      return;

    filterR.processBlock(side, numSamples);

    if (params.dynamicMode != EQDynamicMode::Off) {
      dynamicProcessor.processMono(side, numSamples);
    }

    if (params.characterMode != EQCharacterMode::Clean) {
      applyCharacter(side, numSamples);
    }
  }

  double getMagnitudeResponse(double frequency) const {
    if (params.bypass)
      return 1.0;
    return filterL.getMagnitudeResponse(frequency, sampleRate);
  }

private:
  void updateFilters() {
    if (sampleRate <= 0.0)
      return;

    int order = 2;

    if (params.type == EQFilterType::LowCut ||
        params.type == EQFilterType::HighCut) {
      switch (params.slope) {
      case EQSlope::dB6:
        order = 1;
        break;
      case EQSlope::dB12:
        order = 2;
        break;
      case EQSlope::dB18:
        order = 3;
        break;
      case EQSlope::dB24:
        order = 4;
        break;
      case EQSlope::dB36:
        order = 6;
        break;
      case EQSlope::dB48:
        order = 8;
        break;
      case EQSlope::dB72:
        order = 12;
        break;
      case EQSlope::dB96:
        order = 16;
        break;
      }
      filterL.configureButterworth(params.type, sampleRate, params.frequency,
                                   order);
      filterR.configureButterworth(params.type, sampleRate, params.frequency,
                                   order);
    } else {
      filterL.setNumStages(1);
      filterR.setNumStages(1);

      // For dynamic EQ, the static gain is applied through the filter
      // Dynamic gain modulation happens in the dynamic processor
      double staticGain =
          (params.dynamicMode == EQDynamicMode::Off) ? params.gainDb : 0.0;

      BiquadCoeffs coeffs = RBJCookbook::calculate(
          params.type, sampleRate, params.frequency, params.q, staticGain);
      filterL.setStageCoefficients(0, coeffs);
      filterR.setStageCoefficients(0, coeffs);
    }
  }

  void processStandardStereo(float *left, float *right, int numSamples) {
    // Step 1: Apply static EQ filtering
    filterL.processBlock(left, numSamples);
    filterR.processBlock(right, numSamples);

    // Step 2: Apply dynamic processing if enabled
    if (params.dynamicMode != EQDynamicMode::Off) {
      dynamicProcessor.processBlock(left, right, numSamples);
    }

    // Step 3: Apply character/saturation
    if (params.characterMode != EQCharacterMode::Clean) {
      applyCharacter(left, numSamples);
      applyCharacter(right, numSamples);
    }
  }

  void processLeftOnly(float *left, int numSamples) {
    filterL.processBlock(left, numSamples);

    if (params.dynamicMode != EQDynamicMode::Off) {
      dynamicProcessor.processMono(left, numSamples);
    }

    if (params.characterMode != EQCharacterMode::Clean) {
      applyCharacter(left, numSamples);
    }
  }

  void processRightOnly(float *right, int numSamples) {
    filterR.processBlock(right, numSamples);

    if (params.dynamicMode != EQDynamicMode::Off) {
      dynamicProcessor.processMono(right, numSamples);
    }

    if (params.characterMode != EQCharacterMode::Clean) {
      applyCharacter(right, numSamples);
    }
  }

  void applyCharacter(float *samples, int numSamples) {
    float drive, mix;
    switch (params.characterMode) {
    case EQCharacterMode::Clean:
      return;
    case EQCharacterMode::Subtle:
      drive = 1.1f;
      mix = 0.1f;
      break;
    case EQCharacterMode::Warm:
      drive = 1.5f;
      mix = 0.3f;
      break;
    default:
      return;
    }

    const float dryMix = 1.0f - mix;
    for (int i = 0; i < numSamples; ++i) {
      float dry = samples[i];
      float saturated = std::tanh(drive * dry);
      samples[i] = dry * dryMix + saturated * mix;
    }
  }

  EQBandParams params;
  double sampleRate = 44100.0;
  int maxBlockSize = 0;

  // Static EQ filters
  CascadedBiquad filterL;
  CascadedBiquad filterR;

  // Dynamic EQ processor
  DynamicEQProcessor dynamicProcessor;

  // Gain smoother
  juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> gainSmoother;

  // Work buffers
  std::vector<float> workBufferL;
  std::vector<float> workBufferR;
};

} // namespace Sphere
