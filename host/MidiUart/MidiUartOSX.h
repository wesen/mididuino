#ifndef MIDIUARTOSX_H__
#define MIDIUARTOSX_H__

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
  ~MidiUartOSXClass() {
		// XXX
	}

  static void listInputMidiDevices();
  static void listOutputMidiDevices();  
  
  void init(int _inputDevice, int _outputDevice);
  void runLoop();
  void midiSendLong(unsigned char *buf, unsigned long len);
  void midiSendShort(unsigned char status, unsigned char byte1, unsigned char byte2);
};

extern MidiUartOSXClass MidiUart;

#endif /* MIDIUARTOSX_H__ */
