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
#include "SphereSynthAudio.h"
#include "SphereSynthResources.h"
#include <JuceHeader.h>
#include <algorithm>
#include <atomic>
#include <iostream>

using namespace juce;

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

    // HTML Content from resources - use unique random filename every time
    String htmlContent = SphereSynthResources::html;

    // Generate truly unique filename with random component
    Random random;
    String uniqueId = String(Time::currentTimeMillis()) + "_" +
                      String(random.nextInt(999999));
    File tempFile = File::getSpecialLocation(File::tempDirectory)
                        .getChildFile("ss_" + uniqueId + ".html");

    // Clean up ALL old temp files first
    File tempDir = File::getSpecialLocation(File::tempDirectory);
    for (auto &file :
         tempDir.findChildFiles(File::findFiles, false, "ss_*.html")) {
      file.deleteFile();
    }
    for (auto &file : tempDir.findChildFiles(File::findFiles, false,
                                             "sphere_synth_*.html")) {
      file.deleteFile();
    }

    // Write and load fresh HTML
    tempFile.replaceWithText(htmlContent);

    // Use file:// URL with cache-busting query parameter
    String fileUrl =
        "file://" + tempFile.getFullPathName() + "?nocache=" + uniqueId;
    webView.goToURL(fileUrl);

    audioSourcePlayer.setSource(&synthAudioSource);

#ifndef JUCE_DEMO_RUNNER
    audioDeviceManager.initialise(0, 2, nullptr, true, {}, nullptr);
#endif

    audioDeviceManager.addAudioCallback(&callback);
    audioDeviceManager.addMidiInputDeviceCallback(
        {}, &(synthAudioSource.midiCollector));
    // NOTE: Removed duplicate MIDI callback registration that was causing
    // double note processing
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

    // Send Spectrum Data
    // We send data as a JSON object: { "input": [...], "output": [...] }
    // To optimize, we only send every Nth frame or use a lower resolution if
    // needed. For now, we send 1024 bins (half of 2048 FFT)

    // Input spectrum
    const auto &inputMags = synthAudioSource.getInputAnalyzer().getMagnitudes();
    String inputJson = "[";
    size_t inputSize = std::min(inputMags.size(), (size_t)1024);
    for (size_t i = 0; i < inputSize; i++) {
      inputJson += String(inputMags[i], 1);
      if (i < inputSize - 1)
        inputJson += ",";
    }
    inputJson += "]";

    // Output spectrum
    const auto &outputMags =
        synthAudioSource.getOutputAnalyzer().getMagnitudes();
    String outputJson = "[";
    size_t outputSize = std::min(outputMags.size(), (size_t)1024);
    for (size_t i = 0; i < outputSize; i++) {
      outputJson += String(outputMags[i], 1);
      if (i < outputSize - 1)
        outputJson += ",";
    }
    outputJson += "]";

    webView.evaluateJavascript("updateSpectrum(" + inputJson + ", " +
                               outputJson + ")");
    // Update compressor gain reduction meter and waveform
    float gr = synthAudioSource.getCompressorGainReduction();
    webView.evaluateJavascript("updateCompMeter(" + String(gr) + ")");
    webView.evaluateJavascript("pushWaveformLevel(" +
                               String(synthAudioSource.currentRMS.load()) +
                               ")");
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
  } else if (parts[0] == "eq") {
    // EQ Commands
    if (parts[1] == "enable") {
      bool enable = parts[2].getIntValue() != 0;
      synthAudioSource.setEQEnabled(enable);
    } else if (parts[1] == "band") {
      // Format: eq/band/index/active/type/freq/gain/q
      int bandIndex = parts[2].getIntValue();
      bool active = parts[3].getIntValue() != 0;
      String typeStr = parts[4];
      double freq = parts[5].getDoubleValue();
      double gain = parts[6].getDoubleValue();
      double q = parts[7].getDoubleValue();

      Sphere::EQBandParams params;
      params.bypass = !active;
      params.frequency = freq;
      params.gainDb = gain;
      params.q = q;

      // Parse type string
      if (typeStr == "bell")
        params.type = Sphere::EQFilterType::Bell;
      else if (typeStr == "lowshelf")
        params.type = Sphere::EQFilterType::LowShelf;
      else if (typeStr == "highshelf")
        params.type = Sphere::EQFilterType::HighShelf;
      else if (typeStr == "lowcut")
        params.type = Sphere::EQFilterType::LowCut;
      else if (typeStr == "highcut")
        params.type = Sphere::EQFilterType::HighCut;
      else if (typeStr == "notch")
        params.type = Sphere::EQFilterType::Notch;
      else if (typeStr == "bandpass")
        params.type = Sphere::EQFilterType::BandPass;
      else if (typeStr == "tilt")
        params.type = Sphere::EQFilterType::Tilt;
      else if (typeStr == "allpass")
        params.type = Sphere::EQFilterType::AllPass;

      synthAudioSource.setEQBandParameters(bandIndex, params);
    } else if (parts[1] == "phasemode") {
      // Format: eq/phasemode/mode
      String mode = parts[2];
      if (mode == "minimum") {
        synthAudioSource.setEQPhaseMode(Sphere::EQPhaseMode::MinimumPhase);
      } else if (mode == "natural") {
        synthAudioSource.setEQPhaseMode(Sphere::EQPhaseMode::NaturalPhase);
      } else if (mode == "linear") {
        synthAudioSource.setEQPhaseMode(Sphere::EQPhaseMode::LinearPhase);
      }
    } else if (parts[1] == "oversampling") {
      // Format: eq/oversampling/factor
      int factor = parts[2].getIntValue();
      if (factor == 0 || factor == 1) {
        synthAudioSource.setEQOversampleFactor(
            Sphere::SphereEQOversampler::Factor::None);
      } else if (factor == 2) {
        synthAudioSource.setEQOversampleFactor(
            Sphere::SphereEQOversampler::Factor::X2);
      } else if (factor == 4) {
        synthAudioSource.setEQOversampleFactor(
            Sphere::SphereEQOversampler::Factor::X4);
      }
    } else if (parts[1] == "firlength") {
      // Format: eq/firlength/length
      String length = parts[2];
      if (length == "short") {
        synthAudioSource.setEQLinearPhaseLength(
            Sphere::LinearPhaseLength::Short);
      } else if (length == "medium") {
        synthAudioSource.setEQLinearPhaseLength(
            Sphere::LinearPhaseLength::Medium);
      } else if (length == "long") {
        synthAudioSource.setEQLinearPhaseLength(
            Sphere::LinearPhaseLength::Long);
      }
    } else if (parts[1] == "dynamic") {
      // Format: eq/dynamic/bandIndex/mode/thresh/ratio/attack/release/knee
      int bandIndex = parts[2].getIntValue();
      String modeStr = parts[3];
      double thresh = parts[4].getDoubleValue();
      double ratio = parts[5].getDoubleValue();
      double attack = parts[6].getDoubleValue();
      double release = parts[7].getDoubleValue();
      double knee = parts[8].getDoubleValue();

      // Get current band params and update dynamic section
      auto params = synthAudioSource.getEQBandParameters(bandIndex);

      // Parse dynamic mode
      if (modeStr == "off") {
        params.dynamicMode = Sphere::EQDynamicMode::Off;
      } else if (modeStr == "compress") {
        params.dynamicMode = Sphere::EQDynamicMode::Compress;
      } else if (modeStr == "expand") {
        params.dynamicMode = Sphere::EQDynamicMode::Expand;
      } else if (modeStr == "gate") {
        params.dynamicMode = Sphere::EQDynamicMode::Gate;
      }

      params.dynamicThreshold = thresh;
      params.dynamicRatio = ratio;
      params.dynamicAttack = attack;
      params.dynamicRelease = release;
      params.dynamicKnee = knee;

      synthAudioSource.setEQBandParameters(bandIndex, params);
    } else if (parts[1] == "character") {
      // Format: eq/character/mode
      int modeIdx = parts[2].getIntValue();
      Sphere::EQCharacterMode mode = Sphere::EQCharacterMode::Clean;
      if (modeIdx == 1)
        mode = Sphere::EQCharacterMode::Subtle;
      else if (modeIdx == 2)
        mode = Sphere::EQCharacterMode::Warm;

      synthAudioSource.setEQCharacterMode(mode);
    }
  } else if (parts[0] == "comp") {
    if (parts[1] == "enable") {
      synthAudioSource.setCompressorEnabled(parts[2].getIntValue() != 0);
    } else if (parts[1] == "delta") {
      synthAudioSource.setCompressorDelta(parts[2].getIntValue() != 0);
    } else if (parts[1] == "threshold") {
      synthAudioSource.setCompressorThreshold(parts[2].getFloatValue());
    } else if (parts[1] == "ratio") {
      synthAudioSource.setCompressorRatio(parts[2].getFloatValue());
    } else if (parts[1] == "attack") {
      synthAudioSource.setCompressorAttack(parts[2].getFloatValue());
    } else if (parts[1] == "release") {
      synthAudioSource.setCompressorRelease(parts[2].getFloatValue());
    } else if (parts[1] == "makeup") {
      synthAudioSource.setCompressorMakeup(parts[2].getFloatValue());
    } else if (parts[1] == "knee") {
      synthAudioSource.setCompressorKnee(parts[2].getFloatValue());
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
