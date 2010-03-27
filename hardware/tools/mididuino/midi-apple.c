#include "midi.h"
#include "logging.h"

#include <CoreMIDI/MIDIServices.h>
#include <CoreFoundation/CFRunLoop.h>

#include <CoreMIDI/MIDIServices.h>
#include <CoreFoundation/CFRunLoop.h>

MIDIPortRef     gOutPort = NULL;
MIDIEndpointRef gDest = NULL;
int gChannel = 0;

extern int verbose;
void hexdump(unsigned char *buf, int len);

#define MAX_TIMEOUT_CALLBACKS 3

int callbacks = 0;
/* XXX locks for callbacks */

void midiSysexSent(MIDISysexSendRequest *request) {
  free(request);
  canSendSysex = 1;
}

//static unsigned char sysexBuf[1024];

void midiSendLong(unsigned char *buf, unsigned long len) {
  struct MIDISysexSendRequest *sysexReq = malloc(sizeof(struct MIDISysexSendRequest) + len);
	OSStatus ret;

  sysexReq->destination = gDest;
  sysexReq->data = ((unsigned char *)sysexReq) + sizeof(struct MIDISysexSendRequest);
	debugPrintf(1, "sending MIDI\n");
	debugHexdump(1, buf, len);

  //  printf("sysexReq: %p, data: %p\n", sysexReq, sysexReq->data);
  memcpy((void *)sysexReq->data, buf, len);
  sysexReq->bytesToSend = len;
  sysexReq->complete = false;
  sysexReq->completionProc = midiSysexSent;
  sysexReq->completionRefCon = NULL;
  ret = MIDISendSysex(sysexReq);
	//	printf("ret: %ld\n", ret);
}

void midiReceivedAckCallback(uint8_t *ptr) {
	callbacks = 0;
}

void midiSendShort(unsigned char status,
		   unsigned char byte1, unsigned char byte2) {
  static struct MIDIPacketList pktlist;
	OSStatus ret;
  pktlist.numPackets = 1;
  pktlist.packet[0].timeStamp = 0;
  pktlist.packet[0].length = 3;
  pktlist.packet[0].data[0] = status;
  pktlist.packet[0].data[1] = byte1;
  pktlist.packet[0].data[2] = byte2;
  ret = MIDISend(gOutPort, gDest, &pktlist);
	//	printf("send ret: %ld\n", ret);
}

void timer_callback(CFRunLoopTimerRef ref, void *info) {
  if (exitMainLoop)
    midiClose();
	//	printf("callbacks: %d\n", callbacks);
  if (++callbacks > MAX_TIMEOUT_CALLBACKS)
    midiTimeout();
}

void myReadProc(const MIDIPacketList *pktlist, void *refCon, void *connRefCon) {
  if (gOutPort != NULL && gDest != NULL) {
    MIDIPacket *packet = (MIDIPacket *)pktlist->packet;
    unsigned int j;
    int i;
    for (j = 0; j < pktlist->numPackets; j++) {
      for (i = 0; i < packet->length; i++) {
				midiReceive(packet->data[i]);
      }

      packet = MIDIPacketNext(packet);
    }
  }
}

bool midiPrintEndpoint(char *buf, uint16_t len, MIDIEndpointRef ep) {
	CFStringRef pname, pmanuf, pmodel;
	char name[64], manuf[64], model[64];
	SInt32 offline;
	OSStatus ret;

	ret = MIDIObjectGetIntegerProperty(ep, kMIDIPropertyOffline, &offline);
	if (ret != noErr) {
		printf("error getting offline property\n");
		return false;
	}
	if (offline == 1) {
		printf("device offline\n");
		return false;
	}
	ret = MIDIObjectGetStringProperty(ep, kMIDIPropertyName, &pname);
	if (ret != noErr) {
		printf("error getting name property\n");
		return false;
	}
	ret = MIDIObjectGetStringProperty(ep, kMIDIPropertyManufacturer, &pmanuf);
	if (ret != noErr) {
		printf("error getting manufacturer property\n");
		return false;
	}
	ret = MIDIObjectGetStringProperty(ep, kMIDIPropertyModel, &pmodel);
	if (ret != noErr) {
		printf("error getting model property\n");
		return false;
	}
  
	CFStringGetCString(pname, name, sizeof(name), 0);
	CFStringGetCString(pmanuf, manuf, sizeof(manuf), 0);
	CFStringGetCString(pmodel, model, sizeof(model), 0);
	CFRelease(pname);
	CFRelease(pmanuf);
	CFRelease(pmodel);

	snprintf(buf, len, "%s - %s", name, manuf);
	return true;
}

void listInputMidiDevices(void) {
  unsigned long   iNumDevs, i;

  /* Get the number of MIDI Out devices in this computer */
  iNumDevs = MIDIGetNumberOfSources();

  /* Go through all of those devices, displaying their names */
  for (i = 0; i < iNumDevs; i++) {
		char buf[128];
		if (midiPrintEndpoint(buf, sizeof(buf), MIDIGetSource(i))) {
			printf("%ld) %s\n", i, buf);
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
		char buf[128];
		if (midiPrintEndpoint(buf, sizeof(buf), MIDIGetDestination(i))) {
			printf("%ld) %s\n", i, buf);
		}
  }  
}

void midiInitialize(char *inputDeviceStr, char *outputDeviceStr) {
  int inputDevice = atoi(inputDeviceStr);
  int outputDevice = atoi(outputDeviceStr);
	OSStatus ret;
  MIDIClientRef client = NULL;
  ret = MIDIClientCreate(CFSTR("MIDI Send"), NULL, NULL, &client);
	if (ret != noErr) {
		fprintf(stderr, "Could not create MIDI client\n");
		exit(1);
	}
  ret = MIDIOutputPortCreate(client, CFSTR("Output port"), &gOutPort);
	if (ret != noErr) {
		fprintf(stderr, "Could not create MIDI output port\n");
		exit(1);
	}
  gDest = MIDIGetDestination(outputDevice);
	if (gDest == NULL) {
		fprintf(stderr, "Could not open MIDI destination\n");
		exit(1);
	}
	
  MIDIPortRef inPort = NULL;
  ret = MIDIInputPortCreate(client, CFSTR("Input port"), myReadProc, NULL, &inPort);
	if (ret != noErr) {
		fprintf(stderr, "Could not create MIDI input port\n");
		exit(1);
	}

  MIDIEndpointRef src = MIDIGetSource(inputDevice);
  ret = MIDIPortConnectSource(inPort, src, NULL);
	if (ret != noErr) {
		fprintf(stderr, "Could not connect to midi source\n");
		exit(1);
	}
}

void midiMainLoop(void) {
  float interval = 1;
  CFRunLoopTimerRef timer =
    CFRunLoopTimerCreate(NULL, CFAbsoluteTimeGetCurrent(),
			 interval, 0, 0, timer_callback, NULL);

	midi_ack_callback = midiReceivedAckCallback;
  
  CFRunLoopAddTimer(CFRunLoopGetCurrent(), timer, kCFRunLoopCommonModes);
  
  CFRunLoopRef runLoop;
  runLoop = CFRunLoopGetCurrent();

  CFRunLoopRun();
}

void midiClose(void) {
  // XXX add closing code here
  exit(0);
}
