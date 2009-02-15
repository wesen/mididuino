#ifndef MIDI_UART_H__
#define MIDI_UART_H__

#include <inttypes.h>
#include "MidiClock.h"
#include "RingBuffer.h"
#include "helpers.h"

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

  inline void sendRaw(uint8_t *msg, uint8_t cnt) {
    puts(msg, cnt);
  }
  inline void sendRaw(uint8_t byte) {
    putc(byte);
  }

  volatile RingBuffer<32> rxRb;

#ifdef TX_IRQ
  volatile RingBuffer<32> txRb;
#endif

};

extern MidiUartClass MidiUart;

class MidiUartClass2 {
  void initSerial();
  
 public:
  MidiUartClass2();
  bool avail();
  uint8_t getc();
  volatile RingBuffer<32> rxRb;
};

extern MidiUartClass2 MidiUart2;

#endif /* MIDI_UART_H__ */
