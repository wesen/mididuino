#ifndef MIDI_H__
#define MIDI_H__

#include <stdlib.h>

#include <WProgram.h>
#include <inttypes.h>

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

  MidiSysexClass *sysex;
  
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
  void setSysex(MidiSysexClass *s) {
    sysex = s;
  }
};

class MidiSysexClass {
 protected:
  bool aborted;
  bool recording;

 public:
  uint16_t max_len;
  uint16_t recordLen;
  uint8_t *data;

  uint16_t len;

  MidiSysexClass(uint8_t *_data, uint16_t size) {
    data = _data;
    max_len = size;
    len = 0;
    aborted = false;
    recording = false;
  }

  void startRecord();
  void stopRecord();
  virtual void start();
  virtual void abort();
  virtual void end() {
  }
  virtual void handleByte(uint8_t byte);
};

class MididuinoSysexClass : public MidiSysexClass {
  bool isMididuinoSysex;
 public:
 MididuinoSysexClass(uint8_t *data, uint16_t size) :
  MidiSysexClass(data, size) {
    isMididuinoSysex = true;
  }

  virtual void start();
  virtual void handleByte(uint8_t byte);
};

extern uint8_t mididuino_sysex_data[128];

extern MidiClass Midi;
extern MidiClass Midi2;
extern MidiClass USBMidi;

#endif /* MIDI_H__ */
