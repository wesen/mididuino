#ifndef MIDI_UART_H__
#define MIDI_UART_H__

#include "WProgram.h"
#include <inttypes.h>

#define TX_IRQ 1

class MidiUartClass {
  void initSerial();
  
 public:
  MidiUartClass();
  void puts(uint8_t *data, uint8_t cnt);
  void putc(uint8_t c);
  void putc_immediate(uint8_t c);
  bool avail();
  uint8_t getc();

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

extern MidiUartClass MidiUart;

#endif /* MIDI_UART_H__ */
