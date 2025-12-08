#pragma once
#include "SphereFXTypes.h"
#include <cmath>
#include <array>

namespace Sphere {
namespace FX {
namespace DSP {

// ============================================================================
// Smoothed Value - For parameter smoothing
// ============================================================================
template<typename T>
class SmoothedValue {
public:
    SmoothedValue() = default;
    
    void reset(double sampleRate, double rampTimeSeconds = 0.02) {
        if (sampleRate > 0.0 && rampTimeSeconds > 0.0) {
            stepsToTarget = static_cast<int>(sampleRate * rampTimeSeconds);
        } else {
            stepsToTarget = 1;
        }
        currentStep = 0;
    }
    
    void setTargetValue(T newTarget) {
        if (std::abs(newTarget - targetValue) > static_cast<T>(1e-10)) {
            targetValue = newTarget;
            if (stepsToTarget > 0) {
                increment = (targetValue - currentValue) / static_cast<T>(stepsToTarget);
                currentStep = stepsToTarget;
            } else {
                currentValue = targetValue;
                currentStep = 0;
            }
        }
    }
    
    void setCurrentAndTargetValue(T value) {
        currentValue = targetValue = value;
        currentStep = 0;
    }
    
    T getNextValue() {
        if (currentStep > 0) {
            currentValue += increment;
            --currentStep;
            if (currentStep == 0) {
                currentValue = targetValue;
            }
        }
        return currentValue;
    }
    
    T getCurrentValue() const { return currentValue; }
    T getTargetValue() const { return targetValue; }
    bool isSmoothing() const { return currentStep > 0; }
    
    void skip(int numSamples) {
        if (numSamples >= currentStep) {
            currentValue = targetValue;
            currentStep = 0;
        } else {
            currentValue += increment * static_cast<T>(numSamples);
            currentStep -= numSamples;
        }
    }

private:
    T currentValue = T(0);
    T targetValue = T(0);
    T increment = T(0);
    int currentStep = 0;
    int stepsToTarget = 1;
};

// ============================================================================
// Biquad Filter
// ============================================================================
struct BiquadCoeffs {
    double b0 = 1.0, b1 = 0.0, b2 = 0.0;
    double a0 = 1.0, a1 = 0.0, a2 = 0.0;
    
    void normalize() {
        if (std::abs(a0) > 1e-15) {
            b0 /= a0; b1 /= a0; b2 /= a0;
            a1 /= a0; a2 /= a0; a0 = 1.0;
        }
    }
};

class Biquad {
public:
    void setCoefficients(const BiquadCoeffs& c) {
        coeffs = c;
        coeffs.normalize();
    }
    
    void reset() { z1 = z2 = 0.0; }
    
    inline double processSample(double input) {
        double output = coeffs.b0 * input + z1;
        z1 = coeffs.b1 * input - coeffs.a1 * output + z2;
        z2 = coeffs.b2 * input - coeffs.a2 * output;
        z1 = Utils::flushDenormal(z1);
        z2 = Utils::flushDenormal(z2);
        return output;
    }
    
    void processBlock(float* samples, int numSamples) {
        for (int i = 0; i < numSamples; ++i) {
            samples[i] = static_cast<float>(processSample(static_cast<double>(samples[i])));
        }
    }
    
    void processBlock(double* samples, int numSamples) {
        for (int i = 0; i < numSamples; ++i) {
            samples[i] = processSample(samples[i]);
        }
    }

private:
    BiquadCoeffs coeffs;
    double z1 = 0.0, z2 = 0.0;
};

// ============================================================================
// RBJ Filter Coefficient Calculators
// ============================================================================
namespace RBJ {
    inline BiquadCoeffs makeLowPass(double sampleRate, double freq, double q) {
        freq = juce::jlimit(Constants::MIN_FREQUENCY, sampleRate * 0.499, freq);
        const double w0 = 2.0 * juce::MathConstants<double>::pi * freq / sampleRate;
        const double alpha = std::sin(w0) / (2.0 * q);
        const double cosW0 = std::cos(w0);
        
        BiquadCoeffs c;
        c.b0 = (1.0 - cosW0) / 2.0;
        c.b1 = 1.0 - cosW0;
        c.b2 = (1.0 - cosW0) / 2.0;
        c.a0 = 1.0 + alpha;
        c.a1 = -2.0 * cosW0;
        c.a2 = 1.0 - alpha;
        c.normalize();
        return c;
    }
    
    inline BiquadCoeffs makeHighPass(double sampleRate, double freq, double q) {
        freq = juce::jlimit(Constants::MIN_FREQUENCY, sampleRate * 0.499, freq);
        const double w0 = 2.0 * juce::MathConstants<double>::pi * freq / sampleRate;
        const double alpha = std::sin(w0) / (2.0 * q);
        const double cosW0 = std::cos(w0);
        
        BiquadCoeffs c;
        c.b0 = (1.0 + cosW0) / 2.0;
        c.b1 = -(1.0 + cosW0);
        c.b2 = (1.0 + cosW0) / 2.0;
        c.a0 = 1.0 + alpha;
        c.a1 = -2.0 * cosW0;
        c.a2 = 1.0 - alpha;
        c.normalize();
        return c;
    }
    
    inline BiquadCoeffs makeBandPass(double sampleRate, double freq, double q) {
        freq = juce::jlimit(Constants::MIN_FREQUENCY, sampleRate * 0.499, freq);
        const double w0 = 2.0 * juce::MathConstants<double>::pi * freq / sampleRate;
        const double alpha = std::sin(w0) / (2.0 * q);
        const double cosW0 = std::cos(w0);
        
        BiquadCoeffs c;
        c.b0 = alpha;
        c.b1 = 0.0;
        c.b2 = -alpha;
        c.a0 = 1.0 + alpha;
        c.a1 = -2.0 * cosW0;
        c.a2 = 1.0 - alpha;
        c.normalize();
        return c;
    }
    
    inline BiquadCoeffs makePeakEQ(double sampleRate, double freq, double q, double gainDb) {
        freq = juce::jlimit(Constants::MIN_FREQUENCY, sampleRate * 0.499, freq);
        const double A = std::pow(10.0, gainDb / 40.0);
        const double w0 = 2.0 * juce::MathConstants<double>::pi * freq / sampleRate;
        const double alpha = std::sin(w0) / (2.0 * q);
        const double cosW0 = std::cos(w0);
        
        BiquadCoeffs c;
        c.b0 = 1.0 + alpha * A;
        c.b1 = -2.0 * cosW0;
        c.b2 = 1.0 - alpha * A;
        c.a0 = 1.0 + alpha / A;
        c.a1 = -2.0 * cosW0;
        c.a2 = 1.0 - alpha / A;
        c.normalize();
        return c;
    }
    
    inline BiquadCoeffs makeLowShelf(double sampleRate, double freq, double q, double gainDb) {
        freq = juce::jlimit(Constants::MIN_FREQUENCY, sampleRate * 0.499, freq);
        const double A = std::pow(10.0, gainDb / 40.0);
        const double w0 = 2.0 * juce::MathConstants<double>::pi * freq / sampleRate;
        const double alpha = std::sin(w0) / (2.0 * q);
        const double cosW0 = std::cos(w0);
        const double sqrtA = std::sqrt(A);
        const double sqrtA2Alpha = 2.0 * sqrtA * alpha;
        
        BiquadCoeffs c;
        c.b0 = A * ((A + 1.0) - (A - 1.0) * cosW0 + sqrtA2Alpha);
        c.b1 = 2.0 * A * ((A - 1.0) - (A + 1.0) * cosW0);
        c.b2 = A * ((A + 1.0) - (A - 1.0) * cosW0 - sqrtA2Alpha);
        c.a0 = (A + 1.0) + (A - 1.0) * cosW0 + sqrtA2Alpha;
        c.a1 = -2.0 * ((A - 1.0) + (A + 1.0) * cosW0);
        c.a2 = (A + 1.0) + (A - 1.0) * cosW0 - sqrtA2Alpha;
        c.normalize();
        return c;
    }
    
    inline BiquadCoeffs makeHighShelf(double sampleRate, double freq, double q, double gainDb) {
        freq = juce::jlimit(Constants::MIN_FREQUENCY, sampleRate * 0.499, freq);
        const double A = std::pow(10.0, gainDb / 40.0);
        const double w0 = 2.0 * juce::MathConstants<double>::pi * freq / sampleRate;
        const double alpha = std::sin(w0) / (2.0 * q);
        const double cosW0 = std::cos(w0);
        const double sqrtA = std::sqrt(A);
        const double sqrtA2Alpha = 2.0 * sqrtA * alpha;
        
        BiquadCoeffs c;
        c.b0 = A * ((A + 1.0) + (A - 1.0) * cosW0 + sqrtA2Alpha);
        c.b1 = -2.0 * A * ((A - 1.0) + (A + 1.0) * cosW0);
        c.b2 = A * ((A + 1.0) + (A - 1.0) * cosW0 - sqrtA2Alpha);
        c.a0 = (A + 1.0) - (A - 1.0) * cosW0 + sqrtA2Alpha;
        c.a1 = 2.0 * ((A - 1.0) - (A + 1.0) * cosW0);
        c.a2 = (A + 1.0) - (A - 1.0) * cosW0 - sqrtA2Alpha;
        c.normalize();
        return c;
    }
    
    inline BiquadCoeffs makeAllPass(double sampleRate, double freq, double q) {
        freq = juce::jlimit(Constants::MIN_FREQUENCY, sampleRate * 0.499, freq);
        const double w0 = 2.0 * juce::MathConstants<double>::pi * freq / sampleRate;
        const double alpha = std::sin(w0) / (2.0 * q);
        const double cosW0 = std::cos(w0);
        
        BiquadCoeffs c;
        c.b0 = 1.0 - alpha;
        c.b1 = -2.0 * cosW0;
        c.b2 = 1.0 + alpha;
        c.a0 = 1.0 + alpha;
        c.a1 = -2.0 * cosW0;
        c.a2 = 1.0 - alpha;
        c.normalize();
        return c;
    }
}

// ============================================================================
// Envelope Follower
// ============================================================================
class EnvelopeFollower {
public:
    void prepare(double sampleRate) {
        this->sampleRate = sampleRate;
        updateCoefficients();
    }
    
    void setAttackMs(double ms) {
        attackMs = juce::jlimit(0.1, 1000.0, ms);
        updateCoefficients();
    }
    
    void setReleaseMs(double ms) {
        releaseMs = juce::jlimit(1.0, 5000.0, ms);
        updateCoefficients();
    }
    
    void reset() { envelope = 0.0; }
    
    double processSample(double input) {
        double absInput = std::abs(input);
        if (absInput > envelope) {
            envelope += attackCoeff * (absInput - envelope);
        } else {
            envelope += releaseCoeff * (absInput - envelope);
        }
        return envelope;
    }
    
    double getEnvelope() const { return envelope; }

private:
    void updateCoefficients() {
        if (sampleRate > 0.0) {
            attackCoeff = 1.0 - std::exp(-1.0 / (sampleRate * attackMs * 0.001));
            releaseCoeff = 1.0 - std::exp(-1.0 / (sampleRate * releaseMs * 0.001));
        }
    }
    
    double sampleRate = 44100.0;
    double attackMs = 10.0;
    double releaseMs = 100.0;
    double attackCoeff = 0.1;
    double releaseCoeff = 0.01;
    double envelope = 0.0;
};

// ============================================================================
// Simple Delay Line
// ============================================================================
class DelayLine {
public:
    void prepare(int maxDelaySamples) {
        buffer.resize(maxDelaySamples + 1);
        maxDelay = maxDelaySamples;
        reset();
    }
    
    void reset() {
        std::fill(buffer.begin(), buffer.end(), 0.0f);
        writeIndex = 0;
    }
    
    void setDelay(int delaySamples) {
        currentDelay = juce::jlimit(0, maxDelay, delaySamples);
    }
    
    void setDelayFractional(float delaySamples) {
        int intDelay = static_cast<int>(delaySamples);
        fracDelay = delaySamples - intDelay;
        currentDelay = juce::jlimit(0, maxDelay - 1, intDelay);
    }
    
    float processSample(float input) {
        buffer[writeIndex] = input;
        
        int readIndex = writeIndex - currentDelay;
        if (readIndex < 0) readIndex += static_cast<int>(buffer.size());
        
        float output;
        if (fracDelay > 0.0f) {
            // Linear interpolation
            int nextIndex = readIndex - 1;
            if (nextIndex < 0) nextIndex += static_cast<int>(buffer.size());
            output = buffer[readIndex] * (1.0f - fracDelay) + buffer[nextIndex] * fracDelay;
        } else {
            output = buffer[readIndex];
        }
        
        writeIndex = (writeIndex + 1) % buffer.size();
        return output;
    }
    
    float read(int delaySamples) const {
        int readIndex = writeIndex - delaySamples - 1;
        if (readIndex < 0) readIndex += static_cast<int>(buffer.size());
        return buffer[readIndex];
    }

private:
    std::vector<float> buffer;
    int writeIndex = 0;
    int currentDelay = 0;
    int maxDelay = 0;
    float fracDelay = 0.0f;
};

// ============================================================================
// One-pole filter (for smoothing)
// ============================================================================
class OnePole {
public:
    void setCoefficient(double coeff) {
        a = juce::jlimit(0.0, 1.0, coeff);
    }
    
    void setCutoff(double sampleRate, double freqHz) {
        double x = std::exp(-2.0 * juce::MathConstants<double>::pi * freqHz / sampleRate);
        a = 1.0 - x;
    }
    
    void reset() { z = 0.0; }
    
    double processSample(double input) {
        z = a * input + (1.0 - a) * z;
        return z;
    }

private:
    double a = 0.1;
    double z = 0.0;
};

} // namespace DSP
} // namespace FX
} // namespace Sphere

