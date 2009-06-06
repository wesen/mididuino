#ifndef MIDI_UART_H__
#define MIDI_UART_H__

class MidiUartClass;

#include <inttypes.h>
#include <MidiUartParent.hh>
#include "RingBuffer.h"

#define TX_IRQ 1

class MidiUartClass : public MidiUartParent {
  virtual void initSerial();
  
 public:
  MidiUartClass();
  virtual void putc(uint8_t c);
  virtual void putc_immediate(uint8_t c);
  virtual bool avail();
  virtual uint8_t getc();

  volatile RingBuffer<32> rxRb;

#ifdef TX_IRQ
  volatile RingBuffer<32> txRb;
#endif

};

extern MidiUartClass MidiUart;

class MidiUartClass2 : MidiUartClass {
  virtual void initSerial();
  
 public:
  MidiUartClass2();
  virtual bool avail();
  virtual uint8_t getc();
  volatile RingBuffer<32> rxRb;
};

extern MidiUartClass2 MidiUart2;

#endif /* MIDI_UART_H__ */
