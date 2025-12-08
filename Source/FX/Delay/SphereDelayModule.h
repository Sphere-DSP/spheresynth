#pragma once
#include "../Common/SphereFXModule.h"
#include "../Common/SphereFXDSP.h"

namespace Sphere {
namespace FX {

// ============================================================================
// Delay Types
// ============================================================================
enum class DelayType {
    Simple,         // Basic delay
    PingPong,       // Alternating L/R
    Tape,           // Tape emulation with wow/flutter
    Digital         // Clean digital delay
};

// ============================================================================
// Delay Module
// ============================================================================
class DelayModule : public FXModuleBase {
public:
    static constexpr float MAX_DELAY_SECONDS = 2.0f;
    
    DelayModule() 
        : FXModuleBase(ModuleType::Delay, "Delay", "sphere.fx.delay") {}
    
    // ========================================================================
    // Lifecycle
    // ========================================================================
    void prepare(const AudioContext& context) override {
        audioContext = context;
        
        int maxDelaySamples = static_cast<int>(context.sampleRate * MAX_DELAY_SECONDS);
        
        delayLineL.prepare(maxDelaySamples);
        delayLineR.prepare(maxDelaySamples);
        
        // Filters for tape emulation
        lowpassL.setCutoff(context.sampleRate, 8000.0);
        lowpassR.setCutoff(context.sampleRate, 8000.0);
        highpassL.setCutoff(context.sampleRate, 80.0);
        highpassR.setCutoff(context.sampleRate, 80.0);
        
        updateDelayTime();
    }
    
    void reset() override {
        delayLineL.reset();
        delayLineR.reset();
        lowpassL.reset();
        lowpassR.reset();
        highpassL.reset();
        highpassR.reset();
        lfoPhase = 0.0f;
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
        
        // LFO for modulation
        float lfoIncrement = modRate / static_cast<float>(audioContext.sampleRate);
        
        for (int i = 0; i < numSamples; ++i) {
            float inputL = left[i];
            float inputR = right ? right[i] : inputL;
            
            // Modulation (for tape-style wow/flutter)
            float modAmount = 0.0f;
            if (delayType == DelayType::Tape && modDepth > 0.0f) {
                modAmount = std::sin(lfoPhase * juce::MathConstants<float>::twoPi) * modDepth;
                lfoPhase += lfoIncrement;
                if (lfoPhase >= 1.0f) lfoPhase -= 1.0f;
            }
            
            // Calculate modulated delay
            float modDelayL = delaySamplesL * (1.0f + modAmount * 0.01f);
            float modDelayR = delaySamplesR * (1.0f + modAmount * 0.01f);
            
            delayLineL.setDelayFractional(modDelayL);
            delayLineR.setDelayFractional(modDelayR);
            
            // Read from delay lines
            float delayedL = delayLineL.read(static_cast<int>(modDelayL));
            float delayedR = delayLineR.read(static_cast<int>(modDelayR));
            
            // Apply filtering for tape emulation
            if (delayType == DelayType::Tape) {
                delayedL = static_cast<float>(lowpassL.processSample(delayedL));
                delayedL = static_cast<float>(highpassL.processSample(delayedL));
                delayedR = static_cast<float>(lowpassR.processSample(delayedR));
                delayedR = static_cast<float>(highpassR.processSample(delayedR));
            }
            
            // Calculate feedback
            float feedbackL, feedbackR;
            
            if (delayType == DelayType::PingPong) {
                // Cross-feedback for ping-pong
                feedbackL = inputL + delayedR * feedback;
                feedbackR = inputR + delayedL * feedback;
            } else {
                feedbackL = inputL + delayedL * feedback;
                feedbackR = inputR + delayedR * feedback;
            }
            
            // Write to delay lines
            delayLineL.processSample(feedbackL);
            delayLineR.processSample(feedbackR);
            
            // Mix output
            left[i] = inputL * (1.0f - wetLevel) + delayedL * wetLevel;
            if (right) {
                right[i] = inputR * (1.0f - wetLevel) + delayedR * wetLevel;
            }
        }
    }
    
    // ========================================================================
    // Parameters
    // ========================================================================
    void setDelayTime(float timeMs) {
        delayTimeMs = juce::jlimit(1.0f, MAX_DELAY_SECONDS * 1000.0f, timeMs);
        updateDelayTime();
    }
    
    void setFeedback(float fb) {
        feedback = juce::jlimit(0.0f, 0.95f, fb);
    }
    
    void setWetLevel(float wet) {
        wetLevel = juce::jlimit(0.0f, 1.0f, wet);
    }
    
    void setDelayType(DelayType type) {
        delayType = type;
    }
    
    void setStereoSpread(float spread) {
        stereoSpread = juce::jlimit(0.0f, 1.0f, spread);
        updateDelayTime();
    }
    
    void setModDepth(float depth) {
        modDepth = juce::jlimit(0.0f, 100.0f, depth);
    }
    
    void setModRate(float rate) {
        modRate = juce::jlimit(0.1f, 10.0f, rate);
    }
    
    // ========================================================================
    // Parameters (IFXModule interface)
    // ========================================================================
    std::vector<ParameterDef> getParameterDefinitions() const override {
        return {
            {"delayTime", "Delay Time", "ms", ParameterType::Float, 1.0f, 2000.0f, 300.0f, 0.5f},
            {"feedback", "Feedback", "%", ParameterType::Float, 0.0f, 0.95f, 0.4f},
            {"wetLevel", "Mix", "%", ParameterType::Float, 0.0f, 1.0f, 0.5f},
            {"delayType", "Type", "", ParameterType::Choice, 0.0f, 3.0f, 0.0f},
            {"stereoSpread", "Stereo", "%", ParameterType::Float, 0.0f, 1.0f, 0.0f},
            {"modDepth", "Mod Depth", "%", ParameterType::Float, 0.0f, 100.0f, 0.0f},
            {"modRate", "Mod Rate", "Hz", ParameterType::Float, 0.1f, 10.0f, 1.0f}
        };
    }
    
    float getParameter(const juce::String& paramId) const override {
        if (paramId == "delayTime") return delayTimeMs;
        if (paramId == "feedback") return feedback;
        if (paramId == "wetLevel") return wetLevel;
        if (paramId == "delayType") return static_cast<float>(delayType);
        if (paramId == "stereoSpread") return stereoSpread;
        if (paramId == "modDepth") return modDepth;
        if (paramId == "modRate") return modRate;
        return 0.0f;
    }
    
    void setParameter(const juce::String& paramId, float value) override {
        if (paramId == "delayTime") setDelayTime(value);
        else if (paramId == "feedback") setFeedback(value);
        else if (paramId == "wetLevel") setWetLevel(value);
        else if (paramId == "delayType") setDelayType(static_cast<DelayType>(static_cast<int>(value)));
        else if (paramId == "stereoSpread") setStereoSpread(value);
        else if (paramId == "modDepth") setModDepth(value);
        else if (paramId == "modRate") setModRate(value);
    }

private:
    void updateDelayTime() {
        float baseDelaySamples = static_cast<float>(audioContext.sampleRate * delayTimeMs * 0.001);
        
        // Apply stereo spread
        float spreadAmount = stereoSpread * 0.1f;  // Up to 10% difference
        delaySamplesL = baseDelaySamples * (1.0f - spreadAmount);
        delaySamplesR = baseDelaySamples * (1.0f + spreadAmount);
        
        delayLineL.setDelayFractional(delaySamplesL);
        delayLineR.setDelayFractional(delaySamplesR);
    }
    
    // Parameters
    float delayTimeMs = 300.0f;
    float feedback = 0.4f;
    float wetLevel = 0.5f;
    DelayType delayType = DelayType::Simple;
    float stereoSpread = 0.0f;
    float modDepth = 0.0f;
    float modRate = 1.0f;
    
    // Processing
    DSP::DelayLine delayLineL;
    DSP::DelayLine delayLineR;
    float delaySamplesL = 0.0f;
    float delaySamplesR = 0.0f;
    
    // Tape emulation
    DSP::OnePole lowpassL, lowpassR;
    DSP::OnePole highpassL, highpassR;
    float lfoPhase = 0.0f;
};

// Register the Delay module
REGISTER_FX_MODULE(DelayModule, ModuleType::Delay, "Delay")

} // namespace FX
} // namespace Sphere

