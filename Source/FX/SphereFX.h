#pragma once

// ============================================================================
// Sphere FX - Master Include File
// 
// Include this single file to get access to all FX modules
// ============================================================================

// Common base classes and utilities
#include "Common/SphereFXTypes.h"
#include "Common/SphereFXModule.h"
#include "Common/SphereFXDSP.h"

// Module Manager
#include "ModuleManager/SphereFXChain.h"

// Individual FX Modules
#include "EQ/SphereEQModule.h"
#include "Compressor/SphereCompressorModule.h"
#include "Reverb/SphereReverbModule.h"
#include "Delay/SphereDelayModule.h"

namespace Sphere {
namespace FX {

// ============================================================================
// Factory function to create all available modules
// ============================================================================
inline std::unique_ptr<IFXModule> createModule(ModuleType type) {
    return ModuleRegistry::getInstance().createModule(type);
}

// ============================================================================
// Helper to create a default FX chain with common effects
// ============================================================================
inline std::unique_ptr<FXChain> createDefaultChain() {
    auto chain = std::make_unique<FXChain>();
    
    // Add default modules in typical signal flow order
    chain->addModule(std::make_unique<EQModule>());
    chain->addModule(std::make_unique<CompressorModule>());
    chain->addModule(std::make_unique<DelayModule>());
    chain->addModule(std::make_unique<ReverbModule>());
    
    return chain;
}

// ============================================================================
// Get list of available module types
// ============================================================================
inline std::vector<std::pair<ModuleType, juce::String>> getAvailableModules() {
    return ModuleRegistry::getInstance().getAvailableModules();
}

} // namespace FX
} // namespace Sphere







