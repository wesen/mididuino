#ifndef MIDIUARTOSX_H__
#define MIDIUARTOSX_H__

#include <CoreFoundation/CoreFoundation.h>
#include <CoreFoundation/CFString.h>
#include <CoreFoundation/CFBase.h>
#include <CoreServices/../Frameworks/CarbonCore.framework/Headers/MacTypes.h>
#include <CoreMIDI/MIDIServices.h>
#include <CoreFoundation/CFRunLoop.h>

#include <MidiUartParent.hh>

#ifdef MidiUartHostClass
#error cannot define more than one midi uart host class
#endif

#define MidiUartHostClass MidiUartOSXClass

#include "MidiUartHost.h"

class MidiUartOSXClass : public MidiUartHostParent {
 public:
  MIDIPortRef outPort;
  MIDIEndpointRef dest;
  MIDIClientRef client;
  MIDIPortRef inPort;
  MIDIEndpointRef src;

 public:
  MidiUartOSXClass(int _inputDevice = -1, int _outputDevice = -1);
  MidiUartOSXClass(const char *inputDevice, const char *outputDevice);
  ~MidiUartOSXClass();

  static void listInputMidiDevices();
  static void listOutputMidiDevices();
  static int getInputMidiDevice(const char *name);
  static int getOutputMidiDevice(const char *name);
  
  bool init(int _inputDevice, int _outputDevice);
  bool init(const char *_inputDeviceName, const char *_outputDeviceName);
  void runLoop();
  void midiSendLong(unsigned char *buf, unsigned long len);
  void midiSendShort(unsigned char status, unsigned char byte1);
  void midiSendShort(unsigned char status, unsigned char byte1, unsigned char byte2);
};

#ifndef TEST_SUITE
extern MidiUartOSXClass MidiUart;
#endif

#endif /* MIDIUARTOSX_H__ */
