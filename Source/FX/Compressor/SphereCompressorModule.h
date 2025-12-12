#pragma once
#include "../Common/SphereFXModule.h"
#include "../Common/SphereFXDSP.h"

namespace Sphere {
namespace FX {

// ============================================================================
// Compressor Detection Mode
// ============================================================================
enum class CompressorDetectionMode {
    Peak,
    RMS,
    TruePeak
};

// ============================================================================
// Compressor Knee Type
// ============================================================================
enum class CompressorKneeType {
    Hard,
    Soft
};

// ============================================================================
// Compressor Module
// ============================================================================
class CompressorModule : public FXModuleBase {
public:
    CompressorModule() 
        : FXModuleBase(ModuleType::Compressor, "Compressor", "sphere.fx.compressor") {}
    
    // ========================================================================
    // Lifecycle
    // ========================================================================
    void prepare(const AudioContext& context) override {
        audioContext = context;
        
        envelopeL.prepare(context.sampleRate);
        envelopeR.prepare(context.sampleRate);
        
        envelopeL.setAttackMs(attackMs);
        envelopeL.setReleaseMs(releaseMs);
        envelopeR.setAttackMs(attackMs);
        envelopeR.setReleaseMs(releaseMs);
        
        makeupGain.reset(context.sampleRate, 0.02);
        makeupGain.setCurrentAndTargetValue(1.0f);
        
        gainReductionSmooth.reset(context.sampleRate, 0.01);
        gainReductionSmooth.setCurrentAndTargetValue(0.0f);
    }
    
    void reset() override {
        envelopeL.reset();
        envelopeR.reset();
        currentGainReduction = 0.0f;
    }
    
    // ========================================================================
    // Processing
    // ========================================================================
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& /*midi*/) override {
        juce::ScopedNoDenormals noDenormals;
        
        const int numSamples = buffer.getNumSamples();
        const int numChannels = buffer.getNumChannels();
        
        if (numChannels == 0 || numSamples == 0) return;
        
        float* left = buffer.getWritePointer(0);
        float* right = numChannels > 1 ? buffer.getWritePointer(1) : nullptr;
        
        float maxGR = 0.0f;
        
        for (int i = 0; i < numSamples; ++i) {
            // Get input level
            float inputL = std::abs(left[i]);
            float inputR = right ? std::abs(right[i]) : inputL;
            
            // Envelope following
            float envL = static_cast<float>(envelopeL.processSample(inputL));
            float envR = static_cast<float>(envelopeR.processSample(inputR));
            float env = std::max(envL, envR);
            
            // Convert to dB
            float envDb = Utils::linearToDb(env + 1e-10f);
            
            // Calculate gain reduction
            float gainReductionDb = calculateGainReduction(envDb);
            maxGR = std::max(maxGR, gainReductionDb);
            
            // Apply gain reduction
            float gain = Utils::dbToLinear(-gainReductionDb) * makeupGain.getNextValue();
            
            left[i] *= gain;
            if (right) right[i] *= gain;
        }
        
        // Update metering
        gainReductionSmooth.setTargetValue(maxGR);
        currentGainReduction = gainReductionSmooth.getNextValue();
    }
    
    // ========================================================================
    // Parameters
    // ========================================================================
    void setThreshold(float thresholdDb) {
        this->thresholdDb = juce::jlimit(-60.0f, 0.0f, thresholdDb);
    }
    
    void setRatio(float ratio) {
        this->ratio = juce::jlimit(1.0f, 100.0f, ratio);
    }
    
    void setAttack(float attackMs) {
        this->attackMs = juce::jlimit(0.1f, 500.0f, attackMs);
        envelopeL.setAttackMs(this->attackMs);
        envelopeR.setAttackMs(this->attackMs);
    }
    
    void setRelease(float releaseMs) {
        this->releaseMs = juce::jlimit(10.0f, 3000.0f, releaseMs);
        envelopeL.setReleaseMs(this->releaseMs);
        envelopeR.setReleaseMs(this->releaseMs);
    }
    
    void setMakeupGain(float gainDb) {
        makeupGain.setTargetValue(Utils::dbToLinear(juce::jlimit(-12.0f, 24.0f, gainDb)));
    }
    
    void setKneeWidth(float kneeDb) {
        this->kneeWidthDb = juce::jlimit(0.0f, 24.0f, kneeDb);
    }
    
    // ========================================================================
    // Metering
    // ========================================================================
    float getGainReduction() const { return currentGainReduction; }
    
    ProcessingStats getStats() const override {
        ProcessingStats stats;
        stats.gainReduction = currentGainReduction;
        return stats;
    }
    
    // ========================================================================
    // Parameters (IFXModule interface)
    // ========================================================================
    std::vector<ParameterDef> getParameterDefinitions() const override {
        return {
            {"threshold", "Threshold", "dB", ParameterType::Float, -60.0f, 0.0f, -20.0f},
            {"ratio", "Ratio", ":1", ParameterType::Float, 1.0f, 100.0f, 4.0f, 0.5f},
            {"attack", "Attack", "ms", ParameterType::Float, 0.1f, 500.0f, 10.0f, 0.3f},
            {"release", "Release", "ms", ParameterType::Float, 10.0f, 3000.0f, 100.0f, 0.3f},
            {"makeupGain", "Makeup", "dB", ParameterType::Float, -12.0f, 24.0f, 0.0f},
            {"knee", "Knee", "dB", ParameterType::Float, 0.0f, 24.0f, 6.0f}
        };
    }
    
    float getParameter(const juce::String& paramId) const override {
        if (paramId == "threshold") return thresholdDb;
        if (paramId == "ratio") return ratio;
        if (paramId == "attack") return attackMs;
        if (paramId == "release") return releaseMs;
        if (paramId == "makeupGain") return Utils::linearToDb(makeupGain.getCurrentValue());
        if (paramId == "knee") return kneeWidthDb;
        return 0.0f;
    }
    
    void setParameter(const juce::String& paramId, float value) override {
        if (paramId == "threshold") setThreshold(value);
        else if (paramId == "ratio") setRatio(value);
        else if (paramId == "attack") setAttack(value);
        else if (paramId == "release") setRelease(value);
        else if (paramId == "makeupGain") setMakeupGain(value);
        else if (paramId == "knee") setKneeWidth(value);
    }

private:
    float calculateGainReduction(float inputDb) const {
        if (inputDb < thresholdDb - kneeWidthDb / 2.0f) {
            return 0.0f;  // Below threshold
        }
        
        float overThreshold;
        
        if (kneeWidthDb > 0.0f && inputDb < thresholdDb + kneeWidthDb / 2.0f) {
            // Soft knee region
            float x = inputDb - thresholdDb + kneeWidthDb / 2.0f;
            overThreshold = (x * x) / (2.0f * kneeWidthDb);
        } else {
            // Above knee
            overThreshold = inputDb - thresholdDb;
        }
        
        // Calculate gain reduction based on ratio
        float gainReduction = overThreshold * (1.0f - 1.0f / ratio);
        
        return std::max(0.0f, gainReduction);
    }
    
    // Parameters
    float thresholdDb = -20.0f;
    float ratio = 4.0f;
    float attackMs = 10.0f;
    float releaseMs = 100.0f;
    float kneeWidthDb = 6.0f;
    
    // Processing
    DSP::EnvelopeFollower envelopeL;
    DSP::EnvelopeFollower envelopeR;
    DSP::SmoothedValue<float> makeupGain;
    DSP::SmoothedValue<float> gainReductionSmooth;
    float currentGainReduction = 0.0f;
};

// Register the Compressor module
REGISTER_FX_MODULE(CompressorModule, ModuleType::Compressor, "Compressor")

} // namespace FX
} // namespace Sphere







