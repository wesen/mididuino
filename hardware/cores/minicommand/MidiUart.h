#ifndef MIDI_UART_H__
#define MIDI_UART_H__

#include <inttypes.h>
#include <MidiUartParent.hh>
#include "RingBuffer.h"
#include "helpers.h"

#define TX_IRQ 1

class MidiUartClass : public MidiUartParent {
  void initSerial();
  
 public:
  MidiUartClass();
  void puts(uint8_t *data, uint8_t cnt);
  void putc(uint8_t c);
  void putc_immediate(uint8_t c);
  bool avail();
  uint8_t getc();

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
