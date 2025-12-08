#pragma once
#include <JuceHeader.h>
#include <atomic>
#include <vector>
#include <string>
#include <functional>

namespace Sphere {
namespace FX {

// ============================================================================
// Module Types - Each FX module has a unique type
// ============================================================================
enum class ModuleType {
    EQ,
    Compressor,
    Reverb,
    Delay,
    Chorus,
    Phaser,
    Flanger,
    Distortion,
    Limiter,
    Gate,
    Custom
};

inline const char* moduleTypeToString(ModuleType type) {
    switch (type) {
        case ModuleType::EQ:         return "EQ";
        case ModuleType::Compressor: return "Compressor";
        case ModuleType::Reverb:     return "Reverb";
        case ModuleType::Delay:      return "Delay";
        case ModuleType::Chorus:     return "Chorus";
        case ModuleType::Phaser:     return "Phaser";
        case ModuleType::Flanger:    return "Flanger";
        case ModuleType::Distortion: return "Distortion";
        case ModuleType::Limiter:    return "Limiter";
        case ModuleType::Gate:       return "Gate";
        case ModuleType::Custom:     return "Custom";
        default:                     return "Unknown";
    }
}

// ============================================================================
// Processing Mode
// ============================================================================
enum class ProcessingMode {
    Stereo,         // Normal stereo processing
    Mono,           // Mono (sum to mono, process, split)
    MidSide,        // Mid/Side processing
    DualMono        // Process L and R independently
};

// ============================================================================
// Parameter Types
// ============================================================================
enum class ParameterType {
    Float,          // Continuous value (0.0 - 1.0 normalized)
    Int,            // Integer value
    Bool,           // On/Off toggle
    Choice          // Selection from list
};

// ============================================================================
// Parameter Definition
// ============================================================================
struct ParameterDef {
    juce::String id;                    // Unique parameter ID
    juce::String name;                  // Display name
    juce::String unit;                  // Unit (dB, Hz, ms, %, etc.)
    ParameterType type = ParameterType::Float;
    
    float minValue = 0.0f;
    float maxValue = 1.0f;
    float defaultValue = 0.0f;
    float skewFactor = 1.0f;            // For logarithmic controls
    
    juce::StringArray choices;          // For Choice type
    
    std::function<juce::String(float)> valueToText = nullptr;
    std::function<float(const juce::String&)> textToValue = nullptr;
};

// ============================================================================
// Module State (for preset save/load)
// ============================================================================
struct ModuleState {
    juce::String moduleId;
    ModuleType type;
    bool enabled = true;
    bool bypassed = false;
    float dryWetMix = 1.0f;
    std::map<juce::String, float> parameterValues;
    
    juce::var toVar() const {
        auto obj = new juce::DynamicObject();
        obj->setProperty("moduleId", moduleId);
        obj->setProperty("type", static_cast<int>(type));
        obj->setProperty("enabled", enabled);
        obj->setProperty("bypassed", bypassed);
        obj->setProperty("dryWetMix", dryWetMix);
        
        auto params = new juce::DynamicObject();
        for (const auto& [key, value] : parameterValues) {
            params->setProperty(key, value);
        }
        obj->setProperty("parameters", juce::var(params));
        
        return juce::var(obj);
    }
    
    static ModuleState fromVar(const juce::var& v) {
        ModuleState state;
        if (auto* obj = v.getDynamicObject()) {
            state.moduleId = obj->getProperty("moduleId").toString();
            state.type = static_cast<ModuleType>(static_cast<int>(obj->getProperty("type")));
            state.enabled = obj->getProperty("enabled");
            state.bypassed = obj->getProperty("bypassed");
            state.dryWetMix = obj->getProperty("dryWetMix");
            
            if (auto* params = obj->getProperty("parameters").getDynamicObject()) {
                for (const auto& prop : params->getProperties()) {
                    state.parameterValues[prop.name.toString()] = prop.value;
                }
            }
        }
        return state;
    }
};

// ============================================================================
// Audio Context - Passed to modules during processing
// ============================================================================
struct AudioContext {
    double sampleRate = 44100.0;
    int blockSize = 512;
    int numChannels = 2;
    double bpm = 120.0;                 // For tempo-synced effects
    double ppqPosition = 0.0;           // For tempo-synced effects
    bool isPlaying = false;
};

// ============================================================================
// Processing Statistics (for metering/visualization)
// ============================================================================
struct ProcessingStats {
    float inputLevelL = 0.0f;
    float inputLevelR = 0.0f;
    float outputLevelL = 0.0f;
    float outputLevelR = 0.0f;
    float gainReduction = 0.0f;         // For compressors/limiters
    float cpuUsage = 0.0f;              // Optional CPU monitoring
};

// ============================================================================
// Constants
// ============================================================================
namespace Constants {
    constexpr double MIN_FREQUENCY = 20.0;
    constexpr double MAX_FREQUENCY = 20000.0;
    constexpr double MIN_Q = 0.1;
    constexpr double MAX_Q = 30.0;
    constexpr double MIN_GAIN_DB = -60.0;
    constexpr double MAX_GAIN_DB = 24.0;
    constexpr double DENORMAL_THRESHOLD = 1e-15;
    constexpr int MAX_BLOCK_SIZE = 8192;
    constexpr int MAX_CHANNELS = 2;
}

// ============================================================================
// Utility Functions
// ============================================================================
namespace Utils {
    inline float dbToLinear(float db) {
        return std::pow(10.0f, db / 20.0f);
    }
    
    inline float linearToDb(float linear) {
        return (linear > 0.0f) ? 20.0f * std::log10(linear) : -100.0f;
    }
    
    inline double dbToLinear(double db) {
        return std::pow(10.0, db / 20.0);
    }
    
    inline double linearToDb(double linear) {
        return (linear > 0.0) ? 20.0 * std::log10(linear) : -100.0;
    }
    
    inline float flushDenormal(float value) {
        return (std::abs(value) < static_cast<float>(Constants::DENORMAL_THRESHOLD)) ? 0.0f : value;
    }
    
    inline double flushDenormal(double value) {
        return (std::abs(value) < Constants::DENORMAL_THRESHOLD) ? 0.0 : value;
    }
    
    // Fast approximations
    inline float fastLog2(float x) {
        union { float f; uint32_t i; } vx = { x };
        float y = static_cast<float>(vx.i);
        y *= 1.1920928955078125e-7f;
        return y - 126.94269504f;
    }
    
    inline float fastPow2(float p) {
        float clipp = (p < -126.0f) ? -126.0f : p;
        union { uint32_t i; float f; } v;
        v.i = static_cast<uint32_t>((1 << 23) * (clipp + 126.94269504f));
        return v.f;
    }
    
    inline float fastTanh(float x) {
        float x2 = x * x;
        return x * (27.0f + x2) / (27.0f + 9.0f * x2);
    }
}

} // namespace FX
} // namespace Sphere

