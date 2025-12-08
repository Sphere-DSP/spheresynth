#pragma once
#include "SphereEQTypes.h"
#include "SphereEQCookbook.h"

namespace Sphere {

// ============================================================================
// Biquad Filter Processor
// Direct Form II Transposed implementation for better numerical behavior
// ============================================================================
class Biquad {
public:
    Biquad() { reset(); }
    
    // ========================================================================
    // Set coefficients
    // ========================================================================
    void setCoefficients(const BiquadCoeffs& newCoeffs) {
        coeffs = newCoeffs;
        coeffs.normalize();
    }
    
    void setCoefficients(double b0, double b1, double b2, 
                         double a0, double a1, double a2) {
        coeffs.b0 = b0;
        coeffs.b1 = b1;
        coeffs.b2 = b2;
        coeffs.a0 = a0;
        coeffs.a1 = a1;
        coeffs.a2 = a2;
        coeffs.normalize();
    }
    
    // ========================================================================
    // Reset filter state
    // ========================================================================
    void reset() {
        z1 = 0.0;
        z2 = 0.0;
    }
    
    // ========================================================================
    // Process single sample (Direct Form II Transposed)
    // ========================================================================
    inline double processSample(double input) {
        // JUCE denormal handling
        juce::ScopedNoDenormals noDenormals;
        
        // Direct Form II Transposed:
        // y[n] = b0*x[n] + z1[n-1]
        // z1[n] = b1*x[n] - a1*y[n] + z2[n-1]
        // z2[n] = b2*x[n] - a2*y[n]
        
        const double output = coeffs.b0 * input + z1;
        z1 = coeffs.b1 * input - coeffs.a1 * output + z2;
        z2 = coeffs.b2 * input - coeffs.a2 * output;
        
        // Flush denormals
        z1 = flushDenormal(z1);
        z2 = flushDenormal(z2);
        
        return output;
    }
    
    // ========================================================================
    // Process audio block
    // ========================================================================
    void processBlock(double* samples, int numSamples) {
        juce::ScopedNoDenormals noDenormals;
        
        for (int i = 0; i < numSamples; ++i) {
            samples[i] = processSample(samples[i]);
        }
    }
    
    void processBlock(float* samples, int numSamples) {
        juce::ScopedNoDenormals noDenormals;
        
        for (int i = 0; i < numSamples; ++i) {
            samples[i] = static_cast<float>(processSample(static_cast<double>(samples[i])));
        }
    }
    
    // ========================================================================
    // Get current coefficients
    // ========================================================================
    const BiquadCoeffs& getCoefficients() const { return coeffs; }
    
    // ========================================================================
    // Check if filter is stable
    // ========================================================================
    bool isStable() const { return coeffs.isStable(); }
    
    // ========================================================================
    // Get magnitude response at frequency
    // ========================================================================
    double getMagnitudeResponse(double frequency, double sampleRate) const {
        const double omega = 2.0 * juce::MathConstants<double>::pi * frequency / sampleRate;
        const double cosW = std::cos(omega);
        const double cos2W = std::cos(2.0 * omega);
        const double sinW = std::sin(omega);
        const double sin2W = std::sin(2.0 * omega);
        
        // H(e^jw) = (b0 + b1*e^-jw + b2*e^-2jw) / (1 + a1*e^-jw + a2*e^-2jw)
        // Using: e^-jw = cos(w) - j*sin(w)
        
        const double numReal = coeffs.b0 + coeffs.b1 * cosW + coeffs.b2 * cos2W;
        const double numImag = -coeffs.b1 * sinW - coeffs.b2 * sin2W;
        const double denReal = 1.0 + coeffs.a1 * cosW + coeffs.a2 * cos2W;
        const double denImag = -coeffs.a1 * sinW - coeffs.a2 * sin2W;
        
        const double numMag = std::sqrt(numReal * numReal + numImag * numImag);
        const double denMag = std::sqrt(denReal * denReal + denImag * denImag);
        
        if (denMag < 1e-10) return 0.0;
        return numMag / denMag;
    }
    
    // ========================================================================
    // Get phase response at frequency
    // ========================================================================
    double getPhaseResponse(double frequency, double sampleRate) const {
        const double omega = 2.0 * juce::MathConstants<double>::pi * frequency / sampleRate;
        const double cosW = std::cos(omega);
        const double cos2W = std::cos(2.0 * omega);
        const double sinW = std::sin(omega);
        const double sin2W = std::sin(2.0 * omega);
        
        const double numReal = coeffs.b0 + coeffs.b1 * cosW + coeffs.b2 * cos2W;
        const double numImag = -coeffs.b1 * sinW - coeffs.b2 * sin2W;
        const double denReal = 1.0 + coeffs.a1 * cosW + coeffs.a2 * cos2W;
        const double denImag = -coeffs.a1 * sinW - coeffs.a2 * sin2W;
        
        return std::atan2(numImag, numReal) - std::atan2(denImag, denReal);
    }

private:
    BiquadCoeffs coeffs;
    double z1 = 0.0;  // State variable 1
    double z2 = 0.0;  // State variable 2
};

// ============================================================================
// Cascaded Biquad Filter (for higher-order filters)
// ============================================================================
class CascadedBiquad {
public:
    CascadedBiquad() = default;
    
    // ========================================================================
    // Set number of stages
    // ========================================================================
    void setNumStages(int stages) {
        numStages = juce::jlimit(0, MAX_BIQUADS_PER_BAND, stages);
        reset();
    }
    
    int getNumStages() const { return numStages; }
    
    // ========================================================================
    // Set coefficients for a specific stage
    // ========================================================================
    void setStageCoefficients(int stageIndex, const BiquadCoeffs& coeffs) {
        if (stageIndex >= 0 && stageIndex < numStages) {
            stages[stageIndex].setCoefficients(coeffs);
        }
    }
    
    // ========================================================================
    // Configure as Butterworth high/low pass with given order
    // ========================================================================
    void configureButterworth(EQFilterType type, double sampleRate, 
                               double frequency, int order) {
        // Order determines number of biquad stages
        // Each biquad is 2nd order, so order/2 biquads for even orders
        const int numBiquads = (order + 1) / 2;
        setNumStages(numBiquads);
        
        for (int i = 0; i < numBiquads; ++i) {
            // Calculate Q for this stage in the Butterworth cascade
            double q;
            if (order == 1) {
                // First order uses first-order filter formula
                q = 0.5; // Not really used for first order
            } else {
                q = RBJCookbook::butterworthQ(order, i);
            }
            
            BiquadCoeffs coeffs;
            if (order == 1 && i == 0) {
                // Use first-order filter
                if (type == EQFilterType::LowCut) {
                    coeffs = FirstOrderFilter::makeHighPass(sampleRate, frequency);
                } else if (type == EQFilterType::HighCut) {
                    coeffs = FirstOrderFilter::makeLowPass(sampleRate, frequency);
                }
            } else {
                coeffs = RBJCookbook::calculate(type, sampleRate, frequency, q, 0.0);
            }
            
            stages[i].setCoefficients(coeffs);
        }
    }
    
    // ========================================================================
    // Reset all stages
    // ========================================================================
    void reset() {
        for (int i = 0; i < MAX_BIQUADS_PER_BAND; ++i) {
            stages[i].reset();
        }
    }
    
    // ========================================================================
    // Process single sample through cascade
    // ========================================================================
    inline double processSample(double input) {
        double output = input;
        for (int i = 0; i < numStages; ++i) {
            output = stages[i].processSample(output);
        }
        return output;
    }
    
    // ========================================================================
    // Process audio block through cascade
    // ========================================================================
    void processBlock(double* samples, int numSamples) {
        for (int stage = 0; stage < numStages; ++stage) {
            stages[stage].processBlock(samples, numSamples);
        }
    }
    
    void processBlock(float* samples, int numSamples) {
        for (int stage = 0; stage < numStages; ++stage) {
            stages[stage].processBlock(samples, numSamples);
        }
    }
    
    // ========================================================================
    // Get total magnitude response at frequency
    // ========================================================================
    double getMagnitudeResponse(double frequency, double sampleRate) const {
        double magnitude = 1.0;
        for (int i = 0; i < numStages; ++i) {
            magnitude *= stages[i].getMagnitudeResponse(frequency, sampleRate);
        }
        return magnitude;
    }
    
    // ========================================================================
    // Get total phase response at frequency
    // ========================================================================
    double getPhaseResponse(double frequency, double sampleRate) const {
        double phase = 0.0;
        for (int i = 0; i < numStages; ++i) {
            phase += stages[i].getPhaseResponse(frequency, sampleRate);
        }
        return phase;
    }
    
    // ========================================================================
    // Access individual stages
    // ========================================================================
    Biquad& getStage(int index) { return stages[index]; }
    const Biquad& getStage(int index) const { return stages[index]; }

private:
    std::array<Biquad, MAX_BIQUADS_PER_BAND> stages;
    int numStages = 0;
};

} // namespace Sphere

