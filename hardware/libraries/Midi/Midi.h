#ifndef MIDI_H__
#define MIDI_H__

#include <stdlib.h>

#include <WProgram.h>

#include <MidiUartParent.hh>
#include <inttypes.h>

// #include "MidiSDS.hh"
#include "MidiSysex.hh"
#include "Vector.hh"
#include "ListPool.hh"
#include "Callback.hh"

extern "C" {
#include "midi-common.hh"
}

#define MIDI_NOTE_OFF_CB    0
#define MIDI_NOTE_ON_CB     1
#define MIDI_AT_CB          2
#define MIDI_CC_CB          3
#define MIDI_PRG_CHG_CB     4
#define MIDI_CHAN_PRESS_CB  5
#define MIDI_PITCH_WHEEL_CB 6

typedef struct {
  uint8_t      midi_status;
  midi_state_t next_state;
  //  midi_callback_t callback;
} midi_parse_t;

class MidiSysexClass;

typedef void(MidiCallback::*midi_callback_ptr_t)(uint8_t *msg);

class MidiClass {
 private:
  midi_state_t in_state;
  uint8_t last_status;
  uint8_t running_status;
  uint8_t in_msg_len;
  uint8_t msg[3];

  MidiUartParent *uart;

  uint8_t callback;
  //  midi_callback_t callbacks[7];
  CallbackVector1<MidiCallback, 8, uint8_t *> midiCallbacks[7];

 public:
  bool midiActive;

  uint8_t receiveChannel;

  MidiClass(MidiUartParent *_uart = NULL);

  void init();
  void handleByte(uint8_t c);

  /*
  void addCallback(uint8_t num, midi_callback_t cb);
  void removeCallback(uint8_t num, midi_callback_t cb);
  void setCallback(uint8_t num, midi_callback_t cb);

  void addOnControlChangeCallback(midi_callback_t);
  void removeOnControlChangeCallback(midi_callback_t);
  void setOnControlChangeCallback(midi_callback_t);

  void addOnNoteOnCallback(midi_callback_t);
  void removeOnNoteOnCallback(midi_callback_t);
  void setOnNoteOnCallback(midi_callback_t);

  void addOnNoteOffCallback(midi_callback_t);
  void removeOnNoteOffCallback(midi_callback_t);
  void setOnNoteOffCallback(midi_callback_t);

  void addOnAfterTouchCallback(midi_callback_t);
  void removeOnAfterTouchCallback(midi_callback_t);
  void setOnAfterTouchCallback(midi_callback_t);

  void addOnProgramChangeCallback(midi_callback_t);
  void removeOnProgramChangeCallback(midi_callback_t);
  void setOnProgramChangeCallback(midi_callback_t);

  void addOnChannelPressureCallback(midi_callback_t);
  void removeOnChannelPressureCallback(midi_callback_t);
  void setOnChannelPressureCallback(midi_callback_t);

  void addOnPitchWheelCallback(midi_callback_t);
  void removeOnPitchWheelCallback(midi_callback_t);
  void setOnPitchWheelCallback(midi_callback_t);
  */

  void addOnControlChangeCallback(MidiCallback *obj, void(MidiCallback::*func)(uint8_t *msg)) {
    midiCallbacks[MIDI_CC_CB].add(obj, func);
  }
  void removeOnControlChangeCallback(MidiCallback *obj, void (MidiCallback::*func)(uint8_t *msg)) {
    midiCallbacks[MIDI_CC_CB].remove(obj, func);
  }
  void removeOnControlChangeCallback(MidiCallback *obj) {
    midiCallbacks[MIDI_CC_CB].remove(obj);
  }

  void addOnNoteOnCallback(MidiCallback *obj, void(MidiCallback::*func)(uint8_t *msg)) {
    midiCallbacks[MIDI_NOTE_ON_CB].add(obj, func);
  }
  void removeOnNoteOnCallback(MidiCallback *obj, void(MidiCallback::*func)(uint8_t *msg)) {
    midiCallbacks[MIDI_NOTE_ON_CB].remove(obj, func);
  }
  void removeOnNoteOnCallback(MidiCallback *obj) {
    midiCallbacks[MIDI_NOTE_ON_CB].remove(obj);
  }
  
  void addOnNoteOffCallback(MidiCallback *obj, void(MidiCallback::*func)(uint8_t *msg)) {
    midiCallbacks[MIDI_NOTE_OFF_CB].add(obj, func);
  }
  void removeOnNoteOffCallback(MidiCallback *obj, void(MidiCallback::*func)(uint8_t *msg)) {
    midiCallbacks[MIDI_NOTE_OFF_CB].remove(obj, func);
  }
  void removeOnNoteOffCallback(MidiCallback *obj) {
    midiCallbacks[MIDI_NOTE_OFF_CB].remove(obj);
  }
  
  void addOnAfterTouchCallback(MidiCallback *obj, void(MidiCallback::*func)(uint8_t *msg)) {
    midiCallbacks[MIDI_AT_CB].add(obj, func);
  }
  void removeOnAfterTouchCallback(MidiCallback *obj, void(MidiCallback::*func)(uint8_t *msg)) {
    midiCallbacks[MIDI_AT_CB].remove(obj, func);
  }
  void removeOnAfterTouchCallback(MidiCallback *obj) {
    midiCallbacks[MIDI_AT_CB].remove(obj);
  }
  
  void addOnProgramChangeCallback(MidiCallback *obj, void(MidiCallback::*func)(uint8_t *msg)) {
    midiCallbacks[MIDI_PRG_CHG_CB].add(obj, func);
  }
  void removeOnProgramChangeCallback(MidiCallback *obj, void(MidiCallback::*func)(uint8_t *msg)) {
    midiCallbacks[MIDI_PRG_CHG_CB].remove(obj, func);
  }
  void removeOnProgramChangeCallback(MidiCallback *obj) {
    midiCallbacks[MIDI_PRG_CHG_CB].remove(obj);
  }
  
  void addOnChannelPressureCallback(MidiCallback *obj, void(MidiCallback::*func)(uint8_t *msg)) {
    midiCallbacks[MIDI_CHAN_PRESS_CB].add(obj, func);
  }
  void removeOnChannelPressureCallback(MidiCallback *obj, void(MidiCallback::*func)(uint8_t *msg)) {
    midiCallbacks[MIDI_CHAN_PRESS_CB].remove(obj, func);
  }
  void removeOnChannelPressureCallback(MidiCallback *obj) {
    midiCallbacks[MIDI_CHAN_PRESS_CB].remove(obj);
  }
  
  void addOnPitchWheelCallback(MidiCallback *obj, void(MidiCallback::*func)(uint8_t *msg)) {
    midiCallbacks[MIDI_PITCH_WHEEL_CB].add(obj, func);
  }
  void removeOnPitchWheelCallback(MidiCallback *obj, void(MidiCallback::*func)(uint8_t *msg)) {
    midiCallbacks[MIDI_PITCH_WHEEL_CB].remove(obj, func);
  }
  void removeOnPitchWheelCallback(MidiCallback *obj) {
    midiCallbacks[MIDI_PITCH_WHEEL_CB].remove(obj);
  }
  
};

extern MidiClass Midi;
extern MidiClass Midi2;
extern MidiClass USBMidi;

#endif /* MIDI_H__ */
