#include "midi.h"

#include <CoreMIDI/MIDIServices.h>
#include <CoreFoundation/CFRunLoop.h>

#include <CoreMIDI/MIDIServices.h>
#include <CoreFoundation/CFRunLoop.h>

MIDIPortRef     gOutPort = NULL;
MIDIEndpointRef gDest = NULL;
int gChannel = 0;

#define MAX_TIMEOUT_CALLBACKS 3

int callbacks = 0;
/* XXX locks for callbacks */

void timerFunction(CFRunLoopTimerRef timer, void *info) {
}

void midiSysexSent(MIDISysexSendRequest *request) {
  free(request);
  canSendSysex = 1;
}

//static unsigned char sysexBuf[1024];

void midiSendLong(unsigned char *buf, unsigned long len) {
  struct MIDISysexSendRequest *sysexReq = malloc(sizeof(struct MIDISysexSendRequest) + len);

  sysexReq->destination = gDest;
  sysexReq->data = ((unsigned char *)sysexReq) + sizeof(struct MIDISysexSendRequest);
  //  printf("sysexReq: %p, data: %p\n", sysexReq, sysexReq->data);
  memcpy(sysexReq->data, buf, len);
  sysexReq->bytesToSend = len;
  sysexReq->complete = false;
  sysexReq->completionProc = midiSysexSent;
  sysexReq->completionRefCon = NULL;
  MIDISendSysex(sysexReq);
}

void midiSendShort(unsigned char status,
		   unsigned char byte1, unsigned char byte2) {
  static struct MIDIPacketList pktlist;
  pktlist.numPackets = 1;
  pktlist.packet[0].timeStamp = 0;
  pktlist.packet[0].length = 3;
  pktlist.packet[0].data[0] = status;
  pktlist.packet[0].data[1] = byte1;
  pktlist.packet[0].data[2] = byte2;
  MIDISend(gOutPort, gDest, &pktlist); 
}

void timer_callback(CFRunLoopTimerRef ref, void *info) {
  if (exitMainLoop)
    midiClose();
  if (++callbacks > MAX_TIMEOUT_CALLBACKS)
    midiTimeout();
}

void myReadProc(const MIDIPacketList *pktlist, void *refCon, void *connRefCon) {
  if (gOutPort != NULL && gDest != NULL) {
    MIDIPacket *packet = (MIDIPacket *)pktlist->packet;
    unsigned int j;
    int i;
    for (j = 0; j < pktlist->numPackets; j++) {
      callbacks = 0;
      for (i = 0; i < packet->length; i++) {
	midiReceive(packet->data[i]);
      }

      packet = MIDIPacketNext(packet);
    }
  }
}


void listInputMidiDevices(void) {
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
    
    printf("%d) %s - %s\n", i, name, manuf);
  }

}

void listOutputMidiDevices(void) {
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
    
    printf("%d) %s - %s\n", i, name, manuf);
  }  
}

void midiInitialize(char *inputDeviceStr, char *outputDeviceStr) {
  int inputDevice = atoi(inputDeviceStr);
  int outputDevice = atoi(outputDeviceStr);
  MIDIClientRef client = NULL;
  MIDIClientCreate(CFSTR("MIDI Send"), NULL, NULL, &client);
  MIDIOutputPortCreate(client, CFSTR("Output port"), &gOutPort);
  gDest = MIDIGetDestination(outputDevice);
  MIDIPortRef inPort = NULL;
  MIDIInputPortCreate(client, CFSTR("Input port"), myReadProc, NULL, &inPort);
  

  MIDIEndpointRef src = MIDIGetSource(inputDevice);
  MIDIPortConnectSource(inPort, src, NULL);
}

void midiMainLoop(void) {
  float interval = 1;
  CFRunLoopTimerRef timer =
    CFRunLoopTimerCreate(NULL, CFAbsoluteTimeGetCurrent(),
			 interval, 0, 0, timer_callback, NULL);
  
  CFRunLoopAddTimer(CFRunLoopGetCurrent(), timer, kCFRunLoopCommonModes);
  
  CFRunLoopRef runLoop;
  runLoop = CFRunLoopGetCurrent();

  CFRunLoopRun();
}

void midiClose(void) {
  // XXX add closing code here
  exit(0);
}
