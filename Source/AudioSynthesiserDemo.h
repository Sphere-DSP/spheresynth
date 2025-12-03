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

 END_JUCE_PIP_METADATA

*******************************************************************************/

#pragma once

#include "DemoUtilities.h"
#include <iostream>

//==============================================================================
/** Our demo synth sound is just a basic sine wave.. */
struct SineWaveSound final : public SynthesiserSound {
  bool appliesToNote(int /*midiNoteNumber*/) override { return true; }
  bool appliesToChannel(int /*midiChannel*/) override { return true; }
};

//==============================================================================
/** Our demo synth voice just plays a sine wave.. */
struct SineWaveVoice final : public SynthesiserVoice {
  bool canPlaySound(SynthesiserSound *sound) override {
    return dynamic_cast<SineWaveSound *>(sound) != nullptr;
  }

  void startNote(int midiNoteNumber, float velocity, SynthesiserSound *,
                 int /*currentPitchWheelPosition*/) override {
    currentAngle = 0.0;
    level = velocity * 0.15;
    tailOff = 0.0;

    auto cyclesPerSecond = MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    auto cyclesPerSample = cyclesPerSecond / getSampleRate();

    angleDelta = cyclesPerSample * MathConstants<double>::twoPi;
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

  void renderNextBlock(AudioBuffer<float> &outputBuffer, int startSample,
                       int numSamples) override {
    if (!approximatelyEqual(angleDelta, 0.0)) {
      if (tailOff > 0.0) {
        while (--numSamples >= 0) {
          auto currentSample =
              (float)(std::sin(currentAngle) * level * tailOff);

          for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
            outputBuffer.addSample(i, startSample, currentSample);

          currentAngle += angleDelta;
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
          auto currentSample = (float)(std::sin(currentAngle) * level);

          for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
            outputBuffer.addSample(i, startSample, currentSample);

          currentAngle += angleDelta;
          ++startSample;
        }
      }
    }
  }

  using SynthesiserVoice::renderNextBlock;

private:
  double currentAngle = 0.0, angleDelta = 0.0, level = 0.0, tailOff = 0.0;
};

//==============================================================================
// This is an audio source that streams the output of our demo synth.
struct SynthAudioSource final : public AudioSource {
  SynthAudioSource(MidiKeyboardState &keyState) : keyboardState(keyState) {
    for (auto i = 0; i < 4; ++i) {
      synth.addVoice(new SineWaveVoice());
      synth.addVoice(new SamplerVoice());
    }
    setUsingSineWaveSound();
  }

  void setUsingSineWaveSound() {
    synth.clearSounds();
    synth.addSound(new SineWaveSound());
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
  }

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
class AudioSynthesiserDemo final : public Component {
public:
  // Custom WebBrowserComponent to intercept URL calls
  class SphereSynthBrowser : public WebBrowserComponent {
  public:
    SphereSynthBrowser(AudioSynthesiserDemo &owner) : owner(owner) {}

    bool pageAboutToLoad(const String &newURL) override {
      if (newURL.startsWith("sphere://")) {
        owner.handleSphereCommand(newURL);
        return false; // Don't actually load the URL
      }
      return true;
    }

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
            <style>
              body { background-color: #121212; color: white; font-family: 'Futura', sans-serif; display: flex; flex-direction: column; align-items: center; justify-content: space-between; height: 100vh; margin: 0; overflow: hidden; user-select: none; -webkit-user-select: none; }
              h1 { color: #00e5ff; text-shadow: 0 0 10px #00e5ff; margin-top: 20px; letter-spacing: 2px; font-size: 24px; }
              .sphere-container { flex-grow: 1; display: flex; align-items: center; justify-content: center; width: 100%; }
              .sphere { width: 150px; height: 150px; border-radius: 50%; background: radial-gradient(circle at 30% 30%, #00e5ff, #000); box-shadow: 0 0 20px #00e5ff; transition: transform 0.1s; }
              .sphere.active { transform: scale(1.2); box-shadow: 0 0 50px #00e5ff, 0 0 100px #ff0055; background: radial-gradient(circle at 30% 30%, #fff, #00e5ff); }
              .controls { display: flex; gap: 20px; margin-bottom: 30px; }
              button { background: #2a2a2a; color: white; border: 1px solid #00e5ff; padding: 10px 20px; border-radius: 20px; font-size: 14px; cursor: pointer; transition: all 0.3s; outline: none; }
              button.active { background: #00e5ff; color: black; box-shadow: 0 0 15px #00e5ff; font-weight: bold; }
              .keyboard { display: flex; width: 95%; height: 140px; margin-bottom: 20px; background: #000; padding: 5px; border-radius: 5px; }
              .key { flex-grow: 1; background: white; border: 1px solid #333; border-radius: 0 0 5px 5px; position: relative; cursor: pointer; margin: 0 1px; }
              .key.black { background: #1a1a1a; height: 60%; width: 0; margin: 0 -12px; z-index: 10; border: 1px solid #555; }
              .key:active, .key.pressed { background: #00e5ff; box-shadow: 0 0 15px #00e5ff; }
              .key.black:active, .key.black.pressed { background: #ff0055; box-shadow: 0 0 15px #ff0055; }
            </style>
            </head>
            <body>
              <h1>SPHERE SYNTH</h1>
              <div class="sphere-container"><div class="sphere" id="sphere"></div></div>
              <div class="controls">
                <button id="btn-sine" class="active" onclick="setSound('sine')">SINE WAVE</button>
                <button id="btn-sampled" onclick="setSound('sampled')">SAMPLED CELLO</button>
              </div>
              <div class="keyboard" id="keyboard"></div>
              <script>
                const sphere = document.getElementById('sphere');
                let activeNotes = 0;
                
                function setSound(type) {
                  document.getElementById('btn-sine').className = type === 'sine' ? 'active' : '';
                  document.getElementById('btn-sampled').className = type === 'sampled' ? 'active' : '';
                  window.location = 'sphere://sound/' + type;
                }

                function noteOn(note) {
                  activeNotes++;
                  updateSphere();
                  window.location = 'sphere://noteOn/' + note + '/100';
                }

                function noteOff(note) {
                  activeNotes--;
                  updateSphere();
                  window.location = 'sphere://noteOff/' + note;
                }
                
                function updateSphere() {
                    if (activeNotes > 0) sphere.classList.add('active');
                    else sphere.classList.remove('active');
                }

                const keyboard = document.getElementById('keyboard');
                const startNote = 48; // C3
                const numKeys = 24;
                
                for (let i = 0; i < numKeys; i++) {
                    let note = startNote + i;
                    let isBlack = [1, 3, 6, 8, 10].includes(note % 12);
                    
                    let key = document.createElement('div');
                    key.className = isBlack ? 'key black' : 'key';
                    
                    const start = (e) => { 
                        if(e.type === 'touchstart') e.preventDefault();
                        if(!key.classList.contains('pressed')) {
                            key.classList.add('pressed'); 
                            noteOn(note); 
                        }
                    };
                    const end = (e) => { 
                        if(e.type === 'touchend') e.preventDefault();
                        if(key.classList.contains('pressed')) {
                            key.classList.remove('pressed'); 
                            noteOff(note); 
                        }
                    };

                    key.addEventListener('mousedown', start);
                    key.addEventListener('mouseup', end);
                    key.addEventListener('mouseleave', end);
                    key.addEventListener('touchstart', start);
                    key.addEventListener('touchend', end);

                    keyboard.appendChild(key);
                }
              </script>
            </body>
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

    setOpaque(true);
    setSize(800, 600);
  }

  ~AudioSynthesiserDemo() override {
    audioSourcePlayer.setSource(nullptr);
    audioDeviceManager.removeMidiInputDeviceCallback(
        {}, &(synthAudioSource.midiCollector));
    audioDeviceManager.removeAudioCallback(&callback);
  }

  void handleSphereCommand(const String &url) {
    std::cout << "IPC Received: " << url << std::endl;
    String cmd = url.substring(9); // remove sphere://
    StringArray parts;
    parts.addTokens(cmd, "/", "");

    if (parts[0] == "noteOn") {
      int note = parts[1].getIntValue();
      int velocity = parts[2].getIntValue();
      std::cout << "Note On: " << note << " Vel: " << velocity << std::endl;
      keyboardState.noteOn(1, note, (float)velocity / 127.0f);
    } else if (parts[0] == "noteOff") {
      int note = parts[1].getIntValue();
      std::cout << "Note Off: " << note << std::endl;
      keyboardState.noteOff(1, note, 0.0f);
    } else if (parts[0] == "sound") {
      std::cout << "Set Sound: " << parts[1] << std::endl;
      if (parts[1] == "sine")
        synthAudioSource.setUsingSineWaveSound();
      else if (parts[1] == "sampled")
        synthAudioSource.setUsingSampledSound();
    }
  }

  void paint(Graphics &g) override { g.fillAll(Colours::black); }

  void resized() override { webView.setBounds(getLocalBounds()); }

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
