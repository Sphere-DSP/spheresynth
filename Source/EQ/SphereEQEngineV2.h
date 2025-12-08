#pragma once
#include "SphereEQBandProcessor.h"
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
// 24-Band Parametric EQ Engine (RT-optimized)
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
    // Prepare for playback (called from prepareToPlay with actual block size)
    // ========================================================================
    void prepare(double sampleRate, int maxBlockSize, int numChannels) {
        this->sampleRate = sampleRate;
        this->maxBlockSize = maxBlockSize;
        this->numChannels = juce::jlimit(1, 2, numChannels);
        
        // Prepare all band processors with sample rate
        for (int i = 0; i < MAX_EQ_BANDS; ++i) {
            bands[i].prepare(sampleRate, maxBlockSize);
        }
        
        // Allocate M/S working buffers
        midBuffer.resize(maxBlockSize);
        sideBuffer.resize(maxBlockSize);
        
        // Setup output gain smoother
        outputGainSmoother.reset(sampleRate, 0.05); // 50ms smoothing
        outputGainSmoother.setCurrentAndTargetValue(1.0f);
        
        prepared = true;
        
        DBG(juce::String("EQ Engine V2 prepared: sampleRate=") + juce::String(sampleRate) + " blockSize=" + juce::String(maxBlockSize));
    }
    
    void reset() {
        for (int i = 0; i < MAX_EQ_BANDS; ++i) {
            bands[i].reset();
        }
        outputGainSmoother.setCurrentAndTargetValue(1.0f);
    }
    
    // ========================================================================
    // RT-safe audio processing
    // ========================================================================
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& /*midi*/) {
        if (!prepared) {
            return;
        }
        
        // Check for pending parameter update (lock-free)
        int pending = pendingSnapshot.load(std::memory_order_acquire);
        if (pending >= 0) {
            currentSnapshot.store(pending);
            pendingSnapshot.store(-1, std::memory_order_release);
            
            // Apply new parameters to processors
            auto& snapshot = paramSnapshots[pending];
            for (int idx : snapshot.activeBandIndices) {
                bands[idx].setParametersFromSnapshot(snapshot.bandParams[idx]);
            }
        }
        
        auto& snapshot = paramSnapshots[currentSnapshot.load(std::memory_order_relaxed)];
        
        // Debug: log once when we have active bands
        static bool loggedOnce = false;
        if (!loggedOnce && !snapshot.activeBandIndices.empty()) {
            DBG(juce::String("EQ Processing active: ") + juce::String(static_cast<int>(snapshot.activeBandIndices.size())) + " bands");
            loggedOnce = true;
        }
        
        if (!snapshot.enabled || snapshot.activeBandIndices.empty()) {
            // Just apply output gain if no active bands
            applyOutputGain(buffer, snapshot.outputGainLinear);
            return;
        }
        
        juce::ScopedNoDenormals noDenormals;
        
        const int numSamples = buffer.getNumSamples();
        const int numChans = buffer.getNumChannels();
        
        if (numChans == 0 || numSamples == 0) return;
        
        float* leftChannel = buffer.getWritePointer(0);
        float* rightChannel = (numChans > 1) ? buffer.getWritePointer(1) : nullptr;
        
        // Update filters if needed (once per block)
        for (int idx : snapshot.activeBandIndices) {
            bands[idx].updateIfNeeded();
        }
        
        // Process M/S bands first if any exist (shared conversion)
        if (!snapshot.midModeBandIndices.empty() || !snapshot.sideModeBandIndices.empty()) {
            processMidSideBands(leftChannel, rightChannel, numSamples, snapshot);
        }
        
        // Process regular stereo/L/R bands
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
        
        // DEBUG TEST: Reduce volume to prove EQ is processing
        // Remove this after confirming EQ works!
        for (int i = 0; i < numSamples; ++i) {
            leftChannel[i] *= 0.1f;
            if (rightChannel) rightChannel[i] *= 0.1f;
        }
        
        // Apply output gain with smoothing
        applyOutputGain(buffer, snapshot.outputGainLinear);
    }
    
    // ========================================================================
    // Parameter updates (called from UI thread, lock-free)
    // ========================================================================
    void setBandParameters(int bandIndex, const EQBandParams& params) {
        if (bandIndex < 0 || bandIndex >= MAX_EQ_BANDS) return;
        
        // Check if there's already a pending snapshot to build upon
        int pending = pendingSnapshot.load(std::memory_order_acquire);
        int activeIdx = currentSnapshot.load(std::memory_order_relaxed);
        int writeIdx = 1 - activeIdx;
        
        // If pending exists and matches our write target, keep building on it
        // Otherwise copy from current snapshot
        if (pending != writeIdx) {
            paramSnapshots[writeIdx] = paramSnapshots[activeIdx];
        }
        
        paramSnapshots[writeIdx].bandParams[bandIndex] = params;
        paramSnapshots[writeIdx].rebuildActiveIndices();
        
        // Also immediately update the band processor for the current prepare state
        if (prepared) {
            bands[bandIndex].setParametersFromSnapshot(params);
        }
        
        // Publish new snapshot
        pendingSnapshot.store(writeIdx, std::memory_order_release);
    }
    
    const EQBandParams& getBandParameters(int bandIndex) const {
        if (bandIndex < 0 || bandIndex >= MAX_EQ_BANDS) {
            static EQBandParams defaultParams;
            return defaultParams;
        }
        return paramSnapshots[currentSnapshot.load(std::memory_order_relaxed)].bandParams[bandIndex];
    }
    
    // Convenience setters
    void setBandType(int bandIndex, EQFilterType type) {
        if (bandIndex < 0 || bandIndex >= MAX_EQ_BANDS) return;
        auto params = getBandParameters(bandIndex);
        params.type = type;
        setBandParameters(bandIndex, params);
    }
    
    void setBandFrequency(int bandIndex, double frequency) {
        if (bandIndex < 0 || bandIndex >= MAX_EQ_BANDS) return;
        auto params = getBandParameters(bandIndex);
        params.frequency = juce::jlimit(MIN_FREQUENCY, MAX_FREQUENCY, frequency);
        setBandParameters(bandIndex, params);
    }
    
    void setBandQ(int bandIndex, double q) {
        if (bandIndex < 0 || bandIndex >= MAX_EQ_BANDS) return;
        auto params = getBandParameters(bandIndex);
        params.q = juce::jlimit(MIN_Q, MAX_Q, q);
        setBandParameters(bandIndex, params);
    }
    
    void setBandGain(int bandIndex, double gainDb) {
        if (bandIndex < 0 || bandIndex >= MAX_EQ_BANDS) return;
        auto params = getBandParameters(bandIndex);
        params.gainDb = juce::jlimit(MIN_GAIN_DB, MAX_GAIN_DB, gainDb);
        setBandParameters(bandIndex, params);
    }
    
    void setBandSlope(int bandIndex, EQSlope slope) {
        if (bandIndex < 0 || bandIndex >= MAX_EQ_BANDS) return;
        auto params = getBandParameters(bandIndex);
        params.slope = slope;
        setBandParameters(bandIndex, params);
    }
    
    void setBandBypass(int bandIndex, bool bypass) {
        if (bandIndex < 0 || bandIndex >= MAX_EQ_BANDS) return;
        auto params = getBandParameters(bandIndex);
        params.bypass = bypass;
        setBandParameters(bandIndex, params);
    }
    
    void setBandStereoMode(int bandIndex, EQStereoMode mode) {
        if (bandIndex < 0 || bandIndex >= MAX_EQ_BANDS) return;
        auto params = getBandParameters(bandIndex);
        params.stereoMode = mode;
        setBandParameters(bandIndex, params);
    }
    
    void setBandDynamicMode(int bandIndex, EQDynamicMode mode) {
        if (bandIndex < 0 || bandIndex >= MAX_EQ_BANDS) return;
        auto params = getBandParameters(bandIndex);
        params.dynamicMode = mode;
        setBandParameters(bandIndex, params);
    }
    
    void setBandDynamicParams(int bandIndex, double threshold, double ratio,
                               double attack, double release, double range) {
        if (bandIndex < 0 || bandIndex >= MAX_EQ_BANDS) return;
        auto params = getBandParameters(bandIndex);
        params.dynamicThreshold = threshold;
        params.dynamicRatio = ratio;
        params.dynamicAttack = attack;
        params.dynamicRelease = release;
        params.dynamicRange = range;
        setBandParameters(bandIndex, params);
    }
    
    void setBandCharacter(int bandIndex, EQCharacterMode mode) {
        if (bandIndex < 0 || bandIndex >= MAX_EQ_BANDS) return;
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
        return paramSnapshots[currentSnapshot.load(std::memory_order_relaxed)].enabled;
    }
    
    void setOutputGain(double gainDb) {
        float linear = static_cast<float>(std::pow(10.0, juce::jlimit(-24.0, 24.0, gainDb) / 20.0));
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
        auto& snapshot = paramSnapshots[currentSnapshot.load(std::memory_order_relaxed)];
        if (!snapshot.enabled) return 1.0;
        
        double totalMagnitude = 1.0;
        for (int idx : snapshot.activeBandIndices) {
            totalMagnitude *= bands[idx].getMagnitudeResponse(frequency);
        }
        return totalMagnitude * snapshot.outputGainLinear;
    }
    
    double getMagnitudeResponseDb(double frequency) const {
        double mag = getMagnitudeResponse(frequency);
        if (mag <= 0.0) return -100.0;
        return 20.0 * std::log10(mag);
    }
    
    int getActiveBandCount() const {
        return static_cast<int>(paramSnapshots[currentSnapshot.load()].activeBandIndices.size());
    }

private:
    // ========================================================================
    // Shared M/S conversion and processing
    // ========================================================================
    void processMidSideBands(float* left, float* right, int numSamples,
                             const EQParameterSnapshot& snapshot) {
        if (!right) return;
        
        // Convert L/R to M/S (single conversion for all M/S bands)
        for (int i = 0; i < numSamples; ++i) {
            midBuffer[i] = (left[i] + right[i]) * 0.5f;
            sideBuffer[i] = (left[i] - right[i]) * 0.5f;
        }
        
        // Process all Mid bands
        for (int idx : snapshot.midModeBandIndices) {
            bands[idx].processMidBuffer(midBuffer.data(), numSamples);
        }
        
        // Process all Side bands
        for (int idx : snapshot.sideModeBandIndices) {
            bands[idx].processSideBuffer(sideBuffer.data(), numSamples);
        }
        
        // Convert M/S back to L/R (single conversion)
        for (int i = 0; i < numSamples; ++i) {
            left[i] = midBuffer[i] + sideBuffer[i];
            right[i] = midBuffer[i] - sideBuffer[i];
        }
    }
    
    // ========================================================================
    // Output gain with LinearSmoothedValue
    // ========================================================================
    void applyOutputGain(juce::AudioBuffer<float>& buffer, float targetGain) {
        outputGainSmoother.setTargetValue(targetGain);
        
        if (outputGainSmoother.isSmoothing()) {
            // Apply smoothed gain ramp
            const int numSamples = buffer.getNumSamples();
            const int numChans = buffer.getNumChannels();
            
            for (int i = 0; i < numSamples; ++i) {
                float g = outputGainSmoother.getNextValue();
                for (int ch = 0; ch < numChans; ++ch) {
                    buffer.getWritePointer(ch)[i] *= g;
                }
            }
        } else {
            // No smoothing needed, use JUCE's optimized gain application
            float currentGain = outputGainSmoother.getCurrentValue();
            if (std::abs(currentGain - 1.0f) > 0.0001f) {
                buffer.applyGain(currentGain);
            }
        }
    }
    
    // Band processors
    std::array<EQBandProcessor, MAX_EQ_BANDS> bands;
    
    // Double-buffered parameter snapshots for lock-free updates
    std::array<EQParameterSnapshot, 2> paramSnapshots;
    std::atomic<int> currentSnapshot{0};
    std::atomic<int> pendingSnapshot{-1};
    
    // M/S working buffers
    std::vector<float> midBuffer;
    std::vector<float> sideBuffer;
    
    // Output gain smoother
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> outputGainSmoother;
    
    // State
    double sampleRate = 44100.0;
    int maxBlockSize = 0;
    int numChannels = 2;
    bool prepared = false;
};

} // namespace Sphere
