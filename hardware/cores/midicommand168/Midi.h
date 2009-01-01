#ifndef MIDI_H__
#define MIDI_H__

#include <stdlib.h>
#include "MidiUart.h"

#include "WProgram.h"
#include <inttypes.h>

extern "C" {
#include "midi-common.hh"
}

typedef enum {
  midi_wait_status = 0,
  midi_wait_byte_2,
  midi_wait_byte_1,
  midi_wait_sysex,
  midi_ignore_message
} midi_state_t;

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

  uint8_t callback;
  midi_callback_t callbacks[7];

  MidiSysexClass *sysex;
  
 public:
  uint8_t receiveChannel;

  MidiClass();
  void init();
  void handleByte(uint8_t c);
  void setOnControlChangeCallback(midi_callback_t);
  void setOnNoteOnCallback(midi_callback_t);
  void setOnNoteOffCallback(midi_callback_t);
  void setOnAfterTouchCallback(midi_callback_t);
  void setOnProgramChangeCallback(midi_callback_t);
  void setOnChannelPressureCallback(midi_callback_t);
  void setOnPitchWheelCallback(midi_callback_t);
  void setSysex(MidiSysexClass *s) {
    sysex = s;
  }
};

class MidiSysexClass {
 protected:
  uint8_t *data;
  uint16_t max_len;
  uint16_t len;
  bool aborted;

 public:
  MidiSysexClass(uint8_t *_data, uint16_t size) {
    data = _data;
    max_len = size;
    len = 0;
    aborted = false;
  }

  virtual void start();
  virtual void abort();
  virtual void end() {
  }
  virtual void handleByte(uint8_t byte);
};

class MididuinoSysexClass : public MidiSysexClass {
 public:
 MididuinoSysexClass(uint8_t *data, uint16_t size) :
  MidiSysexClass(data, size) {
  }

  virtual void end();
};

#define DECLARE_SYSEX \
  uint8_t sysex_data[64];					\
  MididuinoSysexClass sysex(sysex_data, sizeof(sysex_data));
    

extern MidiClass Midi;

#endif /* MIDI_H__ */
