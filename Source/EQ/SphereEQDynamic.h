/*
  ==============================================================================
    SphereEQDynamic.h
    Advanced Dynamic EQ Processor
    
    Features:
    - Sidechain filtering at band frequency/Q for accurate level detection
    - Peak and RMS envelope detection with one-pole IIR smoothing
    - Soft knee compression with configurable knee width
    - Downward compression and upward expansion
    - Attack/release times from fast transients to slow mastering
    - Static + dynamic gain combination with range limiting
  ==============================================================================
*/

#pragma once

#include "SphereEQTypes.h"
#include "SphereEQCookbook.h"
#include <cmath>
#include <algorithm>

namespace Sphere {

// ============================================================================
// Fast Math Utilities for RT-safe processing
// ============================================================================
namespace DynamicFastMath {
    // Fast log2 using bit manipulation (IEEE 754 float)
    inline float fastLog2(float x) {
        union { float f; uint32_t i; } vx = { x };
        float y = static_cast<float>(vx.i);
        y *= 1.1920928955078125e-7f;
        return y - 126.94269504f;
    }
    
    inline float fastLog10(float x) {
        return fastLog2(x) * 0.3010299957f;
    }
    
    inline float fastPow2(float p) {
        float clipp = (p < -126.0f) ? -126.0f : p;
        union { uint32_t i; float f; } v;
        v.i = static_cast<uint32_t>((1 << 23) * (clipp + 126.94269504f));
        return v.f;
    }
    
    inline float fastPow10(float x) {
        return fastPow2(x * 3.321928095f);
    }
    
    inline float dbToLinear(float db) {
        return fastPow10(db * 0.05f);
    }
    
    inline float linearToDb(float linear) {
        if (linear < 1e-10f) return -100.0f;
        return 20.0f * fastLog10(linear);
    }
}

// ============================================================================
// Sidechain Biquad Filter
// Narrow bandpass filter for frequency-specific level detection
// ============================================================================
class SidechainFilter {
public:
    void prepare(double sampleRate) {
        this->sampleRate = sampleRate;
        reset();
    }
    
    void setParameters(double frequency, double q) {
        if (sampleRate <= 0.0) return;
        
        // Calculate bandpass coefficients using RBJ cookbook
        double omega = 2.0 * M_PI * frequency / sampleRate;
        double sinOmega = std::sin(omega);
        double cosOmega = std::cos(omega);
        double alpha = sinOmega / (2.0 * q);
        
        // Bandpass (constant 0 dB peak gain)
        b0 = alpha;
        b1 = 0.0;
        b2 = -alpha;
        a0 = 1.0 + alpha;
        a1 = -2.0 * cosOmega;
        a2 = 1.0 - alpha;
        
        // Normalize
        b0 /= a0;
        b1 /= a0;
        b2 /= a0;
        a1 /= a0;
        a2 /= a0;
    }
    
    void reset() {
        x1 = x2 = y1 = y2 = 0.0;
    }
    
    // Process single sample (for sidechain)
    float processSample(float input) {
        double x0 = static_cast<double>(input);
        double y0 = b0 * x0 + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2;
        
        x2 = x1; x1 = x0;
        y2 = y1; y1 = y0;
        
        // Denormal prevention
        if (std::abs(y1) < 1e-15) y1 = 0.0;
        if (std::abs(y2) < 1e-15) y2 = 0.0;
        
        return static_cast<float>(y0);
    }
    
    // Process block for sidechain analysis
    void processBlock(const float* input, float* output, int numSamples) {
        for (int i = 0; i < numSamples; ++i) {
            output[i] = processSample(input[i]);
        }
    }
    
private:
    double sampleRate = 44100.0;
    double b0 = 0.0, b1 = 0.0, b2 = 0.0;
    double a0 = 1.0, a1 = 0.0, a2 = 0.0;
    double x1 = 0.0, x2 = 0.0;
    double y1 = 0.0, y2 = 0.0;
};

// ============================================================================
// Envelope Follower with true Attack/Release behavior
// Supports Peak, RMS, and hybrid detection modes
// ============================================================================
class DynamicEnvelopeFollower {
public:
    void prepare(double sampleRate, int maxBlockSize) {
        this->sampleRate = sampleRate;
        rmsWindowSize = static_cast<int>(sampleRate * 0.01); // 10ms RMS window
        rmsWindowSize = std::min(rmsWindowSize, maxBlockSize);
        rmsBuffer.resize(rmsWindowSize, 0.0f);
        rmsIndex = 0;
        rmsSum = 0.0f;
        reset();
    }
    
    void setAttack(double attackMs) {
        attackMs = std::max(0.1, std::min(500.0, attackMs));
        attackCoeff = static_cast<float>(std::exp(-1.0 / (sampleRate * attackMs * 0.001)));
    }
    
    void setRelease(double releaseMs) {
        releaseMs = std::max(5.0, std::min(5000.0, releaseMs));
        releaseCoeff = static_cast<float>(std::exp(-1.0 / (sampleRate * releaseMs * 0.001)));
    }
    
    void setDetectionMode(EQDetectionMode mode) {
        detectionMode = mode;
    }
    
    void reset() {
        envelope = 0.0f;
        peakHold = 0.0f;
        std::fill(rmsBuffer.begin(), rmsBuffer.end(), 0.0f);
        rmsSum = 0.0f;
        rmsIndex = 0;
    }
    
    // Process sample and return envelope level
    float processSample(float input) {
        float absInput = std::abs(input);
        float level = 0.0f;
        
        switch (detectionMode) {
            case EQDetectionMode::Peak:
                level = absInput;
                break;
                
            case EQDetectionMode::RMS:
                // Running RMS with circular buffer
                rmsSum -= rmsBuffer[rmsIndex];
                rmsBuffer[rmsIndex] = absInput * absInput;
                rmsSum += rmsBuffer[rmsIndex];
                rmsIndex = (rmsIndex + 1) % rmsWindowSize;
                level = std::sqrt(rmsSum / rmsWindowSize);
                break;
                
            case EQDetectionMode::PeakRMS:
                // Peak for attack, RMS for release
                rmsSum -= rmsBuffer[rmsIndex];
                rmsBuffer[rmsIndex] = absInput * absInput;
                rmsSum += rmsBuffer[rmsIndex];
                rmsIndex = (rmsIndex + 1) % rmsWindowSize;
                float rms = std::sqrt(rmsSum / rmsWindowSize);
                level = (absInput > envelope) ? absInput : rms;
                break;
        }
        
        // One-pole envelope follower
        if (level > envelope) {
            envelope = attackCoeff * envelope + (1.0f - attackCoeff) * level;
        } else {
            envelope = releaseCoeff * envelope + (1.0f - releaseCoeff) * level;
        }
        
        // Denormal prevention
        if (envelope < 1e-10f) envelope = 0.0f;
        
        return envelope;
    }
    
    // Block processing - returns smoothed envelope for entire block
    float processBlock(const float* samples, int numSamples) {
        float maxEnv = 0.0f;
        for (int i = 0; i < numSamples; ++i) {
            float env = processSample(samples[i]);
            if (env > maxEnv) maxEnv = env;
        }
        return maxEnv;
    }
    
    float getEnvelope() const { return envelope; }
    
private:
    double sampleRate = 44100.0;
    EQDetectionMode detectionMode = EQDetectionMode::RMS;
    
    float attackCoeff = 0.9f;
    float releaseCoeff = 0.9995f;
    float envelope = 0.0f;
    float peakHold = 0.0f;
    
    // RMS calculation
    std::vector<float> rmsBuffer;
    int rmsWindowSize = 441;
    int rmsIndex = 0;
    float rmsSum = 0.0f;
};

// ============================================================================
// Gain Computer with Soft Knee
// Computes gain reduction/expansion based on threshold, ratio, knee
// ============================================================================
class DynamicGainComputer {
public:
    void setParameters(double thresholdDb, double ratio, double kneeDb, double rangeDb) {
        threshold = static_cast<float>(thresholdDb);
        this->ratio = static_cast<float>(ratio);
        knee = static_cast<float>(kneeDb);
        range = static_cast<float>(rangeDb);
        
        // Pre-calculate knee boundaries
        kneeStart = threshold - knee * 0.5f;
        kneeEnd = threshold + knee * 0.5f;
    }
    
    void setMode(EQDynamicMode mode) {
        this->mode = mode;
    }
    
    // Compute gain for given input level (in dB)
    float computeGain(float inputDb) const {
        float gainDb = 0.0f;
        
        switch (mode) {
            case EQDynamicMode::Off:
                return 0.0f;
                
            case EQDynamicMode::Compress:
                gainDb = computeCompression(inputDb);
                break;
                
            case EQDynamicMode::Expand:
                gainDb = computeExpansion(inputDb);
                break;
                
            case EQDynamicMode::Gate:
                gainDb = computeGate(inputDb);
                break;
                
            case EQDynamicMode::DeEss:
                // De-esser: aggressive compression above threshold
                gainDb = computeDeEss(inputDb);
                break;
        }
        
        // Clamp to range
        return std::max(-range, std::min(range, gainDb));
    }
    
private:
    // Downward compression with soft knee
    float computeCompression(float inputDb) const {
        if (inputDb < kneeStart) {
            // Below knee - no compression
            return 0.0f;
        } else if (inputDb > kneeEnd) {
            // Above knee - full compression
            float overshoot = inputDb - threshold;
            return overshoot * (1.0f / ratio - 1.0f);
        } else {
            // In knee region - soft transition
            float kneeInput = inputDb - kneeStart;
            float kneeWidth = kneeEnd - kneeStart;
            float kneeRatio = kneeInput / kneeWidth;
            
            // Quadratic interpolation for smooth transition
            float compressionAmount = kneeRatio * kneeRatio * 0.5f;
            float overshoot = inputDb - threshold;
            return overshoot * compressionAmount * (1.0f / ratio - 1.0f);
        }
    }
    
    // Upward expansion
    float computeExpansion(float inputDb) const {
        if (inputDb < kneeStart) {
            return 0.0f;
        } else if (inputDb > kneeEnd) {
            float overshoot = inputDb - threshold;
            return overshoot * (ratio - 1.0f);
        } else {
            float kneeInput = inputDb - kneeStart;
            float kneeWidth = kneeEnd - kneeStart;
            float kneeRatio = kneeInput / kneeWidth;
            float expansionAmount = kneeRatio * kneeRatio * 0.5f;
            float overshoot = inputDb - threshold;
            return overshoot * expansionAmount * (ratio - 1.0f);
        }
    }
    
    // Downward expansion (gate)
    float computeGate(float inputDb) const {
        if (inputDb > kneeEnd) {
            return 0.0f;
        } else if (inputDb < kneeStart) {
            float undershoot = threshold - inputDb;
            return -undershoot * (ratio - 1.0f);
        } else {
            float kneeInput = kneeEnd - inputDb;
            float kneeWidth = kneeEnd - kneeStart;
            float kneeRatio = kneeInput / kneeWidth;
            float gateAmount = kneeRatio * kneeRatio * 0.5f;
            float undershoot = threshold - inputDb;
            return -undershoot * gateAmount * (ratio - 1.0f);
        }
    }
    
    // De-esser: very aggressive compression for sibilance
    float computeDeEss(float inputDb) const {
        if (inputDb < threshold) {
            return 0.0f;
        }
        // More aggressive ratio for de-essing
        float effectiveRatio = ratio * 2.0f;
        float overshoot = inputDb - threshold;
        return overshoot * (1.0f / effectiveRatio - 1.0f);
    }
    
    EQDynamicMode mode = EQDynamicMode::Off;
    float threshold = -20.0f;
    float ratio = 2.0f;
    float knee = 6.0f;
    float range = 12.0f;
    float kneeStart = -23.0f;
    float kneeEnd = -17.0f;
};

// ============================================================================
// Complete Dynamic EQ Band Processor
// Combines sidechain filtering, envelope detection, and gain computation
// ============================================================================
class DynamicEQProcessor {
public:
    void prepare(double sampleRate, int maxBlockSize) {
        this->sampleRate = sampleRate;
        this->maxBlockSize = maxBlockSize;
        
        sidechainL.prepare(sampleRate);
        sidechainR.prepare(sampleRate);
        envelopeL.prepare(sampleRate, maxBlockSize);
        envelopeR.prepare(sampleRate, maxBlockSize);
        
        sidechainBufferL.resize(maxBlockSize);
        sidechainBufferR.resize(maxBlockSize);
        
        gainSmoother.reset(sampleRate, 0.005); // 5ms smoothing for gain changes
        gainSmoother.setCurrentAndTargetValue(1.0f);
        
        reset();
    }
    
    void reset() {
        sidechainL.reset();
        sidechainR.reset();
        envelopeL.reset();
        envelopeR.reset();
        gainSmoother.setCurrentAndTargetValue(1.0f);
        currentGainReductionDb = 0.0f;
    }
    
    void setParameters(const EQBandParams& params) {
        // Update sidechain filters to match band frequency
        double sidechainQ = std::max(0.5, std::min(10.0, params.dynamicSidechainQ));
        sidechainL.setParameters(params.frequency, sidechainQ);
        sidechainR.setParameters(params.frequency, sidechainQ);
        
        // Update envelope followers
        envelopeL.setAttack(params.dynamicAttack);
        envelopeL.setRelease(params.dynamicRelease);
        envelopeL.setDetectionMode(params.dynamicDetection);
        envelopeR.setAttack(params.dynamicAttack);
        envelopeR.setRelease(params.dynamicRelease);
        envelopeR.setDetectionMode(params.dynamicDetection);
        
        // Update gain computer
        gainComputer.setParameters(
            params.dynamicThreshold,
            params.dynamicRatio,
            params.dynamicKnee,
            params.dynamicRange
        );
        gainComputer.setMode(params.dynamicMode);
        
        // Store for mix calculation
        staticGainDb = static_cast<float>(params.gainDb);
        dynamicMix = static_cast<float>(params.dynamicMix * 0.01); // Convert % to 0-1
        autoMakeup = params.dynamicAutoMakeup;
        makeupGainDb = static_cast<float>(params.dynamicMakeupGain);
        dynamicMode = params.dynamicMode;
    }
    
    bool isActive() const {
        return dynamicMode != EQDynamicMode::Off;
    }
    
    // Process stereo block with dynamic gain
    void processBlock(float* left, float* right, int numSamples) {
        if (dynamicMode == EQDynamicMode::Off) return;
        
        // Step 1: Filter through sidechain to isolate band frequency
        sidechainL.processBlock(left, sidechainBufferL.data(), numSamples);
        sidechainR.processBlock(right, sidechainBufferR.data(), numSamples);
        
        // Step 2: Compute envelope from filtered sidechain
        float envL = envelopeL.processBlock(sidechainBufferL.data(), numSamples);
        float envR = envelopeR.processBlock(sidechainBufferR.data(), numSamples);
        float envMax = std::max(envL, envR);
        
        // Step 3: Convert to dB and compute dynamic gain
        float envDb = DynamicFastMath::linearToDb(envMax);
        float dynamicGainDb = gainComputer.computeGain(envDb);
        
        // Step 4: Apply makeup gain if enabled
        float totalDynamicGainDb = dynamicGainDb;
        if (autoMakeup) {
            // Simple auto-makeup: add half the threshold for compression
            if (dynamicMode == EQDynamicMode::Compress) {
                totalDynamicGainDb -= dynamicGainDb * 0.5f; // Partial compensation
            }
        } else {
            totalDynamicGainDb += makeupGainDb;
        }
        
        // Step 5: Combine static and dynamic gain
        // Static gain is the baseline, dynamic modulates around it
        float totalGainDb = staticGainDb + totalDynamicGainDb;
        float targetGainLinear = DynamicFastMath::dbToLinear(totalGainDb);
        
        // Step 6: Apply parallel mix if not 100%
        if (dynamicMix < 0.99f) {
            float staticGainLinear = DynamicFastMath::dbToLinear(staticGainDb);
            targetGainLinear = staticGainLinear + (targetGainLinear - staticGainLinear) * dynamicMix;
        }
        
        // Step 7: Smooth gain changes
        gainSmoother.setTargetValue(targetGainLinear);
        
        // Step 8: Apply gain with smoothing
        for (int i = 0; i < numSamples; ++i) {
            float g = gainSmoother.getNextValue();
            left[i] *= g;
            right[i] *= g;
        }
        
        // Store for metering
        currentGainReductionDb = dynamicGainDb;
    }
    
    // Process mono buffer
    void processMono(float* samples, int numSamples) {
        if (dynamicMode == EQDynamicMode::Off) return;
        
        sidechainL.processBlock(samples, sidechainBufferL.data(), numSamples);
        float env = envelopeL.processBlock(sidechainBufferL.data(), numSamples);
        float envDb = DynamicFastMath::linearToDb(env);
        float dynamicGainDb = gainComputer.computeGain(envDb);
        
        float totalDynamicGainDb = dynamicGainDb + makeupGainDb;
        float totalGainDb = staticGainDb + totalDynamicGainDb;
        float targetGainLinear = DynamicFastMath::dbToLinear(totalGainDb);
        
        if (dynamicMix < 0.99f) {
            float staticGainLinear = DynamicFastMath::dbToLinear(staticGainDb);
            targetGainLinear = staticGainLinear + (targetGainLinear - staticGainLinear) * dynamicMix;
        }
        
        gainSmoother.setTargetValue(targetGainLinear);
        
        for (int i = 0; i < numSamples; ++i) {
            samples[i] *= gainSmoother.getNextValue();
        }
        
        currentGainReductionDb = dynamicGainDb;
    }
    
    // Get current gain reduction for metering (negative = reduction)
    float getGainReductionDb() const { return currentGainReductionDb; }
    
private:
    double sampleRate = 44100.0;
    int maxBlockSize = 512;
    
    // Sidechain filters
    SidechainFilter sidechainL;
    SidechainFilter sidechainR;
    
    // Envelope followers
    DynamicEnvelopeFollower envelopeL;
    DynamicEnvelopeFollower envelopeR;
    
    // Gain computer
    DynamicGainComputer gainComputer;
    
    // Gain smoother
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> gainSmoother;
    
    // Work buffers
    std::vector<float> sidechainBufferL;
    std::vector<float> sidechainBufferR;
    
    // Parameters
    EQDynamicMode dynamicMode = EQDynamicMode::Off;
    float staticGainDb = 0.0f;
    float dynamicMix = 1.0f;
    float makeupGainDb = 0.0f;
    bool autoMakeup = false;
    
    // Metering
    float currentGainReductionDb = 0.0f;
};

} // namespace Sphere





