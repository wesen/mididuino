/*
 * MidiCtrl - Virtual MIDI interface
 *
 * (c) June 2012 - Manuel Odendahl - wesen@ruinwesen.com
 */


#ifndef VIRTUAL_MIDI_H__
#define VIRTUAL_MIDI_H__

#include "MidiUartParent.hh"
#include "Midi.h"

class VirtualMidi : public MidiClass, public MidiUartParent {
public:
  VirtualMidi() {
    MidiClass();
    MidiUartParent();
  }

  virtual ~VirtualMidi() {
  }

  virtual void putc(uint8_t c);

#ifdef HOST_MIDIDUINO
  inline void addOnMessageCallback(MidiCallback *obj,
                                   MidiCallback::callback2_ptr_t func) {
    MidiClass::addOnMessageCallback(obj, func);
  }
  void removeOnMessageCallback(MidiCallback *obj,
                               MidiCallback::callback2_ptr_t func) {
    MidiClass::removeOnMessageCallback(obj, func);
  }
  void removeOnMessageCallback(MidiCallback *obj) {
    MidiClass::removeOnMessageCallback(obj);
  }
#endif

  void addOnControlChangeCallback(MidiCallback *obj, MidiCallback::callback_ptr_t func) {
    MidiClass::addOnControlChangeCallback(obj, func);
  }
  void removeOnControlChangeCallback(MidiCallback *obj, MidiCallback::callback_ptr_t func) {
    MidiClass::removeOnControlChangeCallback(obj, func);
  }
  void removeOnControlChangeCallback(MidiCallback *obj) {
    MidiClass::removeOnControlChangeCallback(obj);
  }

  void addOnNoteOnCallback(MidiCallback *obj, MidiCallback::callback_ptr_t func) {
    MidiClass::addOnNoteOnCallback(obj, func);
  }
  void removeOnNoteOnCallback(MidiCallback *obj, MidiCallback::callback_ptr_t func) {
    MidiClass::removeOnNoteOnCallback(obj, func);
  }
  void removeOnNoteOnCallback(MidiCallback *obj) {
    MidiClass::removeOnNoteOnCallback(obj);
  }

  void addOnNoteOffCallback(MidiCallback *obj, MidiCallback::callback_ptr_t func) {
    MidiClass::addOnNoteOffCallback(obj, func);

  }
  void removeOnNoteOffCallback(MidiCallback *obj, MidiCallback::callback_ptr_t func) {
    MidiClass::removeOnNoteOffCallback(obj, func);
  }
  void removeOnNoteOffCallback(MidiCallback *obj) {
    MidiClass::removeOnNoteOffCallback(obj);
  }

  void addOnAfterTouchCallback(MidiCallback *obj, MidiCallback::callback_ptr_t func) {
    MidiClass::addOnAfterTouchCallback(obj, func);
  }
  void removeOnAfterTouchCallback(MidiCallback *obj, MidiCallback::callback_ptr_t func) {
    MidiClass::removeOnAfterTouchCallback(obj, func);
  }
  void removeOnAfterTouchCallback(MidiCallback *obj) {
    MidiClass::removeOnAfterTouchCallback(obj);
  }

  void addOnProgramChangeCallback(MidiCallback *obj, MidiCallback::callback_ptr_t func) {
    MidiClass::addOnProgramChangeCallback(obj, func);
  }
  void removeOnProgramChangeCallback(MidiCallback *obj, MidiCallback::callback_ptr_t func) {
    MidiClass::removeOnProgramChangeCallback(obj, func);
  }
  void removeOnProgramChangeCallback(MidiCallback *obj) {
    MidiClass::removeOnProgramChangeCallback(obj);
  }

  void addOnChannelPressureCallback(MidiCallback *obj, MidiCallback::callback_ptr_t func) {
    MidiClass::addOnChannelPressureCallback(obj, func);
  }
  void removeOnChannelPressureCallback(MidiCallback *obj, MidiCallback::callback_ptr_t func) {
    MidiClass::removeOnChannelPressureCallback(obj, func);
  }
  void removeOnChannelPressureCallback(MidiCallback *obj) {
    MidiClass::removeOnChannelPressureCallback(obj);
  }

  void addOnPitchWheelCallback(MidiCallback *obj, MidiCallback::callback_ptr_t func) {
    MidiClass::addOnPitchWheelCallback(obj, func);
  }
  void removeOnPitchWheelCallback(MidiCallback *obj, MidiCallback::callback_ptr_t func) {
    MidiClass::removeOnPitchWheelCallback(obj, func);

  }
  void removeOnPitchWheelCallback(MidiCallback *obj) {
    MidiClass::removeOnPitchWheelCallback(obj);
  }

  void runLoop() {
    // do nothing
  }
};


#endif /* VIRTUAL_MIDI_H__ */
