#ifndef MIDI_UART_H__
#define MIDI_UART_H__

#include "WProgram.h"
#include <inttypes.h>
#include <MidiUartParent.hh>
#include "RingBuffer.h"

#define TX_IRQ 1

class MidiUartClass : public MidiUartParent {
  
 public:
  MidiUartClass();
  virtual void init(long speed = 31250);
  virtual void puts(uint8_t *data, uint8_t cnt);
  virtual void putc(uint8_t c);
  virtual void putc_immediate(uint8_t c);
  virtual bool avail();
  virtual uint8_t getc();

#ifdef TX_IRQ
  volatile RingBuffer<32> txRb;
#endif

  
};

extern MidiUartClass MidiUart;

extern "C" {
extern volatile uint16_t clock;
}

#endif /* MIDI_UART_H__ */
