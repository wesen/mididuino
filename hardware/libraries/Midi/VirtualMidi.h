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
};


#endif /* VIRTUAL_MIDI_H__ */
