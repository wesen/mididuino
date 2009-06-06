#ifndef MIDI_H__
#define MIDI_H__

#include <stdlib.h>

#include <WProgram.h>
#include <MidiUart.h>
#include <inttypes.h>

#include "MidiSDS.hh"
#include "MidiSysex.hh"

extern "C" {
#include "midi-common.hh"
}

typedef struct {
  uint8_t      midi_status;
  midi_state_t next_state;
  midi_callback_t callback;
} midi_parse_t;

class MidiSysexClass;

class MidiClass {
 private:
  midi_state_t in_state;
  uint8_t last_status;
  uint8_t running_status;
  uint8_t in_msg_len;
  uint8_t msg[3];
  MidiUartClass *uart;

  uint8_t callback;
  midi_callback_t callbacks[7];

 public:
  uint8_t receiveChannel;

  MidiClass(MidiUartClass *_uart = NULL);
  void init();
  void handleByte(uint8_t c);
  void setOnControlChangeCallback(midi_callback_t);
  void setOnNoteOnCallback(midi_callback_t);
  void setOnNoteOffCallback(midi_callback_t);
  void setOnAfterTouchCallback(midi_callback_t);
  void setOnProgramChangeCallback(midi_callback_t);
  void setOnChannelPressureCallback(midi_callback_t);
  void setOnPitchWheelCallback(midi_callback_t);
};

extern MidiClass Midi;
extern MidiClass Midi2;
extern MidiClass USBMidi;

#endif /* MIDI_H__ */
