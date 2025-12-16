/*
  ==============================================================================

   This file is part of the JUCE framework examples.
   Copyright (c) Raw Material Software Limited

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   to use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
   REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
   AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
   INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
   LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
   OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
   PERFORMANCE OF THIS SOFTWARE.

  ==============================================================================
*/

/*******************************************************************************
 The block below describes the properties of this PIP. A PIP is a short snippet
 of code that can be read by the Projucer and used to generate a JUCE project.

 BEGIN_JUCE_PIP_METADATA

 name:             AudioSynthesiserDemo
 version:          1.0.0
 vendor:           JUCE
/*
  ==============================================================================

   This file is part of the JUCE framework examples.
   Copyright (c) Raw Material Software Limited

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   to use, copy, modify, and/or distribute this software for any purpose with or
   without fee hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
   REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
   AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
   INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
   LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
   OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
   PERFORMANCE OF THIS SOFTWARE.

  ==============================================================================
*/

/*******************************************************************************
 The block below describes the properties of this PIP. A PIP is a short snippet
 of code that can be read by the Projucer and used to generate a JUCE project.

 BEGIN_JUCE_PIP_METADATA

 name:             AudioSynthesiserDemo
 version:          1.0.0
 vendor:           JUCE
 website:          http://juce.com
 description:      Simple synthesiser application.

 dependencies:     juce_audio_basics, juce_audio_devices, juce_audio_formats,
                   juce_audio_processors, juce_audio_utils, juce_core,
                   juce_data_structures, juce_events, juce_graphics,
                   juce_gui_basics, juce_gui_extra,
juce_audio_processors_headless exporters:        xcode_mac, vs2022, vs2026,
linux_make, androidstudio, xcode_iphone

 moduleFlags:      JUCE_STRICT_REFCOUNTEDPOINTER=1

 type:             Component
 mainClass:        AudioSynthesiserDemo

 useLocalCopy:     1


*******************************************************************************/

#pragma once

#include "DemoUtilities.h"
#include <JuceHeader.h>
#include <algorithm>
#include <atomic>
#include <iostream>

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
      // Sawtooth: 1.0 to -1.0
      return (float)(1.0 - (currentAngle / juce::MathConstants<double>::pi));
    case OscillatorSound::WaveType::Square:
      // Square: 0.5 or -0.5 (reduced amplitude for balance)
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
    WavAudioFormat wavFormat;
    std::unique_ptr<AudioFormatReader> audioReader(wavFormat.createReaderFor(
        createAssetInputStream("cello.wav").release(), true));
    BigInteger allNotes;
    allNotes.setRange(0, 128, true);

    synth.clearSounds();
    synth.addSound(new SamplerSound("demo sound", *audioReader, allNotes, 74,
                                    0.1, 0.1, 10.0));
  }

  void prepareToPlay(int /*samplesPerBlockExpected*/,
                     double sampleRate) override {
    midiCollector.reset(sampleRate);
    synth.setCurrentPlaybackSampleRate(sampleRate);
  }

  void releaseResources() override {}

  void getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) override {
    bufferToFill.clearActiveBufferRegion();
    MidiBuffer incomingMidi;
    midiCollector.removeNextBlockOfMessages(incomingMidi,
                                            bufferToFill.numSamples);
    keyboardState.processNextMidiBuffer(incomingMidi, 0,
                                        bufferToFill.numSamples, true);
    synth.renderNextBlock(*bufferToFill.buffer, incomingMidi, 0,
                          bufferToFill.numSamples);

    // Calculate RMS for visualization
    if (bufferToFill.buffer->getNumChannels() > 0) {
      currentRMS =
          bufferToFill.buffer->getRMSLevel(0, 0, bufferToFill.numSamples);
    }
  }

  std::atomic<float> currentRMS{0.0f};
  MidiMessageCollector midiCollector;
  MidiKeyboardState &keyboardState;
  Synthesiser synth;
};

//==============================================================================
class Callback final : public AudioIODeviceCallback {
public:
  Callback(AudioSourcePlayer &playerIn) : player(playerIn) {}

  void audioDeviceIOCallbackWithContext(
      const float *const *inputChannelData, int numInputChannels,
      float *const *outputChannelData, int numOutputChannels, int numSamples,
      const AudioIODeviceCallbackContext &context) override {
    player.audioDeviceIOCallbackWithContext(
        inputChannelData, numInputChannels, outputChannelData,
        numOutputChannels, numSamples, context);
  }

  void audioDeviceAboutToStart(AudioIODevice *device) override {
    player.audioDeviceAboutToStart(device);
  }
  void audioDeviceStopped() override { player.audioDeviceStopped(); }

private:
  AudioSourcePlayer &player;
};

//==============================================================================
class AudioSynthesiserDemo final : public Component,
                                   public MidiKeyboardState::Listener,
                                   private Timer {
public:
  // Custom WebBrowserComponent to intercept URL calls
  class SphereSynthBrowser : public WebBrowserComponent {
  public:
    SphereSynthBrowser(AudioSynthesiserDemo &owner) : owner(owner) {}

    bool pageAboutToLoad(const String &newURL) override;

  private:
    AudioSynthesiserDemo &owner;
  };

  AudioSynthesiserDemo() {
    addAndMakeVisible(webView);

    // HTML Content for Sphere Synth
    String htmlContent = R"HTML(
            <!DOCTYPE html>
            <html>
            <head>
            <meta charset="UTF-8">
            <style>
              * { margin: 0; padding: 0; box-sizing: border-box; }
              
              body { 
                background: linear-gradient(135deg, #000000 0%, #000510 100%);
                color: white; 
                font-family: 'SF Pro Display', -apple-system, BlinkMacSystemFont, 'Segoe UI', sans-serif;
                display: flex; 
                flex-direction: column; 
                align-items: center; 
                justify-content: space-between; 
                height: 100vh; 
                overflow: hidden; 
                user-select: none; 
                -webkit-user-select: none;
                position: relative;
              }
              
              /* Wavy Background Animation */
              body::before {
                content: '';
                position: absolute;
                top: -50%;
                left: -50%;
                width: 200%;
                height: 200%;
                background: radial-gradient(circle at center, transparent 0%, rgba(0, 40, 80, 0.2) 40%, transparent 70%);
                animation: waveFlow 15s infinite linear;
                pointer-events: none;
                z-index: 0;
              }

              @keyframes waveFlow {
                0% { transform: rotate(0deg) translate(0, 0); }
                25% { transform: rotate(5deg) translate(2%, 2%); }
                50% { transform: rotate(0deg) translate(0, 4%); }
                75% { transform: rotate(-5deg) translate(-2%, 2%); }
                100% { transform: rotate(0deg) translate(0, 0); }
              }
              
              body::before {
                content: '';
                position: absolute;
                width: 100%;
                height: 100%;
                background: transparent;
                pointer-events: none;
              }
              
              .header {
                text-align: center;
                padding: 30px 0 10px;
                z-index: 10;
                position: relative;
              }
              
              .tutorial-btn {
                position: fixed;
                top: 20px;
                right: 20px;
                background: rgba(0, 0, 0, 0.3);
                backdrop-filter: blur(10px);
                border: 1px solid rgba(120, 120, 130, 0.3);
                color: rgba(255, 255, 255, 0.9);
                padding: 8px 16px;
                border-radius: 4px;
                font-family: 'Courier New', monospace;
                font-size: 11px;
                font-weight: 600;
                cursor: pointer;
                transition: all 0.3s;
                z-index: 1000;
                text-transform: uppercase;
                letter-spacing: 1px;
              }
              
              .tutorial-btn:hover {
                background: rgba(120, 120, 130, 0.1);
                border-color: rgba(150, 150, 160, 0.8);
                box-shadow: 0 0 15px rgba(120, 120, 130, 0.3);
              }

              .settings-btn {
                position: fixed;
                top: 20px;
                left: 20px;
                background: rgba(0, 0, 0, 0.3);
                backdrop-filter: blur(10px);
                border: 1px solid rgba(120, 120, 130, 0.3);
                color: rgba(255, 255, 255, 0.9);
                padding: 8px 16px;
                border-radius: 4px;
                font-family: 'Courier New', monospace;
                font-size: 11px;
                font-weight: 600;
                cursor: pointer;
                transition: all 0.3s;
                z-index: 1000;
                text-transform: uppercase;
                letter-spacing: 1px;
              }

              .settings-btn:hover {
                background: rgba(120, 120, 130, 0.1);
                border-color: rgba(150, 150, 160, 0.8);
                box-shadow: 0 0 15px rgba(120, 120, 130, 0.3);
              }
              
              h1 { 
                font-size: 42px;
                font-weight: 200;
                letter-spacing: 12px;
                color: #ffffff;
                text-shadow: 0 0 20px rgba(255, 255, 255, 0.5);
                margin-bottom: 8px;
                cursor: default;
                user-select: none;
              }
              
              @keyframes shimmer {
                0%, 100% {
                  background-position: 0% center;
                }
                50% {
                  background-position: 100% center;
                }
              }
              
              .tagline {
                font-size: 12px;
                letter-spacing: 3px;
                color: rgba(255, 255, 255, 0.5);
                text-transform: uppercase;
              }
              
              .sphere-container { 
                position: fixed;
                top: 80px;
                left: 0;
                width: 100%; 
                bottom: 180px;
                z-index: 1;
                display: flex;
                align-items: center;
                justify-content: center;
              }
              
              canvas {
                display: block;
                width: 100%;
                height: 100%;
              }

              .header {
                text-align: center;
                padding: 30px 0 10px;
                z-index: 10;
                position: relative;
                pointer-events: none; /* Let clicks pass through header area */
              }
              
              .header h1, .header .tagline, .tutorial-btn, .settings-btn {
                pointer-events: auto; /* Re-enable clicks for buttons/text */
              }

              .controls { 
                display: flex; 
                gap: 12px; 
                z-index: 10;
                position: fixed;
                bottom: 80px;
                left: 50%;
                transform: translateX(-50%);
              }

              .keyboard-container {
                width: 100%;
                display: flex;
                justify-content: center;
                padding: 0 20px 30px;
                z-index: 10;
                position: relative;
              }
              
              .tutorial-popup {
                position: absolute;
                background: rgba(10, 10, 10, 0.95);
                backdrop-filter: blur(20px);
                border: 2px solid rgba(0, 229, 255, 0.5);
                border-radius: 16px;
                padding: 20px;
                max-width: 300px;
                z-index: 1000;
                box-shadow: 0 10px 40px rgba(0, 0, 0, 0.8),
                            0 0 20px rgba(0, 229, 255, 0.3);
                display: none;
              }
              
              .tutorial-popup.active {
                display: block;
                animation: popup-appear 0.3s ease-out;
              }
              
              @keyframes popup-appear {
                from { opacity: 0; transform: translateX(-50%) scale(0.9) translateY(-10px); }
                to { opacity: 1; transform: translateX(-50%) scale(1) translateY(0); }
              }

              @keyframes popup-appear-centered {
                from { opacity: 0; transform: translate(-50%, -50%) scale(0.9); }
                to { opacity: 1; transform: translate(-50%, -50%) scale(1); }
              }
              
              .popup-title {
                color: #00e5ff;
                font-size: 16px;
                font-weight: 700;
                margin-bottom: 10px;
                letter-spacing: 1px;
              }
              
              .popup-text {
                color: rgba(255, 255, 255, 0.8);
                font-size: 13px;
                line-height: 1.6;
                margin-bottom: 15px;
              }
              
              .popup-dismiss {
                background: linear-gradient(135deg, rgba(0, 229, 255, 0.2), rgba(0, 200, 255, 0.1));
                border: 1px solid #00e5ff;
                color: white;
                padding: 8px 16px;
                border-radius: 20px;
                font-size: 12px;
                font-weight: 600;
                cursor: pointer;
                transition: all 0.3s;
                width: 100%;
                text-transform: uppercase;
                letter-spacing: 1px;
              }
              
              .popup-dismiss:hover {
                background: linear-gradient(135deg, rgba(0, 229, 255, 0.3), rgba(0, 200, 255, 0.2));
                box-shadow: 0 0 15px rgba(0, 229, 255, 0.4);
              }

              .settings-select {
                width: 100%;
                background: rgba(30, 30, 30, 0.8);
                backdrop-filter: blur(10px);
                border: 1px solid rgba(0, 229, 255, 0.3);
                color: white;
                padding: 10px;
                border-radius: 8px;
                outline: none;
                font-family: inherit;
                font-size: 12px;
                appearance: none;
                background-image: url("data:image/svg+xml;charset=US-ASCII,%3Csvg%20xmlns%3D%22http%3A%2F%2Fwww.w3.org%2F2000%2Fsvg%22%20width%3D%22292.4%22%20height%3D%22292.4%22%3E%3Cpath%20fill%3D%22%2300e5ff%22%20d%3D%22M287%2069.4a17.6%2017.6%200%200%200-13-5.4H18.4c-5%200-9.3%201.8-12.9%205.4A17.6%2017.6%200%200%200%200%2082.2c0%205%201.8%209.3%205.4%2012.9l128%20127.9c3.6%203.6%207.8%205.4%2012.8%205.4s9.2-1.8%2012.8-5.4L287%2095c3.5-3.5%205.4-7.8%205.4-12.8%200-5-1.9-9.2-5.5-12.8z%22%2F%3E%3C%2Fsvg%3E");
                background-repeat: no-repeat;
                background-position: right 10px center;
                background-size: 10px;
                cursor: pointer;
                transition: all 0.3s;
              }
              .settings-select:hover {
                border-color: #00e5ff;
                box-shadow: 0 0 10px rgba(0, 229, 255, 0.2);
              }
              
              #popup-sphere { top: 200px; left: 50%; transform: translateX(-50%); }
              #popup-controls { top: 420px; left: 50%; transform: translateX(-50%); }
              #popup-keyboard { bottom: 200px; left: 50%; transform: translateX(-50%); }
              
              .controls { 
                display: flex; 
                gap: 16px; 
                margin-bottom: 30px;
                z-index: 10;
              }
              
              button { 
                background: rgba(255, 255, 255, 0.05);
                backdrop-filter: blur(10px);
                -webkit-backdrop-filter: blur(10px);
                color: rgba(255, 255, 255, 0.9);
                border: 1px solid rgba(0, 229, 255, 0.3);
                padding: 10px 20px; 
                border-radius: 20px; 
                font-size: 11px;
                font-weight: 600;
                letter-spacing: 1px;
                cursor: pointer; 
                transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
                outline: none;
                text-transform: uppercase;
                position: relative;
                overflow: hidden;
              }
              
              button::before {
                content: '';
                position: absolute;
                top: 50%;
                left: 50%;
                width: 0;
                height: 0;
                border-radius: 50%;
                background: rgba(0, 229, 255, 0.3);
                transform: translate(-50%, -50%);
                transition: width 0.6s, height 0.6s;
              }
              
              button:hover::before {
                width: 300px;
                height: 300px;
              }
              
              button:hover {
                border-color: rgba(0, 229, 255, 0.8);
                box-shadow: 0 0 20px rgba(0, 229, 255, 0.3);
                transform: translateY(-2px);
              }
              
              button.active { 
                background: linear-gradient(135deg, rgba(0, 229, 255, 0.3), rgba(0, 200, 255, 0.2));
                border-color: #00e5ff;
                color: white;
                box-shadow: 0 0 30px rgba(0, 229, 255, 0.6),
                            inset 0 0 20px rgba(0, 229, 255, 0.2);
              }
              
              .keyboard-container {
                width: 100%;
                display: flex;
                justify-content: center;
                padding: 0 20px;
                z-index: 10;
                position: fixed;
                bottom: 20px;
                left: 0;
              }
              
              .keyboard { 
                display: flex;
                height: 80px;
                background: linear-gradient(180deg, #1a1a1a 0%, #0a0a0a 100%);
                padding: 10px 16px;
                border-radius: 16px;
                box-shadow: 0 10px 40px rgba(0, 0, 0, 0.7),
                            inset 0 1px 0 rgba(255, 255, 255, 0.05);
                position: relative;
                gap: 3px;
              }
              
              .keyboard::before {
                content: '';
                position: absolute;
                top: 0;
                left: 0;
                right: 0;
                height: 2px;
                background: linear-gradient(90deg, transparent, rgba(0, 229, 255, 0.3), transparent);
                border-radius: 16px 16px 0 0;
              }
              
              .key { 
                flex: 1;
                background: linear-gradient(180deg, #ffffff 0%, #f5f5f5 50%, #e8e8e8 100%);
                border: 1px solid #333;
                border-radius: 0 0 8px 8px;
                position: relative;
                cursor: pointer;
                margin: 0;
                margin: 0;
                min-width: 24px;
                box-shadow: 0 4px 8px rgba(0, 0, 0, 0.3),
                            inset 0 -2px 4px rgba(0, 0, 0, 0.1),
                            inset 0 1px 0 rgba(255, 255, 255, 0.8);
                transition: all 0.1s ease;
              }
              
              .key:hover {
                background: linear-gradient(180deg, #ffffff 0%, #fafafa 50%, #f0f0f0 100%);
                box-shadow: 0 2px 4px rgba(0, 0, 0, 0.3),
                            inset 0 -1px 2px rgba(0, 0, 0, 0.1),
                            0 0 15px rgba(0, 229, 255, 0.2);
              }
              
              .key.black { 
                background: linear-gradient(180deg, #2a2a2a 0%, #1a1a1a 50%, #0a0a0a 100%);
                height: 55%;
                width: 24px;
                flex: 0 0 24px;
                margin: 0 -12px;
                z-index: 10;
                border: 1px solid #000;
                border-radius: 0 0 6px 6px;
                box-shadow: 0 4px 8px rgba(0, 0, 0, 0.8),
                            inset 0 1px 0 rgba(255, 255, 255, 0.1),
                            inset 0 -2px 4px rgba(0, 0, 0, 0.5);
              }
              
              .key.black:hover {
                background: linear-gradient(180deg, #353535 0%, #252525 50%, #151515 100%);
                box-shadow: 0 2px 4px rgba(0, 0, 0, 0.8),
                            0 0 15px rgba(0, 229, 255, 0.3);
              }
              
              .key:active, .key.pressed { 
                background: linear-gradient(180deg, #00e5ff 0%, #00b8d4 100%);
                box-shadow: inset 0 4px 8px rgba(0, 0, 0, 0.3),
                            0 0 30px rgba(0, 229, 255, 0.8);
                transform: translateY(2px);
              }
              


              .volume-meter-container {
                position: fixed;
                right: 20px;
                top: 50%;
                transform: translateY(-50%);
                width: 4px;
                height: 300px;
                background: rgba(255, 255, 255, 0.1);
                border: 1px solid rgba(255, 255, 255, 0.2);
                border-radius: 2px;
                overflow: hidden;
                z-index: 100;
                box-shadow: 0 0 10px rgba(0,0,0,0.5);
              }

              .footer-credit {
                position: fixed;
                bottom: 30px;
                left: 30px;
                font-family: 'Courier New', monospace;
                font-size: 10px;
                color: rgba(255, 255, 255, 0.4);
                letter-spacing: 2px;
                text-transform: uppercase;
                z-index: 1000;
                pointer-events: none;
                text-shadow: 0 0 5px rgba(0, 0, 0, 0.5);
              }

              .volume-meter-bar {
                width: 100%;
                height: 0%;
                background: linear-gradient(to top, #00e5ff, #00ff88);
                position: absolute;
                bottom: 0;
                transition: height 0.05s linear;
                box-shadow: 0 0 15px #00e5ff;
              }
            </style>
            </head>
            <body>
              <div class="header">
                <button class="settings-btn" onclick="toggleSettings()">Settings</button>
                <button class="tutorial-btn" id="tutorialBtn" onclick="toggleTutorial()">? Tutorial</button>
                <h1>SPHERE SYNTH</h1>
              </div>
              
              <div class="footer-credit">Sphere DSP</div>
              
              <!-- Tutorial Popups -->
              <div id="popup-sphere" class="tutorial-popup">
                <div class="popup-title">Sphere Visualizer</div>
                <div class="popup-text">This 3D sphere reacts to your notes. It glows and pulses when you play, with water ripples emanating outward at frequencies matching your notes.</div>
                <button class="popup-dismiss" onclick="dismissPopup('popup-sphere')">Next</button>
              </div>
              
              <div id="popup-controls" class="tutorial-popup">
                <div class="popup-title">Sound Selection</div>
                <div class="popup-text">Choose between a pure Sine Wave or a rich Sampled Cello sound. Each gives a different character to your performance.</div>
                <button class="popup-dismiss" onclick="dismissPopup('popup-controls')">Next</button>
              </div>
              
              <div id="popup-keyboard" class="tutorial-popup">
                <div class="popup-title">Piano Keyboard</div>
                <div class="popup-text">Click or tap the keys to play notes. The white keys are natural notes, black keys are sharps/flats. Watch the ripples change speed with different pitches!</div>
                <button class="popup-dismiss" onclick="dismissPopup('popup-keyboard')">Got It!</button>
              </div>
              
              <div class="sphere-container">
                <canvas id="glCanvas"></canvas>
              </div>
              
              <div class="controls">
                <button onclick="selectSound('sine', this)" class="active">Sine</button>
                <button onclick="selectSound('saw', this)">Saw</button>
                <button onclick="selectSound('square', this)">Square</button>
                <button onclick="selectSound('sampled', this)">Cello</button>
              </div>
              
              <div id="settings-modal" class="tutorial-popup" style="max-width: 400px; top: 50%; left: 50%; transform: translate(-50%, -50%); animation-name: popup-appear-centered;">
                <div class="popup-title">Settings</div>
                
                <div style="margin-bottom: 15px;">
                  <label style="display: block; color: rgba(255,255,255,0.6); font-size: 11px; margin-bottom: 5px; text-transform: uppercase; letter-spacing: 1px;">Audio Output</label>
                  <select id="audio-output" onchange="setAudioOutput(this.value)" class="settings-select">
                    <option>Loading...</option>
                  </select>
                </div>
                
                <div style="margin-bottom: 20px;">
                  <label style="display: block; color: rgba(255,255,255,0.6); font-size: 11px; margin-bottom: 5px; text-transform: uppercase; letter-spacing: 1px;">MIDI Input</label>
                  <select id="midi-input" onchange="setMidiInput(this.value)" class="settings-select">
                    <option>Loading...</option>
                  </select>
                </div>
                
                <button class="popup-dismiss" onclick="toggleSettings()">Close</button>
              </div>
              
              <div class="keyboard-container">
                <div class="keyboard" id="keyboard"></div>
              </div>

              <div class="volume-meter-container">
                <div class="volume-meter-bar" id="volume-bar"></div>
              </div>
            </body>
            <script>
                const canvas = document.getElementById('glCanvas');
                const ctx = canvas.getContext('2d');
                
                function resizeCanvas() {
                    const container = canvas.parentElement;
                    const rect = container.getBoundingClientRect();
                    const dpr = window.devicePixelRatio || 1;
                    
                    canvas.width = rect.width * dpr;
                    canvas.height = rect.height * dpr;
                    
                    ctx.scale(dpr, dpr);
                    
                    canvas.style.width = rect.width + 'px';
                    canvas.style.height = rect.height + 'px';
                }
                window.addEventListener('resize', resizeCanvas);
                resizeCanvas();
                
                // NCS-style Particle System
                class Particle {
                    constructor(x, y, angle) {
                        this.x = x;
                        this.y = y;
                        this.baseAngle = angle;
                        this.speed = 0.3 + Math.random() * 0.5;
                        this.offset = Math.random() * Math.PI * 2;
                        this.size = 1.5 + Math.random() * 1.5;
                        this.opacity = 0.6 + Math.random() * 0.4;
                        this.hue = 180 + Math.random() * 60;
                        this.vibrationPhase = Math.random() * Math.PI * 2;
                    }
                    
                    update(centerX, centerY, time, audioIntensity, vibrationSpeed) {
                        // Calculate interpolated speed first
                        // Idle (small sphere): Constant slow speed (0.2)
                        // Active (large sphere): Frequency dependent (0.5 - 8.0)
                        const idleSpeed = 0.2;
                        const currentSpeed = idleSpeed + (vibrationSpeed - idleSpeed) * audioIntensity;

                        // Accumulate phase based on interpolated speed to prevent jumps and ensure idle calm
                        this.vibrationPhase += currentSpeed * 0.05;
                        
                        const noise = Math.sin(this.vibrationPhase + this.offset) * 0.3;
                        const angle = this.baseAngle + noise;
                        
                        // Tighter hole when inactive
                        const effectiveRadius = Math.min(canvas.width / (window.devicePixelRatio||1), canvas.height / (window.devicePixelRatio||1)) / 2;
                        // Increased base distance for slightly larger idle size (60 -> 80)
                        const baseDistance = Math.min(80, effectiveRadius * 0.35); 
                        
                        // Reduced expansion multiplier from 100 to 60 for less extreme growth
                        const distance = baseDistance + Math.sin(time * 0.3 + this.offset) * 20 + audioIntensity * 60;
                        
                        this.x = centerX + Math.cos(angle) * distance;
                        this.y = centerY + Math.sin(angle) * distance;
                        
                        // Rotate particles slightly for tech feel
                        this.baseAngle += 0.005 * currentSpeed * (this.offset > Math.PI ? 1 : -1);
                    }
                    
                    draw(ctx) {
                        // Tech look: Squares/Pixels
                        const size = this.size * 0.8;
                        ctx.fillStyle = `hsla(${this.hue}, 80%, 60%, ${this.opacity})`;
                        ctx.shadowBlur = 10;
                        ctx.shadowColor = `hsla(${this.hue}, 90%, 60%, 0.8)`;
                        ctx.fillRect(this.x - size/2, this.y - size/2, size, size);
                        ctx.shadowBlur = 0;
                    }
                }
                
                const numParticles = 500;
                const particles = [];
                
                for (let i = 0; i < numParticles; i++) {
                    const angle = (i / numParticles) * Math.PI * 2;
                    particles.push(new Particle(canvas.width / 2, canvas.height / 2, angle));
                }

                
                // Audio tracking
                let activeNotes = 0;
                let currentNotes = new Map();
                let audioIntensity = 0;
                let vibrationSpeed = 0.5; // Default speed
                let time = 0;
                
                // Keyboard setup
                const keyboard = document.getElementById('keyboard');
                const keys = ['C', 'C#', 'D', 'D#', 'E', 'F', 'F#', 'G', 'G#', 'A', 'A#', 'B'];
                
                // 3 Octaves (36 keys)
                for (let i = 48; i < 84; i++) {
                    const key = document.createElement('div');
                    const noteName = keys[i % 12];
                    const isBlack = noteName.includes('#');
                    key.className = `key ${isBlack ? 'black' : 'white'}`;
                    key.dataset.note = i;
                    
                    key.addEventListener('mousedown', () => noteOn(i));
                    key.addEventListener('mouseup', () => noteOff(i));
                    key.addEventListener('mouseleave', () => {
                        if (currentNotes.has(i)) noteOff(i);
                    });
                    keyboard.appendChild(key);
                }
                
                function toggleTutorial() {
                    const popups = ['popup-sphere', 'popup-controls', 'popup-keyboard'];
                    const anyActive = popups.some(id => document.getElementById(id).classList.contains('active'));
                    
                    if (!anyActive) {
                        document.getElementById('popup-sphere').classList.add('active');
                    } else {
                        popups.forEach(id => document.getElementById(id).classList.remove('active'));
                    }
                }
                
                function dismissPopup(id) {
                  document.getElementById(id).classList.remove('active');
                  
                  // Sequential flow
                  if (id === 'popup-sphere') {
                    setTimeout(() => document.getElementById('popup-controls').classList.add('active'), 200);
                  } else if (id === 'popup-controls') {
                    setTimeout(() => document.getElementById('popup-keyboard').classList.add('active'), 200);
                  }
                }
                
                function updateVibrationSpeed(note) {
                  if (currentNotes.size === 0) {
                    // Don't reset vibrationSpeed here! 
                    // Let it stay at the last note's speed so the decay preserves the character.
                    // The idle speed blending is handled in Particle.update via audioIntensity.
                    return;
                  }
                  
                  // Calculate average frequency of active notes
                  let totalFreq = 0;
                  for (let n of currentNotes.keys()) {
                    // Convert MIDI note to frequency (A4 = 440Hz)
                    totalFreq += 440 * Math.pow(2, (n - 69) / 12);
                  }
                  const avgFreq = totalFreq / currentNotes.size;
                  
                  // Map frequency to vibration speed
                  // Bass notes (low freq): slow vibration (0.5)
                  // Treble notes (high freq): fast vibration (8.0)
                  const minFreq = 130; // C3
                  const maxFreq = 1046; // C6
                  const normalized = Math.max(0, Math.min(1, (avgFreq - minFreq) / (maxFreq - minFreq)));
                  vibrationSpeed = 0.5 + normalized * 7.5; // Range: 0.5 to 8.0
                }
                
                // Settings Logic
                function toggleSettings() {
                  const modal = document.getElementById('settings-modal');
                  const isVisible = modal.classList.contains('active');
                  
                  if (!isVisible) {
                    modal.classList.add('active');
                    // Request device list from C++
                    window.location = 'sphere://getDevices';
                  } else {
                    modal.classList.remove('active');
                  }
                }
                
                function setAudioOutput(index) {
                  window.location = 'sphere://setAudioOutput/' + index;
                }
                
                function setMidiInput(index) {
                  window.location = 'sphere://setMidiInput/' + index;
                }

                function selectSound(type, btn) {
                    document.querySelectorAll('.controls button').forEach(b => b.classList.remove('active'));
                    if(btn) btn.classList.add('active');
                    window.location = 'sphere://sound/' + type;
                }

                function updateMeter(level) {
                    const bar = document.getElementById('volume-bar');
                    // Level is 0.0 to 1.0
                    const percentage = Math.min(100, Math.max(0, level * 100));
                    bar.style.height = percentage + '%';
                    bar.style.opacity = 0.5 + (level * 0.5);
                }
                
                // Called from C++ to populate dropdowns
                function populateDevices(jsonString) {
                  try {
                    const data = JSON.parse(jsonString);
                    
                    const audioSelect = document.getElementById('audio-output');
                    audioSelect.innerHTML = '';
                    data.audioOutputs.forEach((name, index) => {
                      const option = document.createElement('option');
                      option.text = name;
                      option.value = index;
                      if (index === data.currentAudioOutput) option.selected = true;
                      audioSelect.add(option);
                    });
                    
                    const midiSelect = document.getElementById('midi-input');
                    midiSelect.innerHTML = '';
                    data.midiInputs.forEach((name, index) => {
                      const option = document.createElement('option');
                      option.text = name;
                      option.value = index;
                      if (index === data.currentMidiInput) option.selected = true;
                      midiSelect.add(option);
                    });
                    
                  } catch(e) {
                    console.error("Error parsing device data", e);
                  }

                }

                // Note Handling
                function noteOn(note) {
                    if (currentNotes.has(note)) return;
                    
                    activeNotes++;
                    currentNotes.set(note, Date.now());
                    updateVibrationSpeed(note);
                    
                    // Visuals
                    const key = document.querySelector(`.key[data-note="${note}"]`);
                    if (key) key.classList.add('pressed');
                    
                    // Send to C++
                    window.location = 'sphere://noteOn/' + note + '/100';
                }
                
                function noteOff(note) {
                    if (!currentNotes.has(note)) return;
                    
                    activeNotes = Math.max(0, activeNotes - 1);
                    currentNotes.delete(note);
                    updateVibrationSpeed(note); // Update speed based on remaining notes
                    
                    // Visuals
                    const key = document.querySelector(`.key[data-note="${note}"]`);
                    if (key) key.classList.remove('pressed');
                    
                    // Send to C++
                    window.location = 'sphere://noteOff/' + note + '/0';
                }
                
                // Called from C++ for external MIDI
                function visualizeNoteOn(note) {
                    if (currentNotes.has(note)) return;
                    activeNotes++;
                    currentNotes.set(note, Date.now());
                    updateVibrationSpeed(note);
                    
                    const key = document.querySelector(`.key[data-note="${note}"]`);
                    if (key) key.classList.add('pressed');
                }
                
                function visualizeNoteOff(note) {
                    if (!currentNotes.has(note)) return;
                    activeNotes = Math.max(0, activeNotes - 1);
                    currentNotes.delete(note);
                    updateVibrationSpeed(note);
                    
                    const key = document.querySelector(`.key[data-note="${note}"]`);
                    if (key) key.classList.remove('pressed');
                }

                // Animation loop
                function render() {
                    time += 0.016; // ~60fps
                    
                    // Smooth audio intensity transitions
                    const targetIntensity = activeNotes > 0 ? 1.0 : 0.0;
                    audioIntensity += (targetIntensity - audioIntensity) * 0.1;
                    
                    // Clear canvas to transparent
                    ctx.clearRect(0, 0, canvas.width, canvas.height);
                    
                    // Calculate logical center (accounting for DPI scaling)
                    const dpr = window.devicePixelRatio || 1;
                    const centerX = (canvas.width / dpr) / 2;
                    const centerY = (canvas.height / dpr) / 2;
                    
                    // Update and draw particles
                    particles.forEach(particle => {
                        particle.update(centerX, centerY, time, audioIntensity, vibrationSpeed);
                        particle.draw(ctx);
                    });
                    
                    ctx.beginPath();
                    // Removed dashed ring as requested
                    ctx.setLineDash([]); // Reset dash
                    


                    requestAnimationFrame(render);
                }
                
                
                render();
            </script>
            </html>
        )HTML";

    File tempFile = File::getSpecialLocation(File::tempDirectory)
                        .getChildFile("sphere_synth.html");
    tempFile.replaceWithText(htmlContent);
    webView.goToURL(URL(tempFile).toString(false));

    audioSourcePlayer.setSource(&synthAudioSource);

#ifndef JUCE_DEMO_RUNNER
    audioDeviceManager.initialise(0, 2, nullptr, true, {}, nullptr);
#endif

    audioDeviceManager.addAudioCallback(&callback);
    audioDeviceManager.addMidiInputDeviceCallback(
        {}, &(synthAudioSource.midiCollector));
    audioDeviceManager.addMidiInputDeviceCallback(
        {}, &(synthAudioSource.midiCollector));
    keyboardState.addListener(this);
    startTimerHz(30);

    setOpaque(true);
    setSize(800, 600);
  }

  ~AudioSynthesiserDemo() override {
    audioSourcePlayer.setSource(nullptr);
    audioDeviceManager.removeMidiInputDeviceCallback(
        {}, &(synthAudioSource.midiCollector));
    audioDeviceManager.removeAudioCallback(&callback);
    keyboardState.removeListener(this);
    stopTimer();
  }

  void timerCallback() override {
    float level = synthAudioSource.currentRMS;
    // Boost the level slightly for better visual response
    level = std::min(1.0f, level * 6.0f);
    webView.evaluateJavascript("updateMeter(" + String(level) + ")");
  }

  void resized() override { webView.setBounds(getLocalBounds()); }
  void handleNoteOn(MidiKeyboardState *, int midiChannel, int midiNoteNumber,
                    float velocity) override;
  void handleNoteOff(MidiKeyboardState *, int midiChannel, int midiNoteNumber,
                     float velocity) override;
  void handleSphereCommand(const String &url);
  void sendDevicesToUI();

private:
#ifndef JUCE_DEMO_RUNNER
  AudioDeviceManager audioDeviceManager;
#else
  AudioDeviceManager &audioDeviceManager{getSharedAudioDeviceManager(0, 2)};
#endif

  MidiKeyboardState keyboardState;
  AudioSourcePlayer audioSourcePlayer;
  SynthAudioSource synthAudioSource{keyboardState};

  SphereSynthBrowser webView{*this};
  Callback callback{audioSourcePlayer};

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioSynthesiserDemo)
};

inline bool AudioSynthesiserDemo::SphereSynthBrowser::pageAboutToLoad(
    const String &newURL) {
  if (newURL.startsWith("sphere://")) {
    owner.handleSphereCommand(newURL);
    return false; // Don't actually load the URL
  }
  return true;
}

inline void AudioSynthesiserDemo::handleNoteOn(MidiKeyboardState *,
                                               int midiChannel,
                                               int midiNoteNumber,
                                               float velocity) {
  MessageManager::callAsync([this, midiNoteNumber, velocity]() {
    webView.evaluateJavascript("visualizeNoteOn(" + String(midiNoteNumber) +
                               ")");
  });
}

inline void AudioSynthesiserDemo::handleNoteOff(MidiKeyboardState *,
                                                int midiChannel,
                                                int midiNoteNumber,
                                                float velocity) {
  MessageManager::callAsync([this, midiNoteNumber]() {
    webView.evaluateJavascript("visualizeNoteOff(" + String(midiNoteNumber) +
                               ")");
  });
}

inline void AudioSynthesiserDemo::handleSphereCommand(const String &url) {
  std::cout << "IPC Received: " << url << std::endl;
  String cmd = url.substring(9); // remove sphere://
  StringArray parts;
  parts.addTokens(cmd, "/", "");

  if (parts[0] == "noteOn") {
    int note = parts[1].getIntValue();
    int velocity = parts[2].getIntValue();
    keyboardState.noteOn(1, note, (float)velocity / 127.0f);
  } else if (parts[0] == "noteOff") {
    int note = parts[1].getIntValue();
    keyboardState.noteOff(1, note, 0.0f);
  } else if (parts[0] == "sound") {
    if (parts[1] == "sine") {
      synthAudioSource.setUsingSineWaveSound();
    } else if (parts[1] == "saw") {
      synthAudioSource.setUsingSawWaveSound();
    } else if (parts[1] == "square") {
      synthAudioSource.setUsingSquareWaveSound();
    } else if (parts[1] == "sampled") {
      synthAudioSource.setUsingSampledSound();
    }
  } else if (parts[0] == "getDevices") {
    sendDevicesToUI();
  } else if (parts[0] == "setAudioOutput") {
    int index = parts[1].getIntValue();
    auto &deviceTypes = audioDeviceManager.getAvailableDeviceTypes();
    if (deviceTypes.size() > 0) {
      auto *type = deviceTypes[0]; // Assuming default type
      type->scanForDevices();
      auto outputNames = type->getDeviceNames(false); // false for output
      if (index >= 0 && index < outputNames.size()) {
        AudioDeviceManager::AudioDeviceSetup setup;
        audioDeviceManager.getAudioDeviceSetup(setup);
        setup.outputDeviceName = outputNames[index];
        audioDeviceManager.setAudioDeviceSetup(setup, true);
        sendDevicesToUI(); // Refresh UI
      }
    }
  } else if (parts[0] == "setMidiInput") {
    int index = parts[1].getIntValue();
    auto midiInputs = MidiInput::getAvailableDevices();
    if (index >= 0 && index < midiInputs.size()) {
      auto newId = midiInputs[index].identifier;
      if (!audioDeviceManager.isMidiInputDeviceEnabled(newId)) {
        audioDeviceManager.setMidiInputDeviceEnabled(newId, true);
        audioDeviceManager.addMidiInputDeviceCallback(
            newId, &(synthAudioSource.midiCollector));
      }
      sendDevicesToUI();
    }
  }
}

inline void AudioSynthesiserDemo::sendDevicesToUI() {
  // Audio Outputs
  String audioOutputsJson = "[";
  auto &deviceTypes = audioDeviceManager.getAvailableDeviceTypes();
  int currentOutputIndex = -1;

  if (deviceTypes.size() > 0) {
    auto *type = deviceTypes[0];
    type->scanForDevices();
    auto outputNames = type->getDeviceNames(false);
    auto currentSetup = audioDeviceManager.getCurrentAudioDevice();
    String currentName = currentSetup ? currentSetup->getName() : "";

    for (int i = 0; i < outputNames.size(); ++i) {
      audioOutputsJson += "\"" + outputNames[i] + "\"";
      if (i < outputNames.size() - 1)
        audioOutputsJson += ",";
      if (outputNames[i] == currentName)
        currentOutputIndex = i;
    }
  }
  audioOutputsJson += "]";

  // MIDI Inputs
  String midiInputsJson = "[";
  auto midiInputs = MidiInput::getAvailableDevices();
  int currentMidiIndex = -1;

  for (int i = 0; i < midiInputs.size(); ++i) {
    midiInputsJson += "\"" + midiInputs[i].name + "\"";
    if (i < midiInputs.size() - 1)
      midiInputsJson += ",";
    if (audioDeviceManager.isMidiInputDeviceEnabled(midiInputs[i].identifier))
      currentMidiIndex = i;
  }
  midiInputsJson += "]";

  String json = "{ \"audioOutputs\": " + audioOutputsJson +
                ", \"currentAudioOutput\": " + String(currentOutputIndex) +
                ", \"midiInputs\": " + midiInputsJson +
                ", \"currentMidiInput\": " + String(currentMidiIndex) + " }";

  webView.evaluateJavascript("populateDevices('" + json.replace("'", "\\'") +
                             "')");
}
