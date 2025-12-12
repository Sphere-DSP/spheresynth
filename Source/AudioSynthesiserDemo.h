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
    SphereSynthBrowser(AudioSynthesiserDemo &owner)
        : WebBrowserComponent(
              WebBrowserComponent::Options()
                  .withBackend(WebBrowserComponent::Options::Backend::webview2)),
          owner(owner) {}

    bool pageAboutToLoad(const String &newURL) override;

  private:
    AudioSynthesiserDemo &owner;
  };

  AudioSynthesiserDemo() {
    addAndMakeVisible(webView);

    // HTML Content from resources
    String htmlContent = SphereSynthResources::html;

    File tempDir = File::getSpecialLocation(File::tempDirectory);
    File tempFile = tempDir.getChildFile("sphere_synth.html");
    tempFile.replaceWithText(htmlContent);
    
    // Copy video file to temp directory if it exists
    File exeFile = File::getSpecialLocation(File::currentExecutableFile);
    File videoSrc = exeFile.getParentDirectory().getChildFile("colorful-galaxy.1920x1080.mp4");
    if (videoSrc.existsAsFile()) {
      File videoDst = tempDir.getChildFile("colorful-galaxy.1920x1080.mp4");
      if (!videoDst.existsAsFile()) {
        videoSrc.copyFileTo(videoDst);
      }
    }
    
    webView.goToURL(URL(tempFile).toString(false));

    audioSourcePlayer.setSource(&synthAudioSource);

#ifndef JUCE_DEMO_RUNNER
    audioDeviceManager.initialise(0, 2, nullptr, true, {}, nullptr);
#endif

    audioDeviceManager.addAudioCallback(&callback);
    audioDeviceManager.addMidiInputDeviceCallback(
        {}, &(synthAudioSource.midiCollector));
    // NOTE: Removed duplicate MIDI callback registration that was causing double note processing
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
    
    // Update compressor gain reduction meter and waveform
    float gr = synthAudioSource.getCompressorGainReduction();
    webView.evaluateJavascript("updateCompMeter(" + String(gr) + ")");
    webView.evaluateJavascript("pushWaveformLevel(" + String(synthAudioSource.currentRMS.load()) + ")");
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
  } else if (parts[0] == "comp") {
    // Compressor commands: comp/enable/1, comp/threshold/-20, etc.
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
