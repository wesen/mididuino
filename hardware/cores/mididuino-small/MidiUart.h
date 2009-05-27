#ifndef MIDI_UART_H__
#define MIDI_UART_H__

#include <inttypes.h>
#include <MidiUartParent.hh>
#include "RingBuffer.h"
#include "helpers.h"

#define TX_IRQ 1

#define MAX_TIMED_NOTES 8

typedef struct note_timing_s {
  uint16_t last_clock;
  uint8_t note;
  uint16_t play_length;
  uint16_t length;
} note_timing_t;

class MidiUartClass : public MidiUartParent {
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


class USBMidiUartClass : public MidiUartParent {
 public:
 USBMidiUartClass() : MidiUartParent() {
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
