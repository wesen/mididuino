#ifndef MIDI_UART_H__
#define MIDI_UART_H__

class MidiUartClass;

#include <inttypes.h>

#include <MidiUartParent.hh>
#include <RingBuffer.h>

#define RX_BUF_SIZE 64
#if (RX_BUF_SIZE >= 256)
#define RX_BUF_TYPE uint16_t
#else
#define RX_BUF_TYPE uint8_t
#endif

//#define TX_IRQ 1

#define TX_BUF_SIZE 32
#if (TX_BUF_SIZE >= 256)
#define TX_BUF_TYPE uint16_t
#else
#define TX_BUF_TYPE uint8_t
#endif

class MidiUartClass : public MidiUartParent {
 public:
 MidiUartClass() : MidiUartParent() {
  }

  virtual void putByte(uint8_t c);
  virtual void putByte_immediate(uint8_t c);
  virtual uint8_t getByte();
  virtual bool isAvailable();

  virtual void initSerial();
  void enable();
  void disable();

  volatile RingBuffer<RX_BUF_SIZE, RX_BUF_TYPE> rxRb;

#ifdef TX_IRQ
  volatile RingBuffer<TX_BUF_SIZE, TX_BUF_TYPE> txRb;
#endif
};

extern MidiUartClass MidiUart;

#endif /* MIDI_UART_H__ */
