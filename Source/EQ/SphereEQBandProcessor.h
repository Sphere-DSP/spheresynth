#pragma once
#include "SphereEQBiquad.h"

namespace Sphere {

// ============================================================================
// Fast math approximations for RT-safe processing
// ============================================================================
namespace FastMath {
    // Fast log2 approximation using bit manipulation
    inline float fastLog2(float x) {
        union { float f; uint32_t i; } vx = { x };
        float y = (float)vx.i;
        y *= 1.1920928955078125e-7f; // 1 / (1 << 23)
        return y - 126.94269504f;
    }
    
    // Fast log10 approximation: log10(x) = log2(x) / log2(10)
    inline float fastLog10(float x) {
        return fastLog2(x) * 0.3010299957f; // 1/log2(10)
    }
    
    // Fast pow2 approximation
    inline float fastPow2(float p) {
        float clipp = (p < -126.0f) ? -126.0f : p;
        union { uint32_t i; float f; } v;
        v.i = (uint32_t)((1 << 23) * (clipp + 126.94269504f));
        return v.f;
    }
    
    // Fast pow10 approximation: 10^x = 2^(x * log2(10))
    inline float fastPow10(float x) {
        return fastPow2(x * 3.321928095f); // log2(10)
    }
    
    // dB to linear (fast): 10^(dB/20)
    inline float dbToLinearFast(float db) {
        return fastPow10(db * 0.05f);
    }
    
    // Linear to dB (fast): 20 * log10(linear)
    inline float linearToDbFast(float linear) {
        return 20.0f * fastLog10(linear);
    }
}

// ============================================================================
// Block-based Envelope Follower (RT-optimized)
// ============================================================================
class BlockEnvelopeFollower {
public:
    void prepare(double sampleRate, int maxBlockSize) {
        this->sampleRate = sampleRate;
        updateCoefficients();
        reset();
    }
    
    void setAttack(double attackMs) {
        attackTime = juce::jlimit(0.1, 1000.0, attackMs);
        updateCoefficients();
    }
    
    void setRelease(double releaseMs) {
        releaseTime = juce::jlimit(1.0, 5000.0, releaseMs);
        updateCoefficients();
    }
    
    void reset() {
        envelope = 0.0f;
        peakHold = 0.0f;
    }
    
    // Process entire block and return smoothed envelope (block-rate update)
    float processBlock(const float* samples, int numSamples) {
        // Find peak in block (vectorizable)
        float blockPeak = 0.0f;
        for (int i = 0; i < numSamples; ++i) {
            float abs = std::abs(samples[i]);
            if (abs > blockPeak) blockPeak = abs;
        }
        
        // Smooth envelope
        if (blockPeak > envelope) {
            envelope += (blockPeak - envelope) * attackCoeffBlock;
        } else {
            envelope += (blockPeak - envelope) * releaseCoeffBlock;
        }
        
        // Prevent denormals
        if (envelope < 1e-10f) envelope = 0.0f;
        
        return envelope;
    }
    
    float getEnvelope() const { return envelope; }

private:
    void updateCoefficients() {
        if (sampleRate > 0.0) {
            // Per-sample coefficients
            float attackSamples = static_cast<float>(sampleRate * attackTime * 0.001);
            float releaseSamples = static_cast<float>(sampleRate * releaseTime * 0.001);
            
            // Block-rate coefficients (assuming ~64-256 sample blocks)
            // Faster response for block-based updates
            attackCoeffBlock = 1.0f - std::exp(-1.0f / (attackSamples * 0.1f));
            releaseCoeffBlock = 1.0f - std::exp(-1.0f / (releaseSamples * 0.1f));
        }
    }
    
    double sampleRate = 44100.0;
    double attackTime = 10.0;
    double releaseTime = 100.0;
    float attackCoeffBlock = 0.1f;
    float releaseCoeffBlock = 0.01f;
    float envelope = 0.0f;
    float peakHold = 0.0f;
};

// ============================================================================
// Single EQ Band Processor (RT-optimized)
// ============================================================================
class EQBandProcessor {
public:
    EQBandProcessor() = default;
    
    void prepare(double sampleRate, int maxBlockSize) {
        this->sampleRate = sampleRate;
        this->maxBlockSize = maxBlockSize;
        
        envelopeL.prepare(sampleRate, maxBlockSize);
        envelopeR.prepare(sampleRate, maxBlockSize);
        
        workBufferL.resize(maxBlockSize);
        workBufferR.resize(maxBlockSize);
        
        gainSmoother.reset(sampleRate, 0.02); // 20ms smoothing
        
        reset();
        updateFilters();
    }
    
    void reset() {
        filterL.reset();
        filterR.reset();
        envelopeL.reset();
        envelopeR.reset();
        gainSmoother.setCurrentAndTargetValue(1.0f);
    }
    
    // Parameter update - immediately updates filters
    void setParametersFromSnapshot(const EQBandParams& newParams) {
        params = newParams;
        updateFilters();  // Always update filters immediately
        
        DBG(juce::String("Band updated: type=") + juce::String(static_cast<int>(params.type)) 
            + " freq=" + juce::String(params.frequency) 
            + " gain=" + juce::String(params.gainDb) 
            + " bypass=" + juce::String(params.bypass ? 1 : 0)
            + " numStages=" + juce::String(filterL.getNumStages()));
    }
    
    // Called once per block before processing (for deferred updates if needed)
    void updateIfNeeded() {
        // Currently all updates happen immediately in setParametersFromSnapshot
    }
    
    const EQBandParams& getParameters() const { return params; }
    bool isBypassed() const { return params.bypass; }
    EQStereoMode getStereoMode() const { return params.stereoMode; }
    
    // Optimized stereo processing
    void processBlock(float* leftChannel, float* rightChannel, int numSamples) {
        if (params.bypass || numSamples == 0) return;
        
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
                // M/S handled at engine level for efficiency
                processStandardStereo(leftChannel, rightChannel, numSamples);
                break;
        }
    }
    
    // Process pre-converted M/S buffers
    void processMidBuffer(float* mid, int numSamples) {
        if (params.bypass || numSamples == 0) return;
        filterL.processBlock(mid, numSamples);
        if (params.characterMode != EQCharacterMode::Clean) {
            applyCharacter(mid, numSamples);
        }
    }
    
    void processSideBuffer(float* side, int numSamples) {
        if (params.bypass || numSamples == 0) return;
        filterR.processBlock(side, numSamples);
        if (params.characterMode != EQCharacterMode::Clean) {
            applyCharacter(side, numSamples);
        }
    }
    
    double getMagnitudeResponse(double frequency) const {
        if (params.bypass) return 1.0;
        return filterL.getMagnitudeResponse(frequency, sampleRate);
    }

private:
    void updateFilters() {
        if (sampleRate <= 0.0) return;
        
        int order = 2;
        
        if (params.type == EQFilterType::LowCut || params.type == EQFilterType::HighCut) {
            switch (params.slope) {
                case EQSlope::dB6:  order = 1; break;
                case EQSlope::dB12: order = 2; break;
                case EQSlope::dB18: order = 3; break;
                case EQSlope::dB24: order = 4; break;
                case EQSlope::dB36: order = 6; break;
                case EQSlope::dB48: order = 8; break;
                case EQSlope::dB72: order = 12; break;
                case EQSlope::dB96: order = 16; break;
            }
            filterL.configureButterworth(params.type, sampleRate, params.frequency, order);
            filterR.configureButterworth(params.type, sampleRate, params.frequency, order);
        } else {
            filterL.setNumStages(1);
            filterR.setNumStages(1);
            BiquadCoeffs coeffs = RBJCookbook::calculate(
                params.type, sampleRate, params.frequency, params.q, params.gainDb);
            filterL.setStageCoefficients(0, coeffs);
            filterR.setStageCoefficients(0, coeffs);
        }
        
        envelopeL.setAttack(params.dynamicAttack);
        envelopeL.setRelease(params.dynamicRelease);
        envelopeR.setAttack(params.dynamicAttack);
        envelopeR.setRelease(params.dynamicRelease);
    }
    
    void processStandardStereo(float* left, float* right, int numSamples) {
        if (params.dynamicMode == EQDynamicMode::Off) {
            filterL.processBlock(left, numSamples);
            filterR.processBlock(right, numSamples);
        } else {
            processDynamicBlock(left, right, numSamples);
        }
        
        if (params.characterMode != EQCharacterMode::Clean) {
            applyCharacter(left, numSamples);
            applyCharacter(right, numSamples);
        }
    }
    
    void processLeftOnly(float* left, int numSamples) {
        if (params.dynamicMode == EQDynamicMode::Off) {
            filterL.processBlock(left, numSamples);
        } else {
            // Block-based dynamic processing
            float envL = envelopeL.processBlock(left, numSamples);
            float gainMod = calculateDynamicGain(envL);
            gainSmoother.setTargetValue(gainMod);
            
            filterL.processBlock(left, numSamples);
            
            // Apply smoothed gain
            for (int i = 0; i < numSamples; ++i) {
                left[i] *= gainSmoother.getNextValue();
            }
        }
        
        if (params.characterMode != EQCharacterMode::Clean) {
            applyCharacter(left, numSamples);
        }
    }
    
    void processRightOnly(float* right, int numSamples) {
        if (params.dynamicMode == EQDynamicMode::Off) {
            filterR.processBlock(right, numSamples);
        } else {
            float envR = envelopeR.processBlock(right, numSamples);
            float gainMod = calculateDynamicGain(envR);
            gainSmoother.setTargetValue(gainMod);
            
            filterR.processBlock(right, numSamples);
            
            for (int i = 0; i < numSamples; ++i) {
                right[i] *= gainSmoother.getNextValue();
            }
        }
        
        if (params.characterMode != EQCharacterMode::Clean) {
            applyCharacter(right, numSamples);
        }
    }
    
    // Block-based dynamic EQ (much more efficient than per-sample)
    void processDynamicBlock(float* left, float* right, int numSamples) {
        // Get block envelope (single calculation per block)
        float envL = envelopeL.processBlock(left, numSamples);
        float envR = envelopeR.processBlock(right, numSamples);
        float envMax = std::max(envL, envR);
        
        // Calculate gain modification using fast math
        float gainMod = calculateDynamicGain(envMax);
        gainSmoother.setTargetValue(gainMod);
        
        // Process through filters
        filterL.processBlock(left, numSamples);
        filterR.processBlock(right, numSamples);
        
        // Apply smoothed gain ramp
        for (int i = 0; i < numSamples; ++i) {
            float g = gainSmoother.getNextValue();
            left[i] *= g;
            right[i] *= g;
        }
    }
    
    // Calculate dynamic gain using fast math (called once per block)
    float calculateDynamicGain(float envelope) {
        if (envelope < 1e-10f) return 1.0f;
        
        // Use fast log10 approximation
        float envDb = FastMath::linearToDbFast(envelope);
        float overThreshold = envDb - static_cast<float>(params.dynamicThreshold);
        
        if (overThreshold <= 0.0f) return 1.0f;
        
        float gainModDb = 0.0f;
        if (params.dynamicMode == EQDynamicMode::Compress) {
            gainModDb = -overThreshold * (1.0f - 1.0f / static_cast<float>(params.dynamicRatio));
            gainModDb = std::max(gainModDb, -static_cast<float>(params.dynamicRange));
        } else if (params.dynamicMode == EQDynamicMode::Expand) {
            gainModDb = overThreshold * (static_cast<float>(params.dynamicRatio) - 1.0f);
            gainModDb = std::min(gainModDb, static_cast<float>(params.dynamicRange));
        }
        
        return FastMath::dbToLinearFast(gainModDb);
    }
    
    void applyCharacter(float* samples, int numSamples) {
        float drive, mix;
        switch (params.characterMode) {
            case EQCharacterMode::Clean: return;
            case EQCharacterMode::Analog: drive = 1.1f; mix = 0.1f; break;
            case EQCharacterMode::Vintage: drive = 1.3f; mix = 0.2f; break;
            case EQCharacterMode::Aggressive: drive = 1.8f; mix = 0.35f; break;
            default: return;
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
    bool needsFilterUpdate = false;
    
    CascadedBiquad filterL;
    CascadedBiquad filterR;
    
    BlockEnvelopeFollower envelopeL;
    BlockEnvelopeFollower envelopeR;
    
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> gainSmoother;
    
    std::vector<float> workBufferL;
    std::vector<float> workBufferR;
};

} // namespace Sphere
