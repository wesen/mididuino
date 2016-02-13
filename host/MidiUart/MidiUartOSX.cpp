/*
 * MidiCtrl - MacOSX implementation of MidiUart
 *
 * (c) 2009 - 2011 - Manuel Odendahl - wesen@ruinwesen.com
 */

#ifdef __APPLE__

#include "Platform.h"

#include "midi-common.hh"
#include "MidiUartParent.hh"
#include "MidiUartOSX.h"

/***************************************************************************
 *
 * Helper methods
 *
 ***************************************************************************/

/**
 * Get the name of a midi endpoint.
 **/
void getMidiName(MIDIEndpointRef ep, char *buf, uint16_t len) {
  CFStringRef pname, pmanuf, pmodel;
  char name[64], manuf[64], model[64];
    
  MIDIObjectGetStringProperty(ep, kMIDIPropertyName, &pname);
  MIDIObjectGetStringProperty(ep, kMIDIPropertyManufacturer, &pmanuf);
  MIDIObjectGetStringProperty(ep, kMIDIPropertyModel, &pmodel);

  if (pname == NULL || pmanuf == NULL || pmodel == NULL) {
    snprintf(buf, len, "No such device");
    return;
  }
    
  CFStringGetCString(pname, name, sizeof(name), 0);
  CFStringGetCString(pmanuf, manuf, sizeof(manuf), 0);
  CFStringGetCString(pmodel, model, sizeof(model), 0);
  CFRelease(pname);
  CFRelease(pmanuf);
  CFRelease(pmodel);

  snprintf(buf, len, "%s - %s", name, manuf);
}

/**
 * List the available midi input devices.
 **/
void MidiUartOSXClass::listInputMidiDevices() {
  unsigned long   iNumDevs, i;

  iNumDevs = MIDIGetNumberOfSources();

  if (iNumDevs == 0) {
    printf("No input MIDI devices found\n");
    return;
  }

  for (i = 0; i < iNumDevs; i++) {
    char buf[128];
    MIDIEndpointRef ep = MIDIGetSource(i);
    getMidiName(ep, buf, countof(buf));
    printf("%ld) %s\n", i, buf);
  }
}

/**
 * List the available midi output devices.
 **/
void MidiUartOSXClass::listOutputMidiDevices() {
  unsigned long   iNumDevs, i;

  iNumDevs = MIDIGetNumberOfDestinations();

  if (iNumDevs == 0) {
    printf("No output MIDI devices found\n");
    return;
  }

  for (i = 0; i < iNumDevs; i++) {
    char buf[128];
    MIDIEndpointRef ep = MIDIGetDestination(i);
    getMidiName(ep, buf, countof(buf));
    printf("%ld) %s\n", i, buf);
  }  
}

/**
 * Returns the endpoint number of the first midi input device matching
 * the given name.
 **/
int MidiUartOSXClass::getInputMidiDevice(const char *name) {
  unsigned long   iNumDevs, i;

  iNumDevs = MIDIGetNumberOfSources();

  for (i = 0; i < iNumDevs; i++) {
    char buf[128];
    MIDIEndpointRef ep = MIDIGetSource(i);
    getMidiName(ep, buf, countof(buf));
    if (strcasestr(buf, name) != NULL) {
      return i;
    }
  }

  return -1;
}

/**
 * Returns the endpoint number of the midi output device matching the
 * given name.
 **/
int MidiUartOSXClass::getOutputMidiDevice(const char *name) {
  unsigned long   iNumDevs, i;

  iNumDevs = MIDIGetNumberOfDestinations();

  for (i = 0; i < iNumDevs; i++) {
    char buf[128];
    MIDIEndpointRef ep = MIDIGetDestination(i);
    getMidiName(ep, buf, countof(buf));
    if (strcasestr(buf, name) != NULL) {
      return i;
    }
  }  

  return -1;
}

/***************************************************************************
 *
 * Callbacks
 *
 ***************************************************************************/

/**
 * Read callback for receiving MIDI data.
 *
 * Puts the received data into the receive buffer of the uart for
 * processing in the main loop.
 **/
static void midiReadProc(const MIDIPacketList *pktlist, void *refCon, void *connRefCon) {
  MidiUartOSXClass *uart = (MidiUartOSXClass *)refCon;
  if (uart != NULL) {
    if ((uart->outPort != 0) && (uart->dest != 0)) {
      MIDIPacket *packet = (MIDIPacket *)pktlist->packet;
      unsigned int j;
      int i;
      for (j = 0; j < pktlist->numPackets; j++) {
        for (i = 0; i < packet->length; i++) {
          uart->rxRb.put(packet->data[i]);
        }

    packet = MIDIPacketNext(packet);
      }
    }
  }
}

/**
 * Callback when a sysex message has been sent, frees the request buffer.
 **/
static void midiSysexSent(MIDISysexSendRequest *request) {
  // XXX Debug
  //  printf("refcon : %p\n", request->completionRefCon);
  free(request);
}

/***************************************************************************
 *
 * MIDI initialization
 *
 ***************************************************************************/

MidiUartOSXClass::MidiUartOSXClass(int _inputDevice, int _outputDevice) {
  inputDevice = -1;
  outputDevice = -1;
  outPort = 0;
  dest = 0;
  client = 0;
  inPort = 0;
  src = 0;
  
  if ((_inputDevice >= 0) && (_outputDevice >= 0)) {
    init(_inputDevice, _outputDevice);
  }
}

MidiUartOSXClass::MidiUartOSXClass(const char *_inputDevice, const char *_outputDevice) {
  outPort = 0;
  dest = 0;
  client = 0;
  inPort = 0;
  src = 0;
  
  if (_inputDevice && _outputDevice) {
    init(_inputDevice, _outputDevice);
  }
}

bool MidiUartOSXClass::init(const char *_inputDeviceName, const char *_outputDeviceName) {
  int _inputDevice = MidiUartOSXClass::getInputMidiDevice(_inputDeviceName);
  int _outputDevice = MidiUartOSXClass::getOutputMidiDevice(_outputDeviceName);

  if (_inputDevice == -1 || _outputDevice == -1) {
    fprintf(stderr, "No such MIDI device: %s, %s\n", _inputDeviceName, _outputDeviceName);
    return false;
  }

  return init(_inputDevice, _outputDevice);
}

bool MidiUartOSXClass::init(int _inputDevice, int _outputDevice) {
  if ((inputDevice >= 0) || (outputDevice >= 0)) {
    fprintf(stderr, "MIDI ports already open\n");
    return false;
  }

  MidiUartHostParent::init(_inputDevice, _outputDevice);

  inputDevice = _inputDevice;
  outputDevice = _outputDevice;
  MIDIClientCreate(CFSTR("MIDI Send"), NULL, NULL, &client);
  MIDIOutputPortCreate(client, CFSTR("Output port"), &outPort);
  char buf[128];
  dest = MIDIGetDestination(outputDevice);
  getMidiName(dest, buf, countof(buf));
  printf("opening output device  \"%s\"\n", buf);
  MIDIInputPortCreate(client, CFSTR("Input port"), midiReadProc, this, &inPort);
  src = MIDIGetSource(inputDevice);
  getMidiName(src, buf, countof(buf));
  printf("opening input device:  \"%s\"\n", buf);
  MIDIPortConnectSource(inPort, src, NULL);

  return true;
}

MidiUartOSXClass::~MidiUartOSXClass() {
  if (outPort != 0) {
    MIDIPortDispose(outPort);
    outPort = 0;
  }

  if (inPort != 0) {
    MIDIPortDispose(inPort);
    inPort = 0;
  }

  if (client != 0) {
    MIDIClientDispose(client);
    client = 0;
  }


}

/**
 * Send a long midi message.
 *
 * Allocate a midi sysex send request buffer (which is later on freed by the send completion callback.
 **/
void MidiUartOSXClass::midiSendLong(unsigned char *buf, unsigned long len) {
  struct MIDISysexSendRequest *sysexReq = (struct MIDISysexSendRequest *)malloc(sizeof(struct MIDISysexSendRequest) + len);

  sysexReq->destination = dest;
  sysexReq->data = ((unsigned char *)sysexReq) + sizeof(struct MIDISysexSendRequest);
  memcpy((void *)sysexReq->data, buf, len);
  sysexReq->bytesToSend = len;
  sysexReq->complete = false;
  sysexReq->completionProc = midiSysexSent;
  sysexReq->completionRefCon = this;
  MIDISendSysex(sysexReq);
}

void MidiUartOSXClass::midiSendShort(unsigned char status, unsigned char byte1) {
  /* XXX is the static here correct at all !? */
  static struct MIDIPacketList pktlist;

  pktlist.numPackets = 1;
  pktlist.packet[0].timeStamp = 0;
  pktlist.packet[0].length = 2;
  pktlist.packet[0].data[0] = status;
  pktlist.packet[0].data[1] = byte1;
  MIDISend(outPort, dest, &pktlist);
}

/**
 * Send a short midi message.
 **/
void MidiUartOSXClass::midiSendShort(unsigned char status,
                                     unsigned char byte1,
                                     unsigned char byte2) {
  /* XXX is the static here correct at all !? */
  static struct MIDIPacketList pktlist;
  
  pktlist.numPackets = 1;
  pktlist.packet[0].timeStamp = 0;
  pktlist.packet[0].length = 3;
  pktlist.packet[0].data[0] = status;
  pktlist.packet[0].data[1] = byte1;
  pktlist.packet[0].data[2] = byte2;
  MIDISend(outPort, dest, &pktlist); 
}

/**
 * Run the Carbon run loop.
 **/
void MidiUartOSXClass::runLoop() {
  CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0, true);
}

#endif /* __APPLE__ */
