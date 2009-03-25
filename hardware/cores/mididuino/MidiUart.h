#ifndef MIDI_UART_H__
#define MIDI_UART_H__

#include <inttypes.h>
#include "RingBuffer.h"
#include "MidiClock.h"
#include "helpers.h"

#define TX_IRQ 1

#define MAX_TIMED_NOTES 8

typedef struct note_timing_s {
  uint16_t last_clock;
  uint8_t note;
  uint16_t play_length;
  uint16_t length;
} note_timing_t;

class MidiUartParentClass {
  //  note_timing_t timed_notes[MAX_TIMED_NOTES];
  
 public:
  MidiUartParentClass() {
#if 0
    for (uint8_t i = 0; i < MAX_TIMED_NOTES; i++) {
      timed_notes[i].note = 0xFF;
      timed_notes[i].length = 0;
      timed_notes[i].play_length = 0;
      timed_notes[i].last_clock = 0;
    }
#endif
    currentChannel = 0x0;
  }

  virtual bool avail() {
    return false;
  }
  virtual uint8_t getc() {
    return 0;
  }
  virtual void putc(uint8_t c) {
  }

  void puts(uint8_t *data, uint8_t cnt);
  
  uint8_t currentChannel;
  
  void sendNoteOn(uint8_t channel, uint8_t note, uint8_t velocity);
  inline void sendNoteOn(uint8_t note, uint8_t velocity) {
    sendNoteOn(currentChannel, note, velocity);
  }
  void sendNoteOff(uint8_t channel, uint8_t note, uint8_t velocity);
  inline void sendNoteOff(uint8_t note, uint8_t velocity) {
    sendNoteOff(currentChannel, note, velocity);
  }
  inline void sendNoteOff(uint8_t note) {
    sendNoteOff(currentChannel, note, 0);
  }
  void sendCC(uint8_t channel, uint8_t cc, uint8_t value);
  inline void sendCC(uint8_t cc, uint8_t value) {
    sendCC(currentChannel, cc, value);
  }

  void sendProgramChange(uint8_t channel, uint8_t program);
  inline void sendProgramChange(uint8_t program) {
    sendProgramChange(currentChannel, program);
  }

  void sendPolyKeyPressure(uint8_t channel, uint8_t note, uint8_t pressure);
  inline void sendPolyKeyPressure(uint8_t note, uint8_t pressure) {
    sendPolyKeyPressure(currentChannel, note, pressure);
  }

  void sendChannelPressure(uint8_t channel, uint8_t pressure);
  inline void sendChannelPressure(uint8_t pressure) {
    sendChannelPressure(currentChannel, pressure);
  }

  void sendPitchBend(uint8_t channel, int16_t pitchbend);
  inline void sendPitchBend(int16_t pitchbend) {
    sendPitchBend(currentChannel, pitchbend);
  }

  void sendNRPN(uint8_t channel, uint16_t parameter, uint8_t value);
  inline void sendNRPN(uint16_t parameter, uint8_t value) {
    sendNRPN(currentChannel, parameter, value);
  }
  void sendNRPN(uint8_t channel, uint16_t parameter, uint16_t value);
  inline void sendNRPN(uint16_t parameter, uint16_t value) {
    sendNRPN(currentChannel, parameter, value);
  }

  void sendRPN(uint8_t channel, uint16_t parameter, uint8_t value);
  inline void sendRPN(uint16_t parameter, uint8_t value) {
    sendRPN(currentChannel, parameter, value);
  }
  void sendRPN(uint8_t channel, uint16_t parameter, uint16_t value);
  inline void sendRPN(uint16_t parameter, uint16_t value) {
    sendRPN(currentChannel, parameter, value);
  }

  inline void sendRaw(uint8_t *msg, uint8_t cnt) {
    puts(msg, cnt);
  }
  inline void sendRaw(uint8_t byte) {
    putc(byte);
  }
};

class MidiUartClass : public MidiUartParentClass {
  void initSerial();
  
 public:
  MidiUartClass();
  void putc_immediate(uint8_t c);
  virtual bool avail();
  virtual uint8_t getc();
  virtual void putc(uint8_t c);


  volatile RingBuffer<32> rxRb;

#ifdef TX_IRQ
  volatile RingBuffer<32> txRb;
#endif

};

extern MidiUartClass MidiUart;


class USBMidiUartClass : public MidiUartParentClass {
 public:
 USBMidiUartClass() : MidiUartParentClass() {
    currentChannel = 0x0;
    configured = 0;
  }

  uint8_t configured;

  virtual bool avail();
  virtual uint8_t getc();
  virtual void putc(uint8_t c);

  volatile RingBuffer<32> rxRb;
};

extern "C" {
  void usb_midi_uart_putc(uint8_t c);
}

extern USBMidiUartClass USBMidiUart;

#endif /* MIDI_UART_H__ */
