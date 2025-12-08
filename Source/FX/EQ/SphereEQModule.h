#pragma once
#include "../Common/SphereFXModule.h"
#include "../Common/SphereFXDSP.h"
#include <array>

namespace Sphere {
namespace FX {

// ============================================================================
// EQ Band Types
// ============================================================================
enum class EQBandType {
    Bell,
    LowShelf,
    HighShelf,
    LowCut,
    HighCut,
    Notch,
    BandPass,
    AllPass
};

// ============================================================================
// EQ Filter Slope
// ============================================================================
enum class EQSlope {
    dB6  = 1,
    dB12 = 2,
    dB18 = 3,
    dB24 = 4,
    dB36 = 6,
    dB48 = 8
};

// ============================================================================
// EQ Band Parameters
// ============================================================================
struct EQBandParams {
    bool enabled = false;
    EQBandType type = EQBandType::Bell;
    double frequency = 1000.0;
    double gain = 0.0;      // dB
    double q = 1.0;
    EQSlope slope = EQSlope::dB12;  // For cut filters
    
    bool operator==(const EQBandParams& other) const {
        return enabled == other.enabled && type == other.type &&
               std::abs(frequency - other.frequency) < 0.1 &&
               std::abs(gain - other.gain) < 0.01 &&
               std::abs(q - other.q) < 0.001 &&
               slope == other.slope;
    }
    
    bool operator!=(const EQBandParams& other) const {
        return !(*this == other);
    }
};

// ============================================================================
// EQ Band Processor - Single band of EQ
// ============================================================================
class EQBandProcessor {
public:
    static constexpr int MAX_STAGES = 4;  // For up to 48 dB/oct
    
    void prepare(double sampleRate) {
        this->sampleRate = sampleRate;
        for (auto& stage : stagesL) stage.reset();
        for (auto& stage : stagesR) stage.reset();
    }
    
    void reset() {
        for (auto& stage : stagesL) stage.reset();
        for (auto& stage : stagesR) stage.reset();
    }
    
    void setParameters(const EQBandParams& newParams) {
        if (params != newParams) {
            params = newParams;
            updateCoefficients();
        }
    }
    
    void processBlock(float* left, float* right, int numSamples) {
        if (!params.enabled) return;
        
        for (int stage = 0; stage < numActiveStages; ++stage) {
            stagesL[stage].processBlock(left, numSamples);
            if (right) stagesR[stage].processBlock(right, numSamples);
        }
    }
    
    const EQBandParams& getParameters() const { return params; }
    
    double getMagnitudeResponse(double freq) const {
        if (!params.enabled) return 1.0;
        
        double mag = 1.0;
        for (int i = 0; i < numActiveStages; ++i) {
            // Calculate magnitude response for each stage
            const auto& coeffs = stageCoeffs[i];
            const double w = 2.0 * juce::MathConstants<double>::pi * freq / sampleRate;
            const double cosW = std::cos(w);
            const double cos2W = std::cos(2.0 * w);
            const double sinW = std::sin(w);
            const double sin2W = std::sin(2.0 * w);
            
            double numR = coeffs.b0 + coeffs.b1 * cosW + coeffs.b2 * cos2W;
            double numI = -coeffs.b1 * sinW - coeffs.b2 * sin2W;
            double denR = 1.0 + coeffs.a1 * cosW + coeffs.a2 * cos2W;
            double denI = -coeffs.a1 * sinW - coeffs.a2 * sin2W;
            
            double numMag = std::sqrt(numR * numR + numI * numI);
            double denMag = std::sqrt(denR * denR + denI * denI);
            
            if (denMag > 1e-10) mag *= numMag / denMag;
        }
        return mag;
    }

private:
    void updateCoefficients() {
        if (sampleRate <= 0.0) return;
        
        numActiveStages = 1;
        
        switch (params.type) {
            case EQBandType::Bell:
                stageCoeffs[0] = DSP::RBJ::makePeakEQ(sampleRate, params.frequency, params.q, params.gain);
                break;
                
            case EQBandType::LowShelf:
                stageCoeffs[0] = DSP::RBJ::makeLowShelf(sampleRate, params.frequency, params.q, params.gain);
                break;
                
            case EQBandType::HighShelf:
                stageCoeffs[0] = DSP::RBJ::makeHighShelf(sampleRate, params.frequency, params.q, params.gain);
                break;
                
            case EQBandType::LowCut:
                numActiveStages = setupCutFilter(true);
                break;
                
            case EQBandType::HighCut:
                numActiveStages = setupCutFilter(false);
                break;
                
            case EQBandType::Notch:
                stageCoeffs[0] = DSP::RBJ::makeBandPass(sampleRate, params.frequency, params.q * 10.0);
                // Invert for notch
                stageCoeffs[0].b0 = 1.0 - stageCoeffs[0].b0;
                break;
                
            case EQBandType::BandPass:
                stageCoeffs[0] = DSP::RBJ::makeBandPass(sampleRate, params.frequency, params.q);
                break;
                
            case EQBandType::AllPass:
                stageCoeffs[0] = DSP::RBJ::makeAllPass(sampleRate, params.frequency, params.q);
                break;
        }
        
        // Apply coefficients to filters
        for (int i = 0; i < numActiveStages; ++i) {
            stagesL[i].setCoefficients(stageCoeffs[i]);
            stagesR[i].setCoefficients(stageCoeffs[i]);
        }
    }
    
    int setupCutFilter(bool isHighPass) {
        int order = static_cast<int>(params.slope);
        int numStages = (order + 1) / 2;
        numStages = juce::jlimit(1, MAX_STAGES, numStages);
        
        for (int i = 0; i < numStages; ++i) {
            // Butterworth Q for cascaded sections
            double q = 1.0 / (2.0 * std::cos(juce::MathConstants<double>::pi * (2.0 * i + 1.0) / (2.0 * order)));
            q = juce::jlimit(0.5, 10.0, q);
            
            if (isHighPass) {
                stageCoeffs[i] = DSP::RBJ::makeHighPass(sampleRate, params.frequency, q);
            } else {
                stageCoeffs[i] = DSP::RBJ::makeLowPass(sampleRate, params.frequency, q);
            }
        }
        
        return numStages;
    }
    
    double sampleRate = 44100.0;
    EQBandParams params;
    int numActiveStages = 1;
    
    std::array<DSP::Biquad, MAX_STAGES> stagesL;
    std::array<DSP::Biquad, MAX_STAGES> stagesR;
    std::array<DSP::BiquadCoeffs, MAX_STAGES> stageCoeffs;
};

// ============================================================================
// EQ Module - Full parametric EQ
// ============================================================================
class EQModule : public FXModuleBase {
public:
    static constexpr int NUM_BANDS = 8;
    
    EQModule() 
        : FXModuleBase(ModuleType::EQ, "Parametric EQ", "sphere.fx.eq") {}
    
    // ========================================================================
    // Lifecycle
    // ========================================================================
    void prepare(const AudioContext& context) override {
        audioContext = context;
        
        for (auto& band : bands) {
            band.prepare(context.sampleRate);
        }
        
        outputGain.reset(context.sampleRate, 0.02);
        outputGain.setCurrentAndTargetValue(1.0f);
    }
    
    void reset() override {
        for (auto& band : bands) {
            band.reset();
        }
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
        
        // Process each band
        for (auto& band : bands) {
            band.processBlock(left, right, numSamples);
        }
        
        // Apply output gain
        if (outputGain.isSmoothing() || std::abs(outputGain.getTargetValue() - 1.0f) > 0.001f) {
            for (int i = 0; i < numSamples; ++i) {
                float g = outputGain.getNextValue();
                left[i] *= g;
                if (right) right[i] *= g;
            }
        }
    }
    
    // ========================================================================
    // Band Control
    // ========================================================================
    void setBandParameters(int bandIndex, const EQBandParams& params) {
        if (bandIndex >= 0 && bandIndex < NUM_BANDS) {
            bands[bandIndex].setParameters(params);
        }
    }
    
    const EQBandParams& getBandParameters(int bandIndex) const {
        static EQBandParams defaultParams;
        if (bandIndex >= 0 && bandIndex < NUM_BANDS) {
            return bands[bandIndex].getParameters();
        }
        return defaultParams;
    }
    
    void setBandEnabled(int bandIndex, bool enabled) {
        if (bandIndex >= 0 && bandIndex < NUM_BANDS) {
            auto params = bands[bandIndex].getParameters();
            params.enabled = enabled;
            bands[bandIndex].setParameters(params);
        }
    }
    
    void setBandFrequency(int bandIndex, double freq) {
        if (bandIndex >= 0 && bandIndex < NUM_BANDS) {
            auto params = bands[bandIndex].getParameters();
            params.frequency = juce::jlimit(Constants::MIN_FREQUENCY, Constants::MAX_FREQUENCY, freq);
            bands[bandIndex].setParameters(params);
        }
    }
    
    void setBandGain(int bandIndex, double gainDb) {
        if (bandIndex >= 0 && bandIndex < NUM_BANDS) {
            auto params = bands[bandIndex].getParameters();
            params.gain = juce::jlimit(-24.0, 24.0, gainDb);
            bands[bandIndex].setParameters(params);
        }
    }
    
    void setBandQ(int bandIndex, double q) {
        if (bandIndex >= 0 && bandIndex < NUM_BANDS) {
            auto params = bands[bandIndex].getParameters();
            params.q = juce::jlimit(Constants::MIN_Q, Constants::MAX_Q, q);
            bands[bandIndex].setParameters(params);
        }
    }
    
    void setBandType(int bandIndex, EQBandType type) {
        if (bandIndex >= 0 && bandIndex < NUM_BANDS) {
            auto params = bands[bandIndex].getParameters();
            params.type = type;
            bands[bandIndex].setParameters(params);
        }
    }
    
    void setOutputGain(float gainDb) {
        outputGain.setTargetValue(Utils::dbToLinear(gainDb));
    }
    
    // ========================================================================
    // Analysis
    // ========================================================================
    double getMagnitudeResponse(double frequency) const {
        double totalMag = 1.0;
        for (const auto& band : bands) {
            totalMag *= band.getMagnitudeResponse(frequency);
        }
        return totalMag * outputGain.getCurrentValue();
    }
    
    double getMagnitudeResponseDb(double frequency) const {
        return Utils::linearToDb(getMagnitudeResponse(frequency));
    }
    
    // ========================================================================
    // Parameters (IFXModule interface)
    // ========================================================================
    std::vector<ParameterDef> getParameterDefinitions() const override {
        std::vector<ParameterDef> params;
        
        params.push_back({"outputGain", "Output Gain", "dB", ParameterType::Float, -24.0f, 24.0f, 0.0f});
        
        for (int i = 0; i < NUM_BANDS; ++i) {
            juce::String prefix = "band" + juce::String(i + 1) + "_";
            params.push_back({prefix + "enabled", "Band " + juce::String(i + 1) + " Enable", "", ParameterType::Bool, 0.0f, 1.0f, 0.0f});
            params.push_back({prefix + "type", "Type", "", ParameterType::Choice, 0.0f, 7.0f, 0.0f});
            params.push_back({prefix + "freq", "Frequency", "Hz", ParameterType::Float, 20.0f, 20000.0f, 1000.0f, 0.3f});
            params.push_back({prefix + "gain", "Gain", "dB", ParameterType::Float, -24.0f, 24.0f, 0.0f});
            params.push_back({prefix + "q", "Q", "", ParameterType::Float, 0.1f, 30.0f, 1.0f, 0.5f});
        }
        
        return params;
    }
    
    float getParameter(const juce::String& paramId) const override {
        if (paramId == "outputGain") {
            return Utils::linearToDb(outputGain.getCurrentValue());
        }
        
        for (int i = 0; i < NUM_BANDS; ++i) {
            juce::String prefix = "band" + juce::String(i + 1) + "_";
            const auto& p = bands[i].getParameters();
            
            if (paramId == prefix + "enabled") return p.enabled ? 1.0f : 0.0f;
            if (paramId == prefix + "type") return static_cast<float>(p.type);
            if (paramId == prefix + "freq") return static_cast<float>(p.frequency);
            if (paramId == prefix + "gain") return static_cast<float>(p.gain);
            if (paramId == prefix + "q") return static_cast<float>(p.q);
        }
        
        return 0.0f;
    }
    
    void setParameter(const juce::String& paramId, float value) override {
        if (paramId == "outputGain") {
            setOutputGain(value);
            return;
        }
        
        for (int i = 0; i < NUM_BANDS; ++i) {
            juce::String prefix = "band" + juce::String(i + 1) + "_";
            
            if (paramId == prefix + "enabled") { setBandEnabled(i, value > 0.5f); return; }
            if (paramId == prefix + "type") { setBandType(i, static_cast<EQBandType>(static_cast<int>(value))); return; }
            if (paramId == prefix + "freq") { setBandFrequency(i, value); return; }
            if (paramId == prefix + "gain") { setBandGain(i, value); return; }
            if (paramId == prefix + "q") { setBandQ(i, value); return; }
        }
    }

private:
    std::array<EQBandProcessor, NUM_BANDS> bands;
    DSP::SmoothedValue<float> outputGain;
};

// Register the EQ module
REGISTER_FX_MODULE(EQModule, ModuleType::EQ, "Parametric EQ")

} // namespace FX
} // namespace Sphere

