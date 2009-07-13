#ifndef MIDIUARTOSX_H__
#define MIDIUARTOSX_H__

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
  MidiUartOSXClass(int _inputDevice = -1, int _outputDevice = -1);
  void init(int _inputDevice, int _outputDevice);
  
  virtual void putc(uint8_t c);
  virtual void putc_immediate(uint8_t c) {
    putc(c);
  }
  virtual bool avail();
  virtual uint8_t getc();

  void runLoop();

  virtual void sendSysex(uint8_t *data, uint8_t cnt);
  virtual void puts(uint8_t *data, uint8_t cnt) {
    midiSendLong(data, cnt);
  }

  void midiSendLong(unsigned char *buf, unsigned long len);
  void midiSendShort(unsigned char status, unsigned char byte1, unsigned char byte2);
  
  static void listInputMidiDevices() {
    unsigned long   iNumDevs, i;

    /* Get the number of MIDI Out devices in this computer */
    iNumDevs = MIDIGetNumberOfSources();

    //  printf("%lu output midi devices found\r\n", iNumDevs);
  
    /* Go through all of those devices, displaying their names */
    for (i = 0; i < iNumDevs; i++) {
      CFStringRef pname, pmanuf, pmodel;
      char name[64], manuf[64], model[64];
    
      MIDIEndpointRef ep = MIDIGetSource(i);
      MIDIObjectGetStringProperty(ep, kMIDIPropertyName, &pname);
      MIDIObjectGetStringProperty(ep, kMIDIPropertyManufacturer, &pmanuf);
      MIDIObjectGetStringProperty(ep, kMIDIPropertyModel, &pmodel);
    
      CFStringGetCString(pname, name, sizeof(name), 0);
      CFStringGetCString(pmanuf, manuf, sizeof(manuf), 0);
      CFStringGetCString(pmodel, model, sizeof(model), 0);
      CFRelease(pname);
      CFRelease(pmanuf);
      CFRelease(pmodel);
    
      printf("%ld) %s - %s\n", i, name, manuf);
    }
  }
  static void listOutputMidiDevices() {
    unsigned long   iNumDevs, i;

    /* Get the number of MIDI Out devices in this computer */
    iNumDevs = MIDIGetNumberOfDestinations();

    //  printf("%lu output midi devices found\r\n", iNumDevs);
  
    /* Go through all of those devices, displaying their names */
    for (i = 0; i < iNumDevs; i++) {
      CFStringRef pname, pmanuf, pmodel;
      char name[64], manuf[64], model[64];
    
      MIDIEndpointRef ep = MIDIGetDestination(i);
      MIDIObjectGetStringProperty(ep, kMIDIPropertyName, &pname);
      MIDIObjectGetStringProperty(ep, kMIDIPropertyManufacturer, &pmanuf);
      MIDIObjectGetStringProperty(ep, kMIDIPropertyModel, &pmodel);
    
      CFStringGetCString(pname, name, sizeof(name), 0);
      CFStringGetCString(pmanuf, manuf, sizeof(manuf), 0);
      CFStringGetCString(pmodel, model, sizeof(model), 0);
      CFRelease(pname);
      CFRelease(pmanuf);
      CFRelease(pmodel);
    
      printf("%ld) %s - %s\n", i, name, manuf);
    }  
  }    
  
  volatile RingBuffer<RX_BUF_SIZE, RX_BUF_TYPE> rxRb;
  volatile RingBuffer<TX_BUF_SIZE> txRb;
};

#endif /* MIDIUARTOSX_H__ */
