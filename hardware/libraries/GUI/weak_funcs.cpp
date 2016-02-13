/*
 * MidiCtrl - Weak functions for standard Midi-Ctrl functions
 *
 * This is used so that the user doesn't have to flesh out all
 * necessary functions in his sketch, but can override them at will.
 *
 * (c) 2009-2011 - Manuel Odendahl - wesen@ruinwesen.com
 */

#include "Platform.h"
#include "Midi.h"
#include <GUI.h>

void __attribute__((weak)) setup() {
}

void __attribute__((weak)) loop() {
}

void __attribute__((weak)) onNoteOn(uint8_t *msg) {
}

void __attribute__((weak)) onNoteOff(uint8_t *msg) {
}

void __attribute__((weak)) onControlChange(uint8_t *msg) {
}

void __attribute__((weak)) onProgramChange(uint8_t *msg) {
}

void __attribute__((weak)) onChannelPressure(uint8_t *msg) {
}

void __attribute__((weak)) onAfterTouch(uint8_t *msg) {
}

void __attribute__((weak)) onPitchWheel(uint8_t *msg) {
}

/* MIDI 2 Callbacks */
void __attribute__((weak)) onNoteOn2(uint8_t *msg) {
}

void __attribute__((weak)) onNoteOff2(uint8_t *msg) {
}

void __attribute__((weak)) onControlChange2(uint8_t *msg) {
}

void __attribute__((weak)) onProgramChange2(uint8_t *msg) {
}

void __attribute__((weak)) onChannelPressure2(uint8_t *msg) {
}

void __attribute__((weak)) onAfterTouch2(uint8_t *msg) {
}

void __attribute__((weak)) onPitchWheel2(uint8_t *msg) {
}


void __attribute__((weak)) on16Callback(uint32_t pos) {
}

void __attribute__((weak)) on32Callback(uint32_t pos) {
}

bool __attribute__((weak)) handleEvent(gui_event_t *evt) {
  return false;
}

class DefaultCallbacks : public MidiCallback, public ClockCallback {
public:

  /* callbacks on first midi interface */
  void onNoteOn(uint8_t *msg) {
    ::onNoteOn(msg);
  }
  void onNoteOff(uint8_t *msg) {
    ::onNoteOff(msg);
  }
  void onControlChange(uint8_t *msg) {
    ::onControlChange(msg);
  }
  void onProgramChange(uint8_t *msg) {
    ::onProgramChange(msg);
  }
  void onAfterTouch(uint8_t *msg) {
    ::onAfterTouch(msg);
  }
  void onChannelPressure(uint8_t *msg) {
    ::onChannelPressure(msg);
  }
  void onPitchWheel(uint8_t *msg) {
    ::onPitchWheel(msg);
  }

  /* callbacks on second midi interface */
  void onNoteOn2(uint8_t *msg) {
    ::onNoteOn2(msg);
  }
  void onNoteOff2(uint8_t *msg) {
    ::onNoteOff2(msg);
  }
  void onControlChange2(uint8_t *msg) {
    ::onControlChange2(msg);
  }
  void onProgramChange2(uint8_t *msg) {
    ::onProgramChange2(msg);
  }
  void onAfterTouch2(uint8_t *msg) {
    ::onAfterTouch2(msg);
  }
  void onChannelPressure2(uint8_t *msg) {
    ::onChannelPressure2(msg);
  }
  void onPitchWheel2(uint8_t *msg) {
    ::onPitchWheel2(msg);
  }

  /* midi clock callbacks */
  void on16Callback(uint32_t pos) {
    ::on16Callback(pos);
  }
  void on32Callback(uint32_t pos) {
    ::on32Callback(pos);
  }

  void setupMidiCallbacks() {
    Midi.addOnNoteOnCallback(this, (midi_callback_ptr_t)&DefaultCallbacks::onNoteOn);
    Midi.addOnNoteOffCallback(this, (midi_callback_ptr_t)&DefaultCallbacks::onNoteOff);
    Midi.addOnControlChangeCallback(this, (midi_callback_ptr_t)&DefaultCallbacks::onControlChange);
    Midi.addOnProgramChangeCallback(this, (midi_callback_ptr_t)&DefaultCallbacks::onProgramChange);
    Midi.addOnAfterTouchCallback(this, (midi_callback_ptr_t)&DefaultCallbacks::onAfterTouch);
    Midi.addOnChannelPressureCallback(this, (midi_callback_ptr_t)&DefaultCallbacks::onChannelPressure);
    Midi.addOnPitchWheelCallback(this, (midi_callback_ptr_t)&DefaultCallbacks::onPitchWheel);

    Midi2.addOnNoteOnCallback(this, (midi_callback_ptr_t)&DefaultCallbacks::onNoteOn2);
    Midi2.addOnNoteOffCallback(this, (midi_callback_ptr_t)&DefaultCallbacks::onNoteOff2);
    Midi2.addOnControlChangeCallback(this, (midi_callback_ptr_t)&DefaultCallbacks::onControlChange2);
    Midi2.addOnProgramChangeCallback(this, (midi_callback_ptr_t)&DefaultCallbacks::onProgramChange2);
    Midi2.addOnAfterTouchCallback(this, (midi_callback_ptr_t)&DefaultCallbacks::onAfterTouch2);
    Midi2.addOnChannelPressureCallback(this, (midi_callback_ptr_t)&DefaultCallbacks::onChannelPressure2);
    Midi2.addOnPitchWheelCallback(this, (midi_callback_ptr_t)&DefaultCallbacks::onPitchWheel2);
  }

  void setupClockCallbacks() {
    MidiClock.addOn16Callback(this, (midi_clock_callback_ptr_t)&DefaultCallbacks::on16Callback);
    MidiClock.addOn32Callback(this, (midi_clock_callback_ptr_t)&DefaultCallbacks::on32Callback);
  }

};

DefaultCallbacks defaultCallbacks;

void __attribute__((weak)) setupEventHandlers() {
  GUI.addEventHandler(&handleEvent);
}

void __attribute__((weak)) setupMidiCallbacks() {
  defaultCallbacks.setupMidiCallbacks();
}

void __attribute__((weak)) setupClockCallbacks() {
  defaultCallbacks.setupClockCallbacks();
}
