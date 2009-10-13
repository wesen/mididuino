#include "WProgram.h"

#include <midi-common.hh>
#include <MidiUartParent.hh>
#include "MidiUartOSX.h"

MidiUartOSXClass::MidiUartOSXClass(int _inputDevice, int _outputDevice) {
  inputDevice = -1;
  outputDevice = -1;
  outPort = NULL;
  dest = NULL;
  client = NULL;
  inPort = NULL;
  src = NULL;
  
  if ((_inputDevice >= 0) && (_outputDevice >= 0)) {
    init(_inputDevice, _outputDevice);
  }
}

void MidiUartOSXClass::sendSysex(uint8_t *data, uint8_t cnt) {
  midiSendLong(data, cnt);
}

void MidiUartOSXClass::putc(uint8_t c) {
  static struct MIDIPacketList pktlist;
  
  pktlist.numPackets = 1;
  pktlist.packet[0].timeStamp = 0;
  pktlist.packet[0].length = 1;
  pktlist.packet[0].data[0] = c;
  MIDISend(outPort, dest, &pktlist); 
}

bool MidiUartOSXClass::avail() {
  return !rxRb.isEmpty();
}

uint8_t MidiUartOSXClass::getc() {
  return rxRb.get();
}


static void midiReadProc(const MIDIPacketList *pktlist, void *refCon, void *connRefCon) {
  MidiUartOSXClass *uart = (MidiUartOSXClass *)refCon;
  if (uart != NULL) {
    if (uart->outPort != NULL && uart->dest != NULL) {
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

static void midiSysexSent(MIDISysexSendRequest *request) {
  printf("refcon : %p\n", request->completionRefCon);
  free(request);
}


void MidiUartOSXClass::init(int _inputDevice, int _outputDevice) {
  if ((inputDevice >= 0) || (outputDevice >= 0)) {
    fprintf(stderr, "MIDI ports already open\n");
    return;
  }

  inputDevice = _inputDevice;
  outputDevice = _outputDevice;
  MIDIClientCreate(CFSTR("MIDI Send"), NULL, NULL, &client);
  MIDIOutputPortCreate(client, CFSTR("Output port"), &outPort);
  dest = MIDIGetDestination(outputDevice);
  printf("connecting this: %p\n", this);
  MIDIInputPortCreate(client, CFSTR("Input port"), midiReadProc, this, &inPort);
  src = MIDIGetSource(inputDevice);
  MIDIPortConnectSource(inPort, src, NULL);
}

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

void MidiUartOSXClass::midiSendShort(unsigned char status, unsigned char byte1, unsigned char byte2) {
  static struct MIDIPacketList pktlist;
  
  pktlist.numPackets = 1;
  pktlist.packet[0].timeStamp = 0;
  pktlist.packet[0].length = 3;
  pktlist.packet[0].data[0] = status;
  pktlist.packet[0].data[1] = byte1;
  pktlist.packet[0].data[2] = byte2;
  MIDISend(outPort, dest, &pktlist); 
}

void MidiUartOSXClass::runLoop() {
  CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0, true);
}

