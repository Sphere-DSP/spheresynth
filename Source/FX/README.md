# Sphere FX - Modular Audio Effects System

## Overview

The Sphere FX system provides a modular architecture for audio effects processing. Each effect is implemented as a self-contained module that can be developed, tested, and maintained independently.

## Directory Structure

```
FX/
├── Common/                     # Shared base classes and utilities
│   ├── SphereFXTypes.h        # Common types, enums, and constants
│   ├── SphereFXModule.h       # Base module interface and implementation
│   └── SphereFXDSP.h          # DSP building blocks (filters, delays, etc.)
│
├── ModuleManager/              # Module management system
│   └── SphereFXChain.h        # FX chain and module registry
│
├── EQ/                         # Parametric EQ module
│   └── SphereEQModule.h
│
├── Compressor/                 # Dynamics compressor module
│   └── SphereCompressorModule.h
│
├── Reverb/                     # Reverb module
│   └── SphereReverbModule.h
│
├── Delay/                      # Delay module
│   └── SphereDelayModule.h
│
├── SphereFX.h                  # Master include file
└── README.md                   # This file
```

## Usage

### Basic Usage

```cpp
#include "FX/SphereFX.h"

// In your audio processor:
class MyProcessor {
    Sphere::FX::FXChain fxChain;
    
    void prepareToPlay(double sampleRate, int blockSize) {
        Sphere::FX::AudioContext context;
        context.sampleRate = sampleRate;
        context.blockSize = blockSize;
        context.numChannels = 2;
        
        // Add modules
        fxChain.addModule(std::make_unique<Sphere::FX::EQModule>());
        fxChain.addModule(std::make_unique<Sphere::FX::CompressorModule>());
        fxChain.addModule(std::make_unique<Sphere::FX::ReverbModule>());
        
        // Prepare chain
        fxChain.prepare(context);
    }
    
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) {
        fxChain.processBlock(buffer, midi);
    }
};
```

### Creating a New Module

1. Create a new folder under `FX/` (e.g., `FX/MyEffect/`)
2. Create your module header following this template:

```cpp
#pragma once
#include "../Common/SphereFXModule.h"
#include "../Common/SphereFXDSP.h"

namespace Sphere {
namespace FX {

class MyEffectModule : public FXModuleBase {
public:
    MyEffectModule() 
        : FXModuleBase(ModuleType::Custom, "My Effect", "sphere.fx.myeffect") {}
    
    // Required: Lifecycle methods
    void prepare(const AudioContext& context) override {
        // Initialize DSP components with sample rate, block size, etc.
    }
    
    void reset() override {
        // Clear any state (delay lines, filters, etc.)
    }
    
    // Required: Processing
    void processBlock(juce::AudioBuffer<float>& buffer, 
                      juce::MidiBuffer& midi) override {
        // Your DSP processing here
    }
    
    // Required: Parameters
    std::vector<ParameterDef> getParameterDefinitions() const override {
        return {
            {"param1", "Parameter 1", "Hz", ParameterType::Float, 20.0f, 20000.0f, 1000.0f},
            {"param2", "Parameter 2", "%", ParameterType::Float, 0.0f, 1.0f, 0.5f}
        };
    }
    
    float getParameter(const juce::String& paramId) const override {
        if (paramId == "param1") return param1;
        if (paramId == "param2") return param2;
        return 0.0f;
    }
    
    void setParameter(const juce::String& paramId, float value) override {
        if (paramId == "param1") param1 = value;
        else if (paramId == "param2") param2 = value;
    }

private:
    float param1 = 1000.0f;
    float param2 = 0.5f;
};

// Register the module
REGISTER_FX_MODULE(MyEffectModule, ModuleType::Custom, "My Effect")

} // namespace FX
} // namespace Sphere
```

3. Include your module in `SphereFX.h`:

```cpp
#include "MyEffect/MyEffectModule.h"
```

## Module Interface

Every module must implement `IFXModule`:

| Method | Description |
|--------|-------------|
| `getType()` | Returns the module type |
| `getName()` | Human-readable name |
| `getId()` | Unique identifier |
| `prepare(context)` | Initialize with audio context |
| `reset()` | Clear all state |
| `processBlock(buffer, midi)` | Process audio |
| `isEnabled()` / `setEnabled()` | Enable/disable |
| `isBypassed()` / `setBypassed()` | Bypass processing |
| `getParameterDefinitions()` | List all parameters |
| `getParameter(id)` / `setParameter(id, value)` | Get/set parameters |
| `getState()` / `setState()` | Save/load presets |

## DSP Building Blocks

`SphereFXDSP.h` provides common DSP components:

- **Biquad Filter** - Second-order IIR filter
- **RBJ Filter Coefficients** - Low/high pass, peak, shelving
- **Smoothed Value** - Parameter smoothing
- **Envelope Follower** - For dynamics processing
- **Delay Line** - With fractional delay support
- **One-Pole Filter** - Simple smoothing filter

## Best Practices

1. **Thread Safety**: Use atomic operations for parameters accessed from audio thread
2. **Denormals**: Always use `juce::ScopedNoDenormals` in processBlock
3. **Smoothing**: Smooth parameter changes to avoid clicks
4. **Reset**: Implement reset() to clear all filter states and delay lines
5. **Efficiency**: Avoid allocations in processBlock

## Module Types

Currently supported module types:
- `EQ` - Parametric equalizer
- `Compressor` - Dynamics compressor
- `Reverb` - Algorithmic reverb
- `Delay` - Delay effect
- `Chorus` - Chorus effect (planned)
- `Phaser` - Phaser effect (planned)
- `Flanger` - Flanger effect (planned)
- `Distortion` - Distortion/saturation (planned)
- `Limiter` - Brick-wall limiter (planned)
- `Gate` - Noise gate (planned)
- `Custom` - For custom effects

