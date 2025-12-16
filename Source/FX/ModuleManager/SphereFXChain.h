#pragma once
#include "../Common/SphereFXModule.h"
#include <vector>
#include <memory>
#include <map>

namespace Sphere {
namespace FX {

// ============================================================================
// FX Chain - Manages a chain of FX modules in series
// ============================================================================
class FXChain {
public:
    FXChain() = default;
    ~FXChain() = default;
    
    // Non-copyable, movable
    FXChain(const FXChain&) = delete;
    FXChain& operator=(const FXChain&) = delete;
    FXChain(FXChain&&) = default;
    FXChain& operator=(FXChain&&) = default;
    
    // ========================================================================
    // Lifecycle
    // ========================================================================
    void prepare(const AudioContext& context) {
        audioContext = context;
        for (auto& module : modules) {
            if (module) {
                module->prepare(context);
            }
        }
        prepared = true;
    }
    
    void reset() {
        for (auto& module : modules) {
            if (module) {
                module->reset();
            }
        }
    }
    
    void release() {
        for (auto& module : modules) {
            if (module) {
                module->release();
            }
        }
        prepared = false;
    }
    
    // ========================================================================
    // Module Management
    // ========================================================================
    
    // Add module to end of chain
    void addModule(std::unique_ptr<IFXModule> module) {
        if (module) {
            if (prepared) {
                module->prepare(audioContext);
            }
            modules.push_back(std::move(module));
        }
    }
    
    // Insert module at specific position
    void insertModule(size_t index, std::unique_ptr<IFXModule> module) {
        if (module) {
            if (prepared) {
                module->prepare(audioContext);
            }
            if (index >= modules.size()) {
                modules.push_back(std::move(module));
            } else {
                modules.insert(modules.begin() + index, std::move(module));
            }
        }
    }
    
    // Remove module by index
    std::unique_ptr<IFXModule> removeModule(size_t index) {
        if (index < modules.size()) {
            auto module = std::move(modules[index]);
            modules.erase(modules.begin() + index);
            return module;
        }
        return nullptr;
    }
    
    // Remove module by ID
    std::unique_ptr<IFXModule> removeModuleById(const juce::String& moduleId) {
        for (size_t i = 0; i < modules.size(); ++i) {
            if (modules[i] && modules[i]->getId() == moduleId) {
                return removeModule(i);
            }
        }
        return nullptr;
    }
    
    // Move module to new position
    void moveModule(size_t fromIndex, size_t toIndex) {
        if (fromIndex >= modules.size() || toIndex >= modules.size()) return;
        if (fromIndex == toIndex) return;
        
        auto module = std::move(modules[fromIndex]);
        modules.erase(modules.begin() + fromIndex);
        
        if (toIndex > fromIndex) --toIndex;
        modules.insert(modules.begin() + toIndex, std::move(module));
    }
    
    // Get module by index
    IFXModule* getModule(size_t index) {
        return (index < modules.size()) ? modules[index].get() : nullptr;
    }
    
    const IFXModule* getModule(size_t index) const {
        return (index < modules.size()) ? modules[index].get() : nullptr;
    }
    
    // Get module by ID
    IFXModule* getModuleById(const juce::String& moduleId) {
        for (auto& module : modules) {
            if (module && module->getId() == moduleId) {
                return module.get();
            }
        }
        return nullptr;
    }
    
    // Get module by type (returns first match)
    IFXModule* getModuleByType(ModuleType type) {
        for (auto& module : modules) {
            if (module && module->getType() == type) {
                return module.get();
            }
        }
        return nullptr;
    }
    
    size_t getModuleCount() const { return modules.size(); }
    
    bool isEmpty() const { return modules.empty(); }
    
    void clear() {
        modules.clear();
    }
    
    // ========================================================================
    // Processing
    // ========================================================================
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) {
        if (!enabled) return;
        
        juce::ScopedNoDenormals noDenormals;
        
        for (auto& module : modules) {
            if (module && module->isEnabled() && !module->isBypassed()) {
                module->processBlock(buffer, midi);
            }
        }
    }
    
    // Process with dry/wet handling per module
    void processBlockWithMix(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) {
        if (!enabled) return;
        
        juce::ScopedNoDenormals noDenormals;
        
        for (auto& module : modules) {
            if (module) {
                if (auto* baseModule = dynamic_cast<FXModuleBase*>(module.get())) {
                    baseModule->processBlockWithBypass(buffer, midi);
                } else if (module->isEnabled() && !module->isBypassed()) {
                    module->processBlock(buffer, midi);
                }
            }
        }
    }
    
    // ========================================================================
    // Chain State
    // ========================================================================
    bool isEnabled() const { return enabled; }
    void setEnabled(bool value) { enabled = value; }
    
    // ========================================================================
    // State Save/Load
    // ========================================================================
    juce::var getState() const {
        juce::Array<juce::var> moduleStates;
        for (const auto& module : modules) {
            if (module) {
                moduleStates.add(module->getState().toVar());
            }
        }
        
        auto obj = new juce::DynamicObject();
        obj->setProperty("enabled", enabled);
        obj->setProperty("modules", moduleStates);
        return juce::var(obj);
    }
    
    // Note: setState requires a module factory to recreate modules
    // This is handled by FXChainManager
    
    // ========================================================================
    // Total Latency
    // ========================================================================
    int getTotalLatencySamples() const {
        int total = 0;
        for (const auto& module : modules) {
            if (module) {
                total += module->getLatencySamples();
            }
        }
        return total;
    }
    
    // ========================================================================
    // Iteration
    // ========================================================================
    auto begin() { return modules.begin(); }
    auto end() { return modules.end(); }
    auto begin() const { return modules.begin(); }
    auto end() const { return modules.end(); }

private:
    std::vector<std::unique_ptr<IFXModule>> modules;
    AudioContext audioContext;
    bool prepared = false;
    bool enabled = true;
};

// ============================================================================
// Module Registry - Stores factories for available module types
// ============================================================================
class ModuleRegistry {
public:
    static ModuleRegistry& getInstance() {
        static ModuleRegistry instance;
        return instance;
    }
    
    // Register a module factory
    void registerModule(ModuleType type, const juce::String& name, ModuleFactoryFunc factory) {
        factories[type] = { name, std::move(factory) };
    }
    
    // Create a module instance
    std::unique_ptr<IFXModule> createModule(ModuleType type) {
        auto it = factories.find(type);
        if (it != factories.end()) {
            return it->second.factory();
        }
        return nullptr;
    }
    
    // Get available module types
    std::vector<std::pair<ModuleType, juce::String>> getAvailableModules() const {
        std::vector<std::pair<ModuleType, juce::String>> result;
        for (const auto& [type, info] : factories) {
            result.push_back({ type, info.name });
        }
        return result;
    }
    
    bool hasModule(ModuleType type) const {
        return factories.find(type) != factories.end();
    }

private:
    ModuleRegistry() = default;
    
    struct ModuleInfo {
        juce::String name;
        ModuleFactoryFunc factory;
    };
    
    std::map<ModuleType, ModuleInfo> factories;
};

// ============================================================================
// Helper macro for module registration
// ============================================================================
#define REGISTER_FX_MODULE(ModuleClass, Type, Name) \
    namespace { \
        static bool _##ModuleClass##_registered = []() { \
            Sphere::FX::ModuleRegistry::getInstance().registerModule( \
                Type, Name, []() { return std::make_unique<ModuleClass>(); }); \
            return true; \
        }(); \
    }

} // namespace FX
} // namespace Sphere







