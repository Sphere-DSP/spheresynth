#pragma once
#include <JuceHeader.h>
#include "DemoUtilities.h"
#include <atomic>
#include <algorithm>

#include "EQ/SphereEQTypes.h"
#include "EQ/SphereEQCookbook.h"
#include "EQ/SphereEQBiquad.h"
#include "EQ/SphereEQBandProcessor.h"
#include "EQ/SphereEQEngineV2.h"

// ============================================================================
// Simple Compressor - Standalone implementation
// ============================================================================
namespace Sphere {

class SimpleCompressor {
public:
    void prepare(double sampleRate, int blockSize) {
        this->sampleRate = sampleRate;
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
        
        float maxGR = 0.0f;
        float maxEnvDb = -100.0f;
        
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
            maxEnvDb = std::max(maxEnvDb, envDb);
            
            // Calculate gain reduction
            float gainReductionDb = calculateGainReduction(envDb);
            maxGR = std::max(maxGR, gainReductionDb);
            
            // Apply gain reduction + makeup
            float gain = std::pow(10.0f, (-gainReductionDb + makeupGainDb) / 20.0f);
            
            left[i] *= gain;
            if (right) right[i] *= gain;
        }
        
        currentGainReduction = maxGR;
        
        // Debug output (every ~1 second at 44100 Hz)
        debugCounter += numSamples;
        if (debugCounter > 44100 && maxEnvDb > -80.0f) {
            std::cout << "[COMP] EnvDb: " << maxEnvDb << " Threshold: " << thresholdDb << " GR: " << maxGR << std::endl;
            debugCounter = 0;
        }
    }
    
    // Setters
    void setEnabled(bool e) { enabled = e; }
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
    double sampleRate = 44100.0;
    float thresholdDb = -50.0f;  // Lower default to match synth output levels
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
    int debugCounter = 0;
};

} // namespace Sphere

#include "FX/Compressor/SphereCompressor.h"

using namespace juce;

//==============================================================================
/** Our demo synth sound is just a basic oscillator.. */
struct OscillatorSound final : public juce::SynthesiserSound {
  enum class WaveType { Sine, Saw, Square };

  OscillatorSound(WaveType type) : waveType(type) {}

  bool appliesToNote(int /*midiNoteNumber*/) override { return true; }
  bool appliesToChannel(int /*midiChannel*/) override { return true; }

  WaveType waveType;
};

//==============================================================================
/** Our demo synth voice plays sine, saw, or square waves.. */
struct OscillatorVoice final : public juce::SynthesiserVoice {
  bool canPlaySound(juce::SynthesiserSound *sound) override {
    return dynamic_cast<OscillatorSound *>(sound) != nullptr;
  }

  void startNote(int midiNoteNumber, float velocity,
                 juce::SynthesiserSound *sound,
                 int /*currentPitchWheelPosition*/) override {
    if (auto *oscSound = dynamic_cast<OscillatorSound *>(sound)) {
      currentWaveType = oscSound->waveType;
    }

    currentAngle = 0.0;
    level = velocity * 1.5; // Boosted for louder output (was 0.5)
    tailOff = 0.0;

    auto cyclesPerSecond =
        juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    auto cyclesPerSample = cyclesPerSecond / getSampleRate();

    angleDelta = cyclesPerSample * juce::MathConstants<double>::twoPi;
  }

  void stopNote(float /*velocity*/, bool allowTailOff) override {
    if (allowTailOff) {
      if (approximatelyEqual(tailOff, 0.0))
        tailOff = 1.0;
    } else {
      clearCurrentNote();
      angleDelta = 0.0;
    }
  }

  void pitchWheelMoved(int /*newValue*/) override {}
  void controllerMoved(int /*controllerNumber*/, int /*newValue*/) override {}

  void renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample,
                       int numSamples) override {
    if (!approximatelyEqual(angleDelta, 0.0)) {
      if (tailOff > 0.0) {
        while (--numSamples >= 0) {
          auto currentSample = getNextSample() * level * tailOff;

          for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
            outputBuffer.addSample(i, startSample, currentSample);

          currentAngle += angleDelta;
          if (currentAngle >= juce::MathConstants<double>::twoPi)
            currentAngle -= juce::MathConstants<double>::twoPi;

          ++startSample;

          tailOff *= 0.99;

          if (tailOff <= 0.005) {
            clearCurrentNote();
            angleDelta = 0.0;
            break;
          }
        }
      } else {
        while (--numSamples >= 0) {
          auto currentSample = getNextSample() * level;

          for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
            outputBuffer.addSample(i, startSample, currentSample);

          currentAngle += angleDelta;
          if (currentAngle >= juce::MathConstants<double>::twoPi)
            currentAngle -= juce::MathConstants<double>::twoPi;

          ++startSample;
        }
      }
    }
  }

private:
  float getNextSample() {
    switch (currentWaveType) {
    case OscillatorSound::WaveType::Sine:
      return (float)std::sin(currentAngle);
    case OscillatorSound::WaveType::Saw:
      return (float)(1.0 - (currentAngle / juce::MathConstants<double>::pi));
    case OscillatorSound::WaveType::Square:
      return currentAngle < juce::MathConstants<double>::pi ? 1.0f : -1.0f; // Full amplitude
    default:
      return 0.0f;
    }
  }

  double currentAngle = 0.0, angleDelta = 0.0, level = 0.0, tailOff = 0.0;
  OscillatorSound::WaveType currentWaveType = OscillatorSound::WaveType::Sine;
};

//==============================================================================
// This is an audio source that streams the output of our demo synth.
struct SynthAudioSource final : public AudioSource {
  SynthAudioSource(MidiKeyboardState &keyState) : keyboardState(keyState) {
    for (auto i = 0; i < 4; ++i) {
      synth.addVoice(new OscillatorVoice());
      synth.addVoice(new SamplerVoice());
    }
    
    // Pre-cache the sampled sound to avoid disk I/O on sound change
    cacheSampledSound();
    setUsingSineWaveSound();
  }

  void setUsingSineWaveSound() {
    synth.clearSounds();
    synth.addSound(new OscillatorSound(OscillatorSound::WaveType::Sine));
  }

  void setUsingSawWaveSound() {
    synth.clearSounds();
    synth.addSound(new OscillatorSound(OscillatorSound::WaveType::Saw));
  }

  void setUsingSquareWaveSound() {
    synth.clearSounds();
    synth.addSound(new OscillatorSound(OscillatorSound::WaveType::Square));
  }

  void setUsingSampledSound() {
    synth.clearSounds();
    
    // Try loading on demand if cache is empty
    if (cachedWavData.getSize() == 0) {
      cacheSampledSound();
    }
    
    // Use cached WAV data from memory
    if (cachedWavData.getSize() > 0) {
      WavAudioFormat wavFormat;
      auto memStream = std::make_unique<MemoryInputStream>(cachedWavData, false);
      std::unique_ptr<AudioFormatReader> reader(wavFormat.createReaderFor(memStream.release(), true));
      
      if (reader != nullptr) {
        BigInteger allNotes;
        allNotes.setRange(0, 128, true);
        synth.addSound(new SamplerSound("demo sound", *reader, allNotes, 74,
                                        0.1, 0.1, 10.0));
        return;
      }
    }
    
    // Fallback to sine wave if sampled sound failed to load
    synth.addSound(new OscillatorSound(OscillatorSound::WaveType::Sine));
  }

  void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override {
    midiCollector.reset(sampleRate);
    synth.setCurrentPlaybackSampleRate(sampleRate);
    
    // Use actual host block size instead of fixed 4096
    // This optimizes buffer allocation for the real processing scenario
    int actualBlockSize = juce::jmax(samplesPerBlockExpected, 256);
    eqEngine.prepare(sampleRate, actualBlockSize, 2);
    
    setupDefaultEQBands();
    
    // Prepare Compressor
    compressor.prepare(sampleRate, actualBlockSize);
  }

  void releaseResources() override {
      eqEngine.reset();
      compressor.reset();
  }

  void getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) override {
    bufferToFill.clearActiveBufferRegion();
    MidiBuffer incomingMidi;
    midiCollector.removeNextBlockOfMessages(incomingMidi,
                                            bufferToFill.numSamples);
    keyboardState.processNextMidiBuffer(incomingMidi, 0,
                                        bufferToFill.numSamples, true);
    synth.renderNextBlock(*bufferToFill.buffer, incomingMidi, 0,
                          bufferToFill.numSamples);
    
    // Apply gain boost for sampled sounds (they're typically quieter)
    bufferToFill.buffer->applyGain(3.0f);

    // Apply Sphere EQ V2
    eqEngine.processBlock(*bufferToFill.buffer, incomingMidi);
    
    // Apply Compressor
    juce::MidiBuffer emptyMidi;
    compressor.processBlock(*bufferToFill.buffer, emptyMidi);

    // Calculate RMS for visualization (avoid division, use approximation)
    if (bufferToFill.buffer->getNumChannels() > 0) {
      currentRMS.store(
          bufferToFill.buffer->getRMSLevel(0, 0, bufferToFill.numSamples),
          std::memory_order_relaxed);
    }
  }
  
  // ============================================================================
  // EQ Control Interface
  // ============================================================================
  void setEQEnabled(bool enabled) {
      eqEngine.setEnabled(enabled);
  }
  
  void setEQBandParameters(int bandIndex, const Sphere::EQBandParams& params) {
      eqEngine.setBandParameters(bandIndex, params);
  }
  
  void setEQBandType(int bandIndex, Sphere::EQFilterType type) {
      eqEngine.setBandType(bandIndex, type);
  }
  
  void setEQBandFrequency(int bandIndex, double frequency) {
      eqEngine.setBandFrequency(bandIndex, frequency);
  }
  
  void setEQBandQ(int bandIndex, double q) {
      eqEngine.setBandQ(bandIndex, q);
  }
  
  void setEQBandGain(int bandIndex, double gainDb) {
      eqEngine.setBandGain(bandIndex, gainDb);
  }
  
  void setEQBandSlope(int bandIndex, Sphere::EQSlope slope) {
      eqEngine.setBandSlope(bandIndex, slope);
  }
  
  void setEQBandBypass(int bandIndex, bool bypass) {
      eqEngine.setBandBypass(bandIndex, bypass);
  }
  
  void setEQBandStereoMode(int bandIndex, Sphere::EQStereoMode mode) {
      eqEngine.setBandStereoMode(bandIndex, mode);
  }
  
  void setEQBandDynamicMode(int bandIndex, Sphere::EQDynamicMode mode) {
      eqEngine.setBandDynamicMode(bandIndex, mode);
  }
  
  void setEQBandDynamicParams(int bandIndex, double threshold, double ratio,
                               double attack, double release, double range) {
      eqEngine.setBandDynamicParams(bandIndex, threshold, ratio, attack, release, range);
  }
  
  void setEQBandCharacter(int bandIndex, Sphere::EQCharacterMode mode) {
      eqEngine.setBandCharacter(bandIndex, mode);
  }
  
  double getEQMagnitudeResponse(double frequency) const {
      return eqEngine.getMagnitudeResponseDb(frequency);
  }
  
  const Sphere::EQBandParams& getEQBandParameters(int bandIndex) const {
      return eqEngine.getBandParameters(bandIndex);
  }

  // Public members that need external access
  std::atomic<float> currentRMS{0.0f};
  MidiMessageCollector midiCollector;
  MidiKeyboardState &keyboardState;
  Synthesiser synth;
  Sphere::SphereEQEngineV2 eqEngine;
  Sphere::SphereCompressor compressor;
  
  // ============================================================================
  // Compressor Control Interface
  // ============================================================================
  void setCompressorEnabled(bool enabled) {
      compressor.setEnabled(enabled);
  }
  
  void setCompressorDelta(bool enabled) {
      compressor.setDeltaMonitoring(enabled);
  }
  
  void setCompressorThreshold(float thresholdDb) {
      compressor.setThreshold(thresholdDb);
  }
  
  void setCompressorRatio(float ratio) {
      compressor.setRatio(ratio);
  }
  
  void setCompressorAttack(float attackMs) {
      compressor.setAttack(attackMs);
  }
  
  void setCompressorRelease(float releaseMs) {
      compressor.setRelease(releaseMs);
  }
  
  void setCompressorMakeup(float makeupDb) {
      compressor.setMakeupGain(makeupDb);
  }
  
  void setCompressorKnee(float kneeDb) {
      compressor.setKneeWidth(kneeDb);
  }
  
  float getCompressorGainReduction() const {
      return compressor.getGainReduction();
  }

private:
  // Cache WAV file data in memory
  void cacheSampledSound() {
    // Try direct path relative to executable
    auto exeFile = juce::File::getSpecialLocation(juce::File::currentExecutableFile);
    if (!exeFile.exists()) return;
    
    auto assetFile = exeFile.getParentDirectory().getChildFile("examples").getChildFile("Assets").getChildFile("cello.wav");
    
    if (!assetFile.existsAsFile()) {
      // Also try sibling path
      assetFile = exeFile.getSiblingFile("examples").getChildFile("Assets").getChildFile("cello.wav");
    }
    
    if (assetFile.existsAsFile()) {
      auto stream = assetFile.createInputStream();
      if (stream != nullptr) {
        cachedWavData.reset();
        stream->readIntoMemoryBlock(cachedWavData);
      }
    }
  }
  
  void setupDefaultEQBands() {
      // VERY NOTICEABLE TEST: Low pass at 800Hz - should sound muffled!
      Sphere::EQBandParams p0;
      p0.bypass = false;
      p0.type = Sphere::EQFilterType::HighCut;  // Low pass filter
      p0.frequency = 800.0;  // Cut everything above 800Hz
      p0.q = 0.707;
      p0.slope = Sphere::EQSlope::dB24;  // 24dB/oct
      eqEngine.setBandParameters(0, p0);
  }
  
  // Cached WAV data in memory
  MemoryBlock cachedWavData;
};
