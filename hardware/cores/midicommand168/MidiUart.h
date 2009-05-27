#ifndef MIDI_UART_H__
#define MIDI_UART_H__

#include <MidiUartParent.hh>
#include <inttypes.h>
#include "RingBuffer.h"

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

#endif /* MIDI_UART_H__ */
