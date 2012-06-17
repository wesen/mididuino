#ifndef MIDIUART_HOST_H__
#define MIDIUART_HOST_H__

#include "Platform.h"
#include "Midi.h"
#include "MidiSysex.hh"

#include "RingBuffer.h"
#include "MidiUartParent.hh"

#define RX_BUF_SIZE 2048
#if (RX_BUF_SIZE >= 256)
#define RX_BUF_TYPE uint16_t
#else
#define RX_BUF_TYPE uint8_t
#endif
#define TX_BUF_SIZE 128

class MidiUartHostParent;

class MidiUartHostSysexListener : public MidiSysexListenerClass {
protected:
  MidiUartHostParent *uart;

public:
  MidiUartHostSysexListener(MidiUartHostParent *_uart) : MidiSysexListenerClass() {
    uart = _uart;
    ids[0] = 0xFF;
  }

  virtual void end();
};

class MidiClass;

class MidiUartHostParent : public MidiUartParent, public MidiCallback {
protected:
  uint8_t _sysexBuf[8192];
  MidiClass outputMidi;
  MidiUartHostSysexListener sysexListener;

  void onOutputMessage(uint8_t *msg, uint8_t len);
  
public:
  int inputDevice;
  int outputDevice;
  volatile RingBuffer<RX_BUF_SIZE, RX_BUF_TYPE> rxRb;

  MidiUartHostParent(int _inputDevice = -1, int _outputDevice = -1) :
    outputMidi(NULL, _sysexBuf, sizeof(_sysexBuf)),
    sysexListener(this) {
    init(_inputDevice, _outputDevice);
  }
  
  virtual void putc(uint8_t c) { outputMidi.handleByte(c); }
  virtual bool avail() { return !rxRb.isEmpty(); }
  virtual uint8_t getc() { return rxRb.get(); }

  virtual bool init(int _inputDevice, int _outputDevice);
  virtual void runLoop() = 0;
  virtual void midiSendLong(unsigned char *buf, unsigned long len) = 0;
  virtual void midiSendShort(unsigned char status, unsigned char byte1) = 0;
  virtual void midiSendShort(unsigned char status, unsigned char byte1, unsigned char byte2) = 0;
};

#ifdef __APPLE__
#include "MidiUartOSX.h"
#endif

#ifdef __CYGWIN__
#include "MidiUartWin.h"
#endif

class MidiUartHostClass;
#ifndef TEST_SUITE
extern MidiUartHostClass MidiUart;
#endif

#endif /* MIDIUART_HOST_H__ */
