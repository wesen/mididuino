#ifndef MIDIUARTWIN_H__
#define MIDIUARTWIN_H__

//#ifdef WINDOWS

#ifdef MidiUartHostClass
#error cannot define more than one midi uart host class
#endif

#define MidiUartHostClass MidiUartWinClass

#include <windows.h>
#include <mmsystem.h>

#include <MidiUartParent.hh>
#include "RingBuffer.h"

#define RX_BUF_SIZE 2048
#if (RX_BUF_SIZE >= 256)
#define RX_BUF_TYPE uint16_t
#else
#define RX_BUF_TYPE uint8_t
#endif
#define TX_BUF_SIZE 128

#define MAKE_SHORT_MSG(status, byte1, byte2) \
  ((unsigned long)((status & 0xFF) | ((byte1 & 0xFF) << 8) | ((byte2 & 0xFF) << 16)))

#define SHORT_MSG_STATUS(s) ((unsigned char)(s & 0xF))
#define SHORT_MSG_BYTE1(s) ((unsigned char)((s >> 8) & 0xFF))
#define SHORT_MSG_BYTE2(s) ((unsigned char)((s >> 16) & 0xFF))

class MidiUartWinClass : public MidiUartParent {
 protected:
  HMIDIOUT outHandle;
  HMIDIIN  inHandle;

  int inputDevice;
  int outputDevice;

  static const unsigned long MAX_LONG_SIZE = 8192;
  CHAR inputBuffer[MAX_LONG_SIZE];
  MIDIHDR midiHdr;
  
 public:
  volatile RingBuffer<RX_BUF_SIZE, RX_BUF_TYPE> rxRb;
  volatile RingBuffer<TX_BUF_SIZE> txRb;

  MidiUartWinClass(int _inputDevice = -1, int _outputDevice = -1);
  ~MidiUartWinClass();

  static void listInputMidiDevices();
  static void listOutputMidiDevices();
  
  virtual void putc(uint8_t c);
  virtual void putc_immediate(uint8_t c) { putc(c); }
  virtual bool avail();
  virtual uint8_t getc();
  virtual void puts(uint8_t *data, uint16_t cnt) { midiSendLong(data, cnt); }

  virtual void sendSysex(uint8_t *data, uint8_t cnt);

  void init(int _inputDevice, int _outputDevice);
  void runLoop();
  
  void midiSendLong(unsigned char *buf, unsigned long len);
  void midiSendShort(unsigned char status, unsigned char byte1, unsigned char byte2);
};

//#endif /* WINDOWS */

#endif /* MIDIUARTWIN_H__ */

