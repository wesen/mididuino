/* Copyright (c) 2009 - http://ruinwesen.com/ */

#ifndef MIDI_H__
#define MIDI_H__

#include <stdlib.h>

#include <inttypes.h>

// #include "MidiSDS.hh"
#include "Vector.hh"
#include "Callback.hh"

class MidiUartParent;

extern "C" {
#include "midi-common.hh"
}

/**
 * \addtogroup Midi
 *
 * @{
 **/

/**
 * \addtogroup midi_class Midi Parser Class
 *
 * @{
 **/

#define MIDI_NOTE_OFF_CB    0
#define MIDI_NOTE_ON_CB     1
#define MIDI_AT_CB          2
#define MIDI_CC_CB          3
#define MIDI_PRG_CHG_CB     4
#define MIDI_CHAN_PRESS_CB  5
#define MIDI_PITCH_WHEEL_CB 6

/**
 * Structure to hold state-transitions for incoming MIDI messages.
 **/
typedef struct {
  uint8_t      midi_status;
  midi_state_t next_state;
} midi_parse_t;

class MidiSysexClass;

/**
 * Callback type for a MIDI message.
 **/
typedef void(MidiCallback::*midi_callback_ptr_t)(uint8_t *msg);
#ifdef HOST_MIDIDUINO
/**
 * Callbacks with message length on the host.
 **/
typedef void(MidiCallback::*midi_callback_ptr2_t)(uint8_t *msg, uint8_t len);
#endif

#include "MidiSysex.hh"

class MidiClass {
  /**
   * \addtogroup midi_class
   *
   * @{
   **/
	
 protected:
  midi_state_t in_state;
  uint8_t last_status;
  uint8_t running_status;
  uint8_t in_msg_len;
  uint8_t msg[3];

  MidiUartParent *uart;

  uint8_t callback;
  //  midi_callback_t callbacks[7];
  CallbackVector1<MidiCallback, 8, uint8_t *> midiCallbacks[7];
#ifdef HOST_MIDIDUINO
  CallbackVector2<MidiCallback, 8, uint8_t *, uint8_t> messageCallback;
#endif

 public:
  bool midiActive;
  MidiSysexClass midiSysex;
  uint8_t receiveChannel;

  /*
    uint8_t sysexBuf[SYSEX_BUF_SIZE];
  */
  uint8_t *sysexBuf;
  uint16_t sysexBufLen;

  MidiClass(MidiUartParent *_uart = NULL, uint8_t *_sysexBuf = NULL, uint16_t _sysexBufLen = 0);

  void init();
  void handleByte(uint8_t c);

  void removeAllCallbacks() {
    for (int i = 0; i < countof(midiCallbacks); i++) {
      midiCallbacks[i].removeAll();
    }
#ifdef HOST_MIDIDUINO
    messageCallback.removeAll();
#endif

  }

#ifdef HOST_MIDIDUINO
  void addOnMessageCallback(MidiCallback *obj,
                            MidiCallback::callback2_ptr_t func) {
    messageCallback.add(obj, func);
  }
  void removeOnMessageCallback(MidiCallback *obj,
                               MidiCallback::callback2_ptr_t func) {
    messageCallback.remove(obj, func);
  }
  void removeOnMessageCallback(MidiCallback *obj) {
    messageCallback.remove(obj);
  }
#endif

  void addOnControlChangeCallback(MidiCallback *obj, MidiCallback::callback_ptr_t func) {
    midiCallbacks[MIDI_CC_CB].add(obj, func);
  }
  void removeOnControlChangeCallback(MidiCallback *obj, MidiCallback::callback_ptr_t func) {
    midiCallbacks[MIDI_CC_CB].remove(obj, func);
  }
  void removeOnControlChangeCallback(MidiCallback *obj) {
    midiCallbacks[MIDI_CC_CB].remove(obj);
  }

  void addOnNoteOnCallback(MidiCallback *obj, MidiCallback::callback_ptr_t func) {
    midiCallbacks[MIDI_NOTE_ON_CB].add(obj, func);
  }
  void removeOnNoteOnCallback(MidiCallback *obj, MidiCallback::callback_ptr_t func) {
    midiCallbacks[MIDI_NOTE_ON_CB].remove(obj, func);
  }
  void removeOnNoteOnCallback(MidiCallback *obj) {
    midiCallbacks[MIDI_NOTE_ON_CB].remove(obj);
  }
  
  void addOnNoteOffCallback(MidiCallback *obj, MidiCallback::callback_ptr_t func) {
    midiCallbacks[MIDI_NOTE_OFF_CB].add(obj, func);
  }
  void removeOnNoteOffCallback(MidiCallback *obj, MidiCallback::callback_ptr_t func) {
    midiCallbacks[MIDI_NOTE_OFF_CB].remove(obj, func);
  }
  void removeOnNoteOffCallback(MidiCallback *obj) {
    midiCallbacks[MIDI_NOTE_OFF_CB].remove(obj);
  }
  
  void addOnAfterTouchCallback(MidiCallback *obj, MidiCallback::callback_ptr_t func) {
    midiCallbacks[MIDI_AT_CB].add(obj, func);
  }
  void removeOnAfterTouchCallback(MidiCallback *obj, MidiCallback::callback_ptr_t func) {
    midiCallbacks[MIDI_AT_CB].remove(obj, func);
  }
  void removeOnAfterTouchCallback(MidiCallback *obj) {
    midiCallbacks[MIDI_AT_CB].remove(obj);
  }
  
  void addOnProgramChangeCallback(MidiCallback *obj, MidiCallback::callback_ptr_t func) {
    midiCallbacks[MIDI_PRG_CHG_CB].add(obj, func);
  }
  void removeOnProgramChangeCallback(MidiCallback *obj, MidiCallback::callback_ptr_t func) {
    midiCallbacks[MIDI_PRG_CHG_CB].remove(obj, func);
  }
  void removeOnProgramChangeCallback(MidiCallback *obj) {
    midiCallbacks[MIDI_PRG_CHG_CB].remove(obj);
  }
  
  void addOnChannelPressureCallback(MidiCallback *obj, MidiCallback::callback_ptr_t func) {
    midiCallbacks[MIDI_CHAN_PRESS_CB].add(obj, func);
  }
  void removeOnChannelPressureCallback(MidiCallback *obj, MidiCallback::callback_ptr_t func) {
    midiCallbacks[MIDI_CHAN_PRESS_CB].remove(obj, func);
  }
  void removeOnChannelPressureCallback(MidiCallback *obj) {
    midiCallbacks[MIDI_CHAN_PRESS_CB].remove(obj);
  }
  
  void addOnPitchWheelCallback(MidiCallback *obj, MidiCallback::callback_ptr_t func) {
    midiCallbacks[MIDI_PITCH_WHEEL_CB].add(obj, func);
  }
  void removeOnPitchWheelCallback(MidiCallback *obj, MidiCallback::callback_ptr_t func) {
    midiCallbacks[MIDI_PITCH_WHEEL_CB].remove(obj, func);
  }
  void removeOnPitchWheelCallback(MidiCallback *obj) {
    midiCallbacks[MIDI_PITCH_WHEEL_CB].remove(obj);
  }

  /* @} */
  
};

#ifndef TEST_SUITE
extern MidiClass Midi;
extern MidiClass Midi2;
extern MidiClass USBMidi;
#else
#include "VirtualMidi.h"
class VirtualMidi;
extern VirtualMidi Midi;
extern VirtualMidi Midi2;
#endif

/* @} @} */

#include <MidiUartParent.hh>

#endif /* MIDI_H__ */
