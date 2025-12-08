#pragma once
#include "SphereEQTypes.h"

namespace Sphere {

// ============================================================================
// RBJ Audio EQ Cookbook Biquad Coefficient Calculator
// Reference: https://www.w3.org/2011/audio/audio-eq-cookbook.html
// ============================================================================
class RBJCookbook {
public:
    // ========================================================================
    // Main coefficient calculation function
    // ========================================================================
    static BiquadCoeffs calculate(EQFilterType type, 
                                   double sampleRate, 
                                   double frequency, 
                                   double q, 
                                   double gainDb) {
        // Clamp frequency to valid range
        frequency = juce::jlimit(MIN_FREQUENCY, sampleRate * 0.499, frequency);
        q = juce::jlimit(MIN_Q, MAX_Q, q);
        
        // Pre-compute common values
        const double omega0 = 2.0 * juce::MathConstants<double>::pi * frequency / sampleRate;
        const double sinOmega = std::sin(omega0);
        const double cosOmega = std::cos(omega0);
        const double alpha = sinOmega / (2.0 * q);
        
        // A = sqrt(10^(dBgain/20)) = 10^(dBgain/40)
        const double A = std::pow(10.0, gainDb / 40.0);
        
        // For shelving filters: 2*sqrt(A)*alpha
        const double sqrtA = std::sqrt(A);
        const double twoSqrtAAlpha = 2.0 * sqrtA * alpha;
        
        BiquadCoeffs coeffs;
        
        switch (type) {
            case EQFilterType::Bell:
                coeffs = makePeakingEQ(cosOmega, alpha, A);
                break;
                
            case EQFilterType::LowShelf:
                coeffs = makeLowShelf(cosOmega, sinOmega, A, sqrtA, twoSqrtAAlpha);
                break;
                
            case EQFilterType::HighShelf:
                coeffs = makeHighShelf(cosOmega, sinOmega, A, sqrtA, twoSqrtAAlpha);
                break;
                
            case EQFilterType::LowCut:
                coeffs = makeHighPass(cosOmega, alpha);
                break;
                
            case EQFilterType::HighCut:
                coeffs = makeLowPass(cosOmega, alpha);
                break;
                
            case EQFilterType::Notch:
                coeffs = makeNotch(cosOmega, alpha);
                break;
                
            case EQFilterType::BandPass:
                coeffs = makeBandPass(cosOmega, sinOmega, alpha);
                break;
                
            case EQFilterType::Tilt:
                coeffs = makeTiltShelf(sampleRate, frequency, gainDb);
                break;
                
            case EQFilterType::AllPass:
                coeffs = makeAllPass(cosOmega, alpha);
                break;
                
            default:
                // Return unity (pass-through)
                coeffs.b0 = 1.0; coeffs.b1 = 0.0; coeffs.b2 = 0.0;
                coeffs.a0 = 1.0; coeffs.a1 = 0.0; coeffs.a2 = 0.0;
                break;
        }
        
        coeffs.normalize();
        return coeffs;
    }
    
    // ========================================================================
    // Calculate Q for Butterworth cascade stages
    // For building higher-order filters from cascaded 2nd-order sections
    // ========================================================================
    static double butterworthQ(int order, int stage) {
        // For 2nd order sections in a Butterworth cascade
        // Q = 1 / (2 * cos(pi * (2*k + 1) / (2*n)))
        // where k is the stage (0-indexed), n is the total order
        if (order <= 0 || stage < 0) return 0.7071; // sqrt(2)/2 for 2nd order
        
        const double angle = juce::MathConstants<double>::pi * (2.0 * stage + 1.0) / (2.0 * order);
        const double denominator = 2.0 * std::cos(angle);
        
        if (std::abs(denominator) < 1e-10) return 10.0; // Limit Q
        return 1.0 / denominator;
    }
    
    // ========================================================================
    // Calculate Q for Linkwitz-Riley cascade (squared Butterworth)
    // ========================================================================
    static double linkwitzRileyQ(int order, int stage) {
        // Linkwitz-Riley uses lower Q values for flatter summed response
        return butterworthQ(order, stage) * 0.7071;
    }

private:
    // ========================================================================
    // Peaking EQ (Bell)
    // H(s) = (s^2 + s*(A/Q) + 1) / (s^2 + s/(A*Q) + 1)
    // ========================================================================
    static BiquadCoeffs makePeakingEQ(double cosOmega, double alpha, double A) {
        BiquadCoeffs c;
        const double alphaOverA = alpha / A;
        const double alphaTimesA = alpha * A;
        
        c.b0 = 1.0 + alphaTimesA;
        c.b1 = -2.0 * cosOmega;
        c.b2 = 1.0 - alphaTimesA;
        c.a0 = 1.0 + alphaOverA;
        c.a1 = -2.0 * cosOmega;
        c.a2 = 1.0 - alphaOverA;
        
        return c;
    }
    
    // ========================================================================
    // Low Shelf
    // ========================================================================
    static BiquadCoeffs makeLowShelf(double cosOmega, double sinOmega, 
                                      double A, double sqrtA, double twoSqrtAAlpha) {
        BiquadCoeffs c;
        const double AmP1 = A + 1.0;
        const double AmM1 = A - 1.0;
        const double AmP1CosOmega = AmP1 * cosOmega;
        const double AmM1CosOmega = AmM1 * cosOmega;
        
        c.b0 = A * (AmP1 - AmM1CosOmega + twoSqrtAAlpha);
        c.b1 = 2.0 * A * (AmM1 - AmP1CosOmega);
        c.b2 = A * (AmP1 - AmM1CosOmega - twoSqrtAAlpha);
        c.a0 = AmP1 + AmM1CosOmega + twoSqrtAAlpha;
        c.a1 = -2.0 * (AmM1 + AmP1CosOmega);
        c.a2 = AmP1 + AmM1CosOmega - twoSqrtAAlpha;
        
        return c;
    }
    
    // ========================================================================
    // High Shelf
    // ========================================================================
    static BiquadCoeffs makeHighShelf(double cosOmega, double sinOmega,
                                       double A, double sqrtA, double twoSqrtAAlpha) {
        BiquadCoeffs c;
        const double AmP1 = A + 1.0;
        const double AmM1 = A - 1.0;
        const double AmP1CosOmega = AmP1 * cosOmega;
        const double AmM1CosOmega = AmM1 * cosOmega;
        
        c.b0 = A * (AmP1 + AmM1CosOmega + twoSqrtAAlpha);
        c.b1 = -2.0 * A * (AmM1 + AmP1CosOmega);
        c.b2 = A * (AmP1 + AmM1CosOmega - twoSqrtAAlpha);
        c.a0 = AmP1 - AmM1CosOmega + twoSqrtAAlpha;
        c.a1 = 2.0 * (AmM1 - AmP1CosOmega);
        c.a2 = AmP1 - AmM1CosOmega - twoSqrtAAlpha;
        
        return c;
    }
    
    // ========================================================================
    // High Pass (for Low Cut)
    // H(s) = s^2 / (s^2 + s/Q + 1)
    // ========================================================================
    static BiquadCoeffs makeHighPass(double cosOmega, double alpha) {
        BiquadCoeffs c;
        const double onePlusCos = 1.0 + cosOmega;
        
        c.b0 = onePlusCos / 2.0;
        c.b1 = -(onePlusCos);
        c.b2 = onePlusCos / 2.0;
        c.a0 = 1.0 + alpha;
        c.a1 = -2.0 * cosOmega;
        c.a2 = 1.0 - alpha;
        
        return c;
    }
    
    // ========================================================================
    // Low Pass (for High Cut)
    // H(s) = 1 / (s^2 + s/Q + 1)
    // ========================================================================
    static BiquadCoeffs makeLowPass(double cosOmega, double alpha) {
        BiquadCoeffs c;
        const double oneMinusCos = 1.0 - cosOmega;
        
        c.b0 = oneMinusCos / 2.0;
        c.b1 = oneMinusCos;
        c.b2 = oneMinusCos / 2.0;
        c.a0 = 1.0 + alpha;
        c.a1 = -2.0 * cosOmega;
        c.a2 = 1.0 - alpha;
        
        return c;
    }
    
    // ========================================================================
    // Notch (Band Reject)
    // H(s) = (s^2 + 1) / (s^2 + s/Q + 1)
    // ========================================================================
    static BiquadCoeffs makeNotch(double cosOmega, double alpha) {
        BiquadCoeffs c;
        
        c.b0 = 1.0;
        c.b1 = -2.0 * cosOmega;
        c.b2 = 1.0;
        c.a0 = 1.0 + alpha;
        c.a1 = -2.0 * cosOmega;
        c.a2 = 1.0 - alpha;
        
        return c;
    }
    
    // ========================================================================
    // Band Pass (constant skirt gain, peak gain = Q)
    // H(s) = (s/Q) / (s^2 + s/Q + 1)
    // ========================================================================
    static BiquadCoeffs makeBandPass(double cosOmega, double sinOmega, double alpha) {
        BiquadCoeffs c;
        
        c.b0 = sinOmega / 2.0;  // = Q*alpha
        c.b1 = 0.0;
        c.b2 = -sinOmega / 2.0;
        c.a0 = 1.0 + alpha;
        c.a1 = -2.0 * cosOmega;
        c.a2 = 1.0 - alpha;
        
        return c;
    }
    
    // ========================================================================
    // All Pass
    // H(s) = (s^2 - s/Q + 1) / (s^2 + s/Q + 1)
    // ========================================================================
    static BiquadCoeffs makeAllPass(double cosOmega, double alpha) {
        BiquadCoeffs c;
        
        c.b0 = 1.0 - alpha;
        c.b1 = -2.0 * cosOmega;
        c.b2 = 1.0 + alpha;
        c.a0 = 1.0 + alpha;
        c.a1 = -2.0 * cosOmega;
        c.a2 = 1.0 - alpha;
        
        return c;
    }
    
    // ========================================================================
    // Tilt Shelf (combines low and high shelf for a tilt effect)
    // ========================================================================
    static BiquadCoeffs makeTiltShelf(double sampleRate, double frequency, double gainDb) {
        // Tilt is implemented as a combination that boosts one end while cutting the other
        // Use a moderate Q for smooth tilt
        const double omega0 = 2.0 * juce::MathConstants<double>::pi * frequency / sampleRate;
        const double sinOmega = std::sin(omega0);
        const double cosOmega = std::cos(omega0);
        const double q = 0.65; // Moderate Q for smooth tilt
        const double alpha = sinOmega / (2.0 * q);
        
        // Half the gain for each direction
        const double A = std::pow(10.0, gainDb / 80.0); // Quarter of normal gain effect
        const double sqrtA = std::sqrt(A);
        const double twoSqrtAAlpha = 2.0 * sqrtA * alpha;
        
        // Use a modified shelf formula that creates a tilt
        BiquadCoeffs c;
        const double AmP1 = A + 1.0;
        const double AmM1 = A - 1.0;
        
        // This creates an asymmetric response
        c.b0 = A * (AmP1 - AmM1 * cosOmega + twoSqrtAAlpha);
        c.b1 = 2.0 * A * (AmM1 - AmP1 * cosOmega);
        c.b2 = A * (AmP1 - AmM1 * cosOmega - twoSqrtAAlpha);
        c.a0 = AmP1 + AmM1 * cosOmega + twoSqrtAAlpha;
        c.a1 = -2.0 * (AmM1 + AmP1 * cosOmega);
        c.a2 = AmP1 + AmM1 * cosOmega - twoSqrtAAlpha;
        
        return c;
    }
};

// ============================================================================
// First Order Filter Coefficients (for 6 dB/oct slopes)
// ============================================================================
class FirstOrderFilter {
public:
    static BiquadCoeffs makeLowPass(double sampleRate, double frequency) {
        frequency = juce::jlimit(MIN_FREQUENCY, sampleRate * 0.499, frequency);
        
        const double omega = 2.0 * juce::MathConstants<double>::pi * frequency / sampleRate;
        const double cosOmega = std::cos(omega);
        const double alpha = (1.0 - cosOmega) / (1.0 + cosOmega);
        
        BiquadCoeffs c;
        c.b0 = (1.0 - alpha) / 2.0;
        c.b1 = (1.0 - alpha) / 2.0;
        c.b2 = 0.0;
        c.a0 = 1.0;
        c.a1 = -alpha;
        c.a2 = 0.0;
        
        return c;
    }
    
    static BiquadCoeffs makeHighPass(double sampleRate, double frequency) {
        frequency = juce::jlimit(MIN_FREQUENCY, sampleRate * 0.499, frequency);
        
        const double omega = 2.0 * juce::MathConstants<double>::pi * frequency / sampleRate;
        const double cosOmega = std::cos(omega);
        const double alpha = (1.0 - cosOmega) / (1.0 + cosOmega);
        
        BiquadCoeffs c;
        c.b0 = (1.0 + alpha) / 2.0;
        c.b1 = -(1.0 + alpha) / 2.0;
        c.b2 = 0.0;
        c.a0 = 1.0;
        c.a1 = -alpha;
        c.a2 = 0.0;
        
        return c;
    }
};

} // namespace Sphere

