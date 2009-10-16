#ifndef MIDIUARTOSX_H__
#define MIDIUARTOSX_H__

#ifdef apple

#include <CoreMIDI/MIDIServices.h>
#include <CoreFoundation/CFRunLoop.h>

#include <MidiUartParent.hh>
#include "RingBuffer.h"

#define RX_BUF_SIZE 2048
#if (RX_BUF_SIZE >= 256)
#define RX_BUF_TYPE uint16_t
#else
#define RX_BUF_TYPE uint8_t
#endif
#define TX_BUF_SIZE 128

class MidiUartOSXClass : public MidiUartParent {
 public:
  MIDIPortRef outPort;
  MIDIEndpointRef dest;
  MIDIClientRef client;
  MIDIPortRef inPort;
  MIDIEndpointRef src;

  int inputDevice, outputDevice;

 public:
  volatile RingBuffer<RX_BUF_SIZE, RX_BUF_TYPE> rxRb;
  volatile RingBuffer<TX_BUF_SIZE> txRb;

  MidiUartOSXClass(int _inputDevice = -1, int _outputDevice = -1);
  ~MidiUartOSXClass();

  static void listInputMidiDevices();
  static void listOutputMidiDevices();  
  
  virtual void putc(uint8_t c);
  virtual void putc_immediate(uint8_t c) { putc(c); }
  virtual void puts(uint8_t *data, uint8_t cnt) { midiSendLong(data, cnt); }

  virtual bool avail();
  virtual uint8_t getc();

  void init(int _inputDevice, int _outputDevice);

  void runLoop();

  virtual void sendSysex(uint8_t *data, uint16_t cnt);
  void midiSendLong(unsigned char *buf, unsigned long len);
  void midiSendShort(unsigned char status, unsigned char byte1, unsigned char byte2);
};

#ifdef MidiUartHostClass
#error cannot define more than one midi uart host class
#endif

#define MidiUartHostClass MidiUartOSXClass

extern MidiUartOSXClass MidiUart;

#endif /* apple */

#endif /* MIDIUARTOSX_H__ */
