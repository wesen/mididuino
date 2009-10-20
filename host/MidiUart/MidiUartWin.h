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

#define MAKE_SHORT_MSG(status, byte1, byte2) \
  ((unsigned long)((status & 0xFF) | ((byte1 & 0xFF) << 8) | ((byte2 & 0xFF) << 16)))

#define SHORT_MSG_STATUS(s) ((unsigned char)(s & 0xF))
#define SHORT_MSG_BYTE1(s) ((unsigned char)((s >> 8) & 0xFF))
#define SHORT_MSG_BYTE2(s) ((unsigned char)((s >> 16) & 0xFF))

class MidiUartWinClass : public MidiUartHostParent {
 protected:
  HMIDIOUT outHandle;
  HMIDIIN  inHandle;

  static const unsigned long MAX_LONG_SIZE = 8192;
  CHAR inputBuffer[MAX_LONG_SIZE];
  MIDIHDR midiHdr;

 public:
  MidiUartWinClass(int _inputDevice = -1, int _outputDevice = -1);
  ~MidiUartWinClass();

  static void listInputMidiDevices();
  static void listOutputMidiDevices();

  void init(int _inputDevice, int _outputDevice);
  void runLoop();
  void midiSendLong(unsigned char *buf, unsigned long len);
  void midiSendShort(unsigned char status, unsigned char byte1, unsigned char byte2);
};

//#endif /* WINDOWS */

#endif /* MIDIUARTWIN_H__ */

