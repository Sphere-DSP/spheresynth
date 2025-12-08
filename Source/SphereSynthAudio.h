#pragma once
#include <JuceHeader.h>
#include "DemoUtilities.h"
#include <atomic>
#include <algorithm>

// Include new EQ system (header-only design)
#include "EQ/SphereEQTypes.h"
#include "EQ/SphereEQCookbook.h"
#include "EQ/SphereEQBiquad.h"
#include "EQ/SphereEQBandProcessor.h"
#include "EQ/SphereEQEngineV2.h"

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
    level = velocity * 0.15;
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

  using SynthesiserVoice::renderNextBlock;

private:
  float getNextSample() {
    switch (currentWaveType) {
    case OscillatorSound::WaveType::Sine:
      return (float)std::sin(currentAngle);
    case OscillatorSound::WaveType::Saw:
      return (float)(1.0 - (currentAngle / juce::MathConstants<double>::pi));
    case OscillatorSound::WaveType::Square:
      return currentAngle < juce::MathConstants<double>::pi ? 0.5f : -0.5f;
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
    
    // Use cached WAV data from memory instead of reading from disk each time
    if (cachedWavData.getSize() > 0) {
      WavAudioFormat wavFormat;
      auto memStream = std::make_unique<MemoryInputStream>(cachedWavData, false);
      std::unique_ptr<AudioFormatReader> reader(wavFormat.createReaderFor(memStream.release(), true));
      
      if (reader != nullptr) {
        BigInteger allNotes;
        allNotes.setRange(0, 128, true);
        synth.addSound(new SamplerSound("demo sound", *reader, allNotes, 74,
                                        0.1, 0.1, 10.0));
      }
    }
  }

  void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override {
    midiCollector.reset(sampleRate);
    synth.setCurrentPlaybackSampleRate(sampleRate);
    
    // Use actual host block size instead of fixed 4096
    // This optimizes buffer allocation for the real processing scenario
    int actualBlockSize = juce::jmax(samplesPerBlockExpected, 256);
    eqEngine.prepare(sampleRate, actualBlockSize, 2);
    
    setupDefaultEQBands();
  }

  void releaseResources() override {
      eqEngine.reset();
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

    // Apply Sphere EQ V2
    eqEngine.processBlock(*bufferToFill.buffer, incomingMidi);

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

private:
  // Cache WAV file data in memory at construction time to avoid disk I/O later
  void cacheSampledSound() {
    auto stream = createAssetInputStream("cello.wav");
    if (stream != nullptr) {
      cachedWavData.reset();
      stream->readIntoMemoryBlock(cachedWavData);
    }
  }
  
  void setupDefaultEQBands() {
      DBG("=== Setting up default EQ bands ===");
      
      // VERY NOTICEABLE TEST: Low pass at 800Hz - should sound muffled!
      Sphere::EQBandParams p0;
      p0.bypass = false;
      p0.type = Sphere::EQFilterType::HighCut;  // Low pass filter
      p0.frequency = 800.0;  // Cut everything above 800Hz
      p0.q = 0.707;
      p0.slope = Sphere::EQSlope::dB24;  // 24dB/oct
      eqEngine.setBandParameters(0, p0);
      
      DBG("=== EQ Setup complete: HighCut at 800Hz 24dB/oct ===");
      DBG("=== Sound should be noticeably muffled! ===");
  }
  
  // Cached WAV data in memory
  MemoryBlock cachedWavData;
};
