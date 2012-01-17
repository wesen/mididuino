/**
 * Midi Merger for MacOSX
 *
 * (c) 2008 - Manuel Odendahl <wesen@ruinwesen.com>
 */

#include <unistd.h>
#include <stdio.h>
#include <CoreMIDI/MIDIServices.h>
#include <CoreFoundation/CFRunLoop.h>

#include <CoreMIDI/MIDIServices.h>
#include <CoreFoundation/CFRunLoop.h>
#include <stdio.h>

#define MIN(a, b) ((a > b) ? b : a)

MIDIPortRef     gOutPort = (MIDIPortRef)0;
MIDIEndpointRef gDest = (MIDIEndpointRef)0;
MIDIPortRef     gOutPort2 = (MIDIPortRef)0;
MIDIEndpointRef gDest2 = (MIDIEndpointRef)0;
int gChannel = 0;

FILE *fin;
int canSendSysex = 1;

/**
 * Create a new packet list, copy data from packet into it, and send it out.
 *
 * @param packet the actual packet data
 * @param outport output port
 * @param dest destination endpoint
 */
void midiSendPacket(MIDIPacket *packet, MIDIPortRef outport, MIDIEndpointRef dest) {
  struct MIDIPacketList pktlist;
  pktlist.numPackets = 1;
  pktlist.packet[0].timeStamp = 0;
  pktlist.packet[0].length = packet->length;
  int i;
  for (i = 0; i < packet->length; i++) {
    pktlist.packet[0].data[i] = packet->data[i];
  }
  MIDISend(outport, dest, &pktlist); 
}

void myReadProc(const MIDIPacketList *pktlist, void *refCon, void *connRefCon) {
  if (gOutPort != 0 && gDest != 0) {
    MIDIPacket *packet = (MIDIPacket *)pktlist->packet;
    unsigned int j;
    int i;
    for (j = 0; j < pktlist->numPackets; j++) {
      midiSendPacket(packet, gOutPort, gDest);
      packet = MIDIPacketNext(packet);
    }
  }
}

void myReadProc2(const MIDIPacketList *pktlist, void *refCon, void *connRefCon) {
  if (gOutPort2 != 0 && gDest2 != 0) {
    MIDIPacket *packet = (MIDIPacket *)pktlist->packet;
    unsigned int j;
    int i;
    for (j = 0; j < pktlist->numPackets; j++) {
      midiSendPacket(packet, gOutPort2, gDest2);
      packet = MIDIPacketNext(packet);
    }
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
    MIDIEntityRef ent;
    MIDIDeviceRef dev;
    MIDIEndpointGetEntity(ep, &ent);
    MIDIEntityGetDevice(ent, &dev);
    MIDIObjectGetStringProperty(ep, kMIDIPropertyName, &pname);
    MIDIObjectGetStringProperty(ep, kMIDIPropertyManufacturer, &pmanuf);
    MIDIObjectGetStringProperty(dev, kMIDIPropertyName, &pmodel);

    CFStringGetCString(pname, name, sizeof(name), 0);
    CFStringGetCString(pmanuf, manuf, sizeof(manuf), 0);
    CFStringGetCString(pmodel, model, sizeof(model), 0);
    CFRelease(pname);
    CFRelease(pmanuf);
    CFRelease(pmodel);
    
    printf("%lu) %s - %s - %s\n", i, name, manuf, model);
  }  
}

void usage(void) {
  fprintf(stderr, "Usage: ./midi-merge [-l] [-h] port1 port2\n");
}

int main(int argc, char *argv[]) {
  int c;
  int outputDevice = -1;
  int inputDevice = -1;

  while ((c = getopt(argc, argv, "hlb")) != -1) {
    switch (c) {
    case 'l':
      listOutputMidiDevices();
      exit(0);
      break;

    case 'h':
    default:
      usage();
      exit(0);
      break;
    }
  }

  if ((optind + 2) != argc) {
    usage();
    exit(1);
  }
  outputDevice = atoi(argv[optind]);
  inputDevice = atoi(argv[optind+1]);

  if (outputDevice == -1 || inputDevice == -1) {
    usage();
    exit(1);
  }

  MIDIClientRef client = 0;
  MIDIClientCreate(CFSTR("MIDI Send"), NULL, NULL, &client);
  MIDIOutputPortCreate(client, CFSTR("Output port"), &gOutPort);
  MIDIOutputPortCreate(client, CFSTR("Output port"), &gOutPort2);
  gDest = MIDIGetDestination(outputDevice);
  gDest2 = MIDIGetDestination(inputDevice);
  MIDIPortRef inPort = 0;
  MIDIInputPortCreate(client, CFSTR("Input port"), myReadProc, NULL, &inPort);
  MIDIPortRef inPort2 = 0;
  MIDIInputPortCreate(client, CFSTR("Input port"), myReadProc2, NULL, &inPort2);

  MIDIEndpointRef src = MIDIGetSource(inputDevice);
  MIDIPortConnectSource(inPort, src, NULL);
  src = MIDIGetSource(outputDevice);
  MIDIPortConnectSource(inPort2, src, NULL);

  CFRunLoopRef runLoop;
  runLoop = CFRunLoopGetCurrent();

  CFRunLoopRun();

  return 0;
}
