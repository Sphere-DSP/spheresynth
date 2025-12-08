#pragma once
#include "../Common/SphereFXModule.h"
#include "../Common/SphereFXDSP.h"
#include <array>

namespace Sphere {
namespace FX {

// ============================================================================
// Reverb Types
// ============================================================================
enum class ReverbType {
    Room,
    Hall,
    Plate,
    Chamber,
    Cathedral
};

// ============================================================================
// All-pass filter for reverb diffusion
// ============================================================================
class AllPassFilter {
public:
    void prepare(int delaySamples) {
        delay.prepare(delaySamples + 1);
        delay.setDelay(delaySamples);
    }
    
    void setFeedback(float fb) { feedback = fb; }
    
    void reset() { delay.reset(); }
    
    float processSample(float input) {
        float delayed = delay.read(0);
        float output = -input + delayed;
        delay.processSample(input + delayed * feedback);
        return output;
    }
    
private:
    DSP::DelayLine delay;
    float feedback = 0.5f;
};

// ============================================================================
// Comb filter for reverb tail
// ============================================================================
class CombFilter {
public:
    void prepare(int maxDelaySamples) {
        delay.prepare(maxDelaySamples);
    }
    
    void setDelay(int samples) {
        delay.setDelay(samples);
    }
    
    void setFeedback(float fb) { feedback = fb; }
    void setDamping(float damp) { damping = damp; }
    
    void reset() { 
        delay.reset();
        filterStore = 0.0f;
    }
    
    float processSample(float input) {
        float output = delay.read(0);
        
        // Low-pass filter for damping
        filterStore = output * (1.0f - damping) + filterStore * damping;
        
        delay.processSample(input + filterStore * feedback);
        
        return output;
    }
    
private:
    DSP::DelayLine delay;
    float feedback = 0.8f;
    float damping = 0.2f;
    float filterStore = 0.0f;
};

// ============================================================================
// Freeverb-style Reverb Module
// ============================================================================
class ReverbModule : public FXModuleBase {
public:
    static constexpr int NUM_COMBS = 8;
    static constexpr int NUM_ALLPASSES = 4;
    
    // Comb filter delay times (in samples at 44.1kHz)
    static constexpr std::array<int, NUM_COMBS> COMB_TUNINGS = {
        1116, 1188, 1277, 1356, 1422, 1491, 1557, 1617
    };
    
    // All-pass delay times
    static constexpr std::array<int, NUM_ALLPASSES> ALLPASS_TUNINGS = {
        556, 441, 341, 225
    };
    
    ReverbModule() 
        : FXModuleBase(ModuleType::Reverb, "Reverb", "sphere.fx.reverb") {}
    
    // ========================================================================
    // Lifecycle
    // ========================================================================
    void prepare(const AudioContext& context) override {
        audioContext = context;
        
        // Scale delay times for sample rate
        double ratio = context.sampleRate / 44100.0;
        
        for (int i = 0; i < NUM_COMBS; ++i) {
            int delayL = static_cast<int>(COMB_TUNINGS[i] * ratio);
            int delayR = static_cast<int>((COMB_TUNINGS[i] + 23) * ratio);  // Stereo spread
            
            combsL[i].prepare(delayL + 100);
            combsL[i].setDelay(delayL);
            
            combsR[i].prepare(delayR + 100);
            combsR[i].setDelay(delayR);
        }
        
        for (int i = 0; i < NUM_ALLPASSES; ++i) {
            int delay = static_cast<int>(ALLPASS_TUNINGS[i] * ratio);
            
            allpassesL[i].prepare(delay);
            allpassesL[i].setFeedback(0.5f);
            
            allpassesR[i].prepare(delay + 13);  // Stereo spread
            allpassesR[i].setFeedback(0.5f);
        }
        
        // Pre-delay
        int maxPreDelay = static_cast<int>(context.sampleRate * 0.1);  // 100ms max
        preDelayL.prepare(maxPreDelay);
        preDelayR.prepare(maxPreDelay);
        
        // Input/output filtering
        inputFilterL.setCutoff(context.sampleRate, 8000.0);
        inputFilterR.setCutoff(context.sampleRate, 8000.0);
        
        updateParameters();
    }
    
    void reset() override {
        for (auto& comb : combsL) comb.reset();
        for (auto& comb : combsR) comb.reset();
        for (auto& ap : allpassesL) ap.reset();
        for (auto& ap : allpassesR) ap.reset();
        preDelayL.reset();
        preDelayR.reset();
        inputFilterL.reset();
        inputFilterR.reset();
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
        
        for (int i = 0; i < numSamples; ++i) {
            float inputL = left[i];
            float inputR = right ? right[i] : inputL;
            
            // Pre-delay
            float preDelayedL = preDelayL.processSample(inputL);
            float preDelayedR = preDelayR.processSample(inputR);
            
            // Input filter (removes harsh highs)
            float filteredL = static_cast<float>(inputFilterL.processSample(preDelayedL));
            float filteredR = static_cast<float>(inputFilterR.processSample(preDelayedR));
            
            // Process through comb filters (parallel)
            float combOutL = 0.0f;
            float combOutR = 0.0f;
            
            for (int c = 0; c < NUM_COMBS; ++c) {
                combOutL += combsL[c].processSample(filteredL);
                combOutR += combsR[c].processSample(filteredR);
            }
            
            combOutL /= NUM_COMBS;
            combOutR /= NUM_COMBS;
            
            // Process through all-pass filters (series)
            for (int a = 0; a < NUM_ALLPASSES; ++a) {
                combOutL = allpassesL[a].processSample(combOutL);
                combOutR = allpassesR[a].processSample(combOutR);
            }
            
            // Mix dry and wet
            left[i] = inputL * (1.0f - wetLevel) + combOutL * wetLevel;
            if (right) {
                right[i] = inputR * (1.0f - wetLevel) + combOutR * wetLevel;
            }
        }
    }
    
    // ========================================================================
    // Parameters
    // ========================================================================
    void setRoomSize(float size) {
        roomSize = juce::jlimit(0.0f, 1.0f, size);
        updateParameters();
    }
    
    void setDamping(float damp) {
        damping = juce::jlimit(0.0f, 1.0f, damp);
        updateParameters();
    }
    
    void setWetLevel(float wet) {
        wetLevel = juce::jlimit(0.0f, 1.0f, wet);
    }
    
    void setPreDelay(float preDelayMs) {
        this->preDelayMs = juce::jlimit(0.0f, 100.0f, preDelayMs);
        int samples = static_cast<int>(audioContext.sampleRate * preDelayMs * 0.001);
        preDelayL.setDelay(samples);
        preDelayR.setDelay(samples);
    }
    
    void setWidth(float width) {
        this->width = juce::jlimit(0.0f, 1.0f, width);
    }
    
    // ========================================================================
    // Parameters (IFXModule interface)
    // ========================================================================
    std::vector<ParameterDef> getParameterDefinitions() const override {
        return {
            {"roomSize", "Room Size", "%", ParameterType::Float, 0.0f, 1.0f, 0.5f},
            {"damping", "Damping", "%", ParameterType::Float, 0.0f, 1.0f, 0.5f},
            {"wetLevel", "Wet Level", "%", ParameterType::Float, 0.0f, 1.0f, 0.3f},
            {"preDelay", "Pre-Delay", "ms", ParameterType::Float, 0.0f, 100.0f, 10.0f},
            {"width", "Width", "%", ParameterType::Float, 0.0f, 1.0f, 1.0f}
        };
    }
    
    float getParameter(const juce::String& paramId) const override {
        if (paramId == "roomSize") return roomSize;
        if (paramId == "damping") return damping;
        if (paramId == "wetLevel") return wetLevel;
        if (paramId == "preDelay") return preDelayMs;
        if (paramId == "width") return width;
        return 0.0f;
    }
    
    void setParameter(const juce::String& paramId, float value) override {
        if (paramId == "roomSize") setRoomSize(value);
        else if (paramId == "damping") setDamping(value);
        else if (paramId == "wetLevel") setWetLevel(value);
        else if (paramId == "preDelay") setPreDelay(value);
        else if (paramId == "width") setWidth(value);
    }

private:
    void updateParameters() {
        // Map room size to feedback (0.7 - 0.99)
        float feedback = 0.7f + roomSize * 0.28f;
        
        for (int i = 0; i < NUM_COMBS; ++i) {
            combsL[i].setFeedback(feedback);
            combsL[i].setDamping(damping);
            combsR[i].setFeedback(feedback);
            combsR[i].setDamping(damping);
        }
    }
    
    // Parameters
    float roomSize = 0.5f;
    float damping = 0.5f;
    float wetLevel = 0.3f;
    float preDelayMs = 10.0f;
    float width = 1.0f;
    
    // Processing
    std::array<CombFilter, NUM_COMBS> combsL;
    std::array<CombFilter, NUM_COMBS> combsR;
    std::array<AllPassFilter, NUM_ALLPASSES> allpassesL;
    std::array<AllPassFilter, NUM_ALLPASSES> allpassesR;
    DSP::DelayLine preDelayL;
    DSP::DelayLine preDelayR;
    DSP::OnePole inputFilterL;
    DSP::OnePole inputFilterR;
};

// Register the Reverb module
REGISTER_FX_MODULE(ReverbModule, ModuleType::Reverb, "Reverb")

} // namespace FX
} // namespace Sphere

