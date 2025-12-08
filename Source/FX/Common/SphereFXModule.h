#pragma once
#include "SphereFXTypes.h"
#include <memory>
#include <mutex>

namespace Sphere {
namespace FX {

// ============================================================================
// Base FX Module Interface
// All FX modules must implement this interface
// ============================================================================
class IFXModule {
public:
    virtual ~IFXModule() = default;
    
    // ========================================================================
    // Identity
    // ========================================================================
    virtual ModuleType getType() const = 0;
    virtual juce::String getName() const = 0;
    virtual juce::String getId() const = 0;
    virtual juce::String getVersion() const { return "1.0.0"; }
    virtual juce::String getAuthor() const { return "Sphere DSP"; }
    
    // ========================================================================
    // Lifecycle
    // ========================================================================
    virtual void prepare(const AudioContext& context) = 0;
    virtual void reset() = 0;
    virtual void release() {}
    
    // ========================================================================
    // Processing
    // ========================================================================
    virtual void processBlock(juce::AudioBuffer<float>& buffer, 
                              juce::MidiBuffer& midi) = 0;
    
    // ========================================================================
    // State
    // ========================================================================
    virtual bool isEnabled() const = 0;
    virtual void setEnabled(bool enabled) = 0;
    virtual bool isBypassed() const = 0;
    virtual void setBypassed(bool bypassed) = 0;
    
    // ========================================================================
    // Parameters
    // ========================================================================
    virtual std::vector<ParameterDef> getParameterDefinitions() const = 0;
    virtual float getParameter(const juce::String& paramId) const = 0;
    virtual void setParameter(const juce::String& paramId, float value) = 0;
    
    // ========================================================================
    // State Save/Load
    // ========================================================================
    virtual ModuleState getState() const = 0;
    virtual void setState(const ModuleState& state) = 0;
    
    // ========================================================================
    // Metering/Visualization
    // ========================================================================
    virtual ProcessingStats getStats() const { return {}; }
    virtual int getLatencySamples() const { return 0; }
};

// ============================================================================
// Base FX Module Implementation
// Provides common functionality for all FX modules
// ============================================================================
class FXModuleBase : public IFXModule {
public:
    FXModuleBase(ModuleType type, const juce::String& name, const juce::String& id)
        : moduleType(type), moduleName(name), moduleId(id) {}
    
    ~FXModuleBase() override = default;
    
    // ========================================================================
    // Identity (implemented)
    // ========================================================================
    ModuleType getType() const override { return moduleType; }
    juce::String getName() const override { return moduleName; }
    juce::String getId() const override { return moduleId; }
    
    // ========================================================================
    // State (implemented)
    // ========================================================================
    bool isEnabled() const override { return enabled.load(std::memory_order_relaxed); }
    void setEnabled(bool value) override { enabled.store(value, std::memory_order_relaxed); }
    bool isBypassed() const override { return bypassed.load(std::memory_order_relaxed); }
    void setBypassed(bool value) override { bypassed.store(value, std::memory_order_relaxed); }
    
    // ========================================================================
    // Dry/Wet Mix
    // ========================================================================
    float getDryWetMix() const { return dryWetMix.load(std::memory_order_relaxed); }
    void setDryWetMix(float mix) { 
        dryWetMix.store(juce::jlimit(0.0f, 1.0f, mix), std::memory_order_relaxed); 
    }
    
    // ========================================================================
    // State Save/Load (base implementation)
    // ========================================================================
    ModuleState getState() const override {
        ModuleState state;
        state.moduleId = moduleId;
        state.type = moduleType;
        state.enabled = enabled.load();
        state.bypassed = bypassed.load();
        state.dryWetMix = dryWetMix.load();
        
        for (const auto& def : getParameterDefinitions()) {
            state.parameterValues[def.id] = getParameter(def.id);
        }
        
        return state;
    }
    
    void setState(const ModuleState& state) override {
        setEnabled(state.enabled);
        setBypassed(state.bypassed);
        setDryWetMix(state.dryWetMix);
        
        for (const auto& [paramId, value] : state.parameterValues) {
            setParameter(paramId, value);
        }
    }
    
    // ========================================================================
    // Processing with bypass/dry-wet handling
    // ========================================================================
    void processBlockWithBypass(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) {
        if (!enabled.load(std::memory_order_relaxed)) {
            return;
        }
        
        if (bypassed.load(std::memory_order_relaxed)) {
            return;
        }
        
        const float mix = dryWetMix.load(std::memory_order_relaxed);
        
        if (mix < 0.001f) {
            return; // Full dry
        }
        
        if (mix > 0.999f) {
            // Full wet - no need for dry buffer
            processBlock(buffer, midi);
            return;
        }
        
        // Dry/Wet mix - need to store dry signal
        juce::AudioBuffer<float> dryBuffer;
        dryBuffer.makeCopyOf(buffer);
        
        processBlock(buffer, midi);
        
        // Mix dry and wet
        const float dryGain = 1.0f - mix;
        const float wetGain = mix;
        
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch) {
            auto* wet = buffer.getWritePointer(ch);
            const auto* dry = dryBuffer.getReadPointer(ch);
            
            for (int i = 0; i < buffer.getNumSamples(); ++i) {
                wet[i] = dry[i] * dryGain + wet[i] * wetGain;
            }
        }
    }

protected:
    ModuleType moduleType;
    juce::String moduleName;
    juce::String moduleId;
    
    std::atomic<bool> enabled{true};
    std::atomic<bool> bypassed{false};
    std::atomic<float> dryWetMix{1.0f};
    
    AudioContext audioContext;
    mutable std::mutex parameterMutex;
};

// ============================================================================
// Module Factory Function Type
// Each module provides a factory function to create instances
// ============================================================================
using ModuleFactoryFunc = std::function<std::unique_ptr<IFXModule>()>;

} // namespace FX
} // namespace Sphere

