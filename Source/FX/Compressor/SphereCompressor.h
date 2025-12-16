#pragma once
#include <JuceHeader.h>
#include <algorithm>
#include <cmath>

namespace Sphere {

class SphereCompressor {
public:
    void prepare(double newSampleRate, int blockSize) {
        this->sampleRate = newSampleRate;
        updateCoefficients();
    }
    
    void reset() {
        envelopeL = envelopeR = 0.0f;
        currentGainReduction = 0.0f;
    }
    
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) {
        if (!enabled) return;
        
        const int numSamples = buffer.getNumSamples();
        const int numChannels = buffer.getNumChannels();
        if (numChannels == 0 || numSamples == 0) return;
        
        float* left = buffer.getWritePointer(0);
        float* right = numChannels > 1 ? buffer.getWritePointer(1) : nullptr;
        
        // Temporary buffers for delta monitoring if needed
        juce::AudioBuffer<float> dryBuffer;
        if (deltaMonitoring) {
            dryBuffer.makeCopyOf(buffer);
        }
        
        float maxGR = 0.0f;
        
        for (int i = 0; i < numSamples; ++i) {
            float inputL = std::abs(left[i]);
            float inputR = right ? std::abs(right[i]) : inputL;
            
            // Envelope following
            if (inputL > envelopeL) envelopeL += attackCoeff * (inputL - envelopeL);
            else envelopeL += releaseCoeff * (inputL - envelopeL);
            
            if (inputR > envelopeR) envelopeR += attackCoeff * (inputR - envelopeR);
            else envelopeR += releaseCoeff * (inputR - envelopeR);
            
            float env = std::max(envelopeL, envelopeR);
            
            // Convert to dB
            float envDb = (env > 1e-10f) ? 20.0f * std::log10(env) : -100.0f;
            
            // Calculate gain reduction
            float gainReductionDb = calculateGainReduction(envDb);
            maxGR = std::max(maxGR, gainReductionDb);
            
            // Apply gain reduction + makeup
            float gain = std::pow(10.0f, (-gainReductionDb + makeupGainDb) / 20.0f);
            
            left[i] *= gain;
            if (right) right[i] *= gain;
        }
        
        currentGainReduction = maxGR;
        
        // Apply Delta Monitoring (Output = Dry - Compressed)
        // Actually, delta is usually (Input - Output) to hear what was removed.
        // Let's implement it as: Output = Dry - Wet(without makeup)
        // But for simplicity/standard behavior: Input - Output(before makeup)
        // However, standard "Delta" often means just the GR signal.
        
        if (deltaMonitoring) {
             const float* dryL = dryBuffer.getReadPointer(0);
             const float* dryR = numChannels > 1 ? dryBuffer.getReadPointer(1) : nullptr;
             
             for (int i = 0; i < numSamples; ++i) {
                 // Delta = Dry - Wet. 
                 // If we had Makeup gain, Wet might be louder than Dry, so Delta would be negative?
                 // Usually Delta is (Input - CompressedInput).
                 
                 // Let's do: Output = Input - Output.
                 left[i] = dryL[i] - left[i];
                 if (right) right[i] = (dryR ? dryR[i] : dryL[i]) - right[i];
             }
        }
    }
    
    // Setters
    void setEnabled(bool e) { enabled = e; }
    void setDeltaMonitoring(bool d) { deltaMonitoring = d; }
    
    void setThreshold(float db) { thresholdDb = juce::jlimit(-60.0f, 0.0f, db); }
    void setRatio(float r) { ratio = juce::jlimit(1.0f, 100.0f, r); }
    void setAttack(float ms) { attackMs = juce::jlimit(0.1f, 500.0f, ms); updateCoefficients(); }
    void setRelease(float ms) { releaseMs = juce::jlimit(10.0f, 3000.0f, ms); updateCoefficients(); }
    void setMakeupGain(float db) { makeupGainDb = juce::jlimit(-12.0f, 24.0f, db); }
    void setKneeWidth(float db) { kneeWidthDb = juce::jlimit(0.0f, 24.0f, db); }
    
    // Getters
    float getGainReduction() const { return currentGainReduction; }

private:
    float calculateGainReduction(float inputDb) const {
        if (inputDb < thresholdDb - kneeWidthDb / 2.0f) return 0.0f;
        
        float overThreshold;
        if (kneeWidthDb > 0.0f && inputDb < thresholdDb + kneeWidthDb / 2.0f) {
            float x = inputDb - thresholdDb + kneeWidthDb / 2.0f;
            overThreshold = (x * x) / (2.0f * kneeWidthDb);
        } else {
            overThreshold = inputDb - thresholdDb;
        }
        
        return std::max(0.0f, overThreshold * (1.0f - 1.0f / ratio));
    }
    
    void updateCoefficients() {
        if (sampleRate > 0.0) {
            attackCoeff = 1.0f - std::exp(-1.0f / (float(sampleRate) * attackMs * 0.001f));
            releaseCoeff = 1.0f - std::exp(-1.0f / (float(sampleRate) * releaseMs * 0.001f));
        }
    }
    
    bool enabled = true;
    bool deltaMonitoring = false;
    double sampleRate = 44100.0;
    
    float thresholdDb = -20.0f;
    float ratio = 4.0f;
    float attackMs = 10.0f;
    float releaseMs = 100.0f;
    float makeupGainDb = 0.0f;
    float kneeWidthDb = 6.0f;
    
    float attackCoeff = 0.1f;
    float releaseCoeff = 0.01f;
    float envelopeL = 0.0f;
    float envelopeR = 0.0f;
    float currentGainReduction = 0.0f;
};

} // namespace Sphere
