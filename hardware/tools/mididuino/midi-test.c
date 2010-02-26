/**# Midi port tester (windows).
 **
 ** This program opens an input midi port and an output midi
 ** port. Both have to be connected together, so that the output port
 ** sends to the input port.
 **
 **## Usage:
 **
 ** ./midi-test -l shows all available midi devices
 **
 ** ./midi-test -i 1 -o 1 opens the midi input device 1 and the midi
 ** output device 1, and then carries on testing that data sent on
 ** input device 1 is received on input device 2.
 **/

#include <unistd.h>
#include <stdio.h>
#include <windows.h>
#include <mmsystem.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define MAKE_SHORT_MSG(status, byte1, byte2) \
       ((unsigned long)((status & 0xFF) | ((byte1 & 0xFF) << 8) | ((byte2 & 0xFF) << 16)))

#define SHORT_MSG_STATUS(s) ((unsigned char)(s & 0xFF))
#define SHORT_MSG_BYTE1(s) ((unsigned char)((s >> 8) & 0xFF))
#define SHORT_MSG_BYTE2(s) ((unsigned char)((s >> 16) & 0xFF))


void midiSendShort(HMIDIOUT handle, unsigned char status,
		   unsigned char byte1, unsigned char byte2) {
  midiOutShortMsg(handle, MAKE_SHORT_MSG(status, byte1, byte2));
}

void midiSendLong(HMIDIOUT handle, unsigned char *buf, unsigned long len) {
  MIDIHDR midiHdr;

  midiHdr.lpData = (LPBYTE)buf;
  midiHdr.dwBufferLength = len;
  midiHdr.dwFlags = 0;
  
  UINT err = midiOutPrepareHeader(handle, &midiHdr, sizeof(MIDIHDR));
  if (!err) {
    //    memcpy(midiHdr.lpData, buf, len);
    err = midiOutLongMsg(handle, &midiHdr, sizeof(MIDIHDR));
    if (err) {
      char errBuf[256];
      midiOutGetErrorText(err, errBuf, sizeof(errBuf));
      printf("error sending long message: %s\n", errBuf);
      
    }
    while (MIDIERR_STILLPLAYING == midiOutUnprepareHeader(handle, &midiHdr, sizeof(MIDIHDR)))
      ;
  }
}

typedef enum {
  MIDI_NONE_EVENT,
  MIDI_DATA_RECEIVED,
  MIDI_LONGDATA_RECEIVED,
  MIDI_ERROR,
  MIDI_LONGERROR
} midi_event_e;

#define MAX_LONG_SIZE 8192

typedef struct midi_event_s {
  midi_event_e midiEvent;
  unsigned long midiTimestamp; // = 0;
  
  unsigned long midiShortMsg; // = 0;
  
  unsigned char midiLongMsgData[MAX_LONG_SIZE];
  unsigned long midiLongMsgLen; //  = 0;
  
  unsigned long midiErrorMsg; // = 0;
  unsigned char midiErrorMsgData[MAX_LONG_SIZE];
  unsigned char midiErrorMsgLen; // = 0;
} midi_event_t;

#define EVENT_RB_SIZE 32
typedef struct midi_event_rb_s {
  midi_event_t events[EVENT_RB_SIZE];
  unsigned int rd, len;
} midi_event_rb_t;

static midi_event_rb_t event_rb;
static CRITICAL_SECTION rb_cs;

void event_rb_init(void) {
  event_rb.rd = 0;
  event_rb.len = 0;
  InitializeCriticalSection(&rb_cs);
}

int event_rb_empty(void) {
  return (event_rb.len == 0);
}

int event_rb_full(void) {
  return (event_rb.len >= EVENT_RB_SIZE);
}

midi_event_t *event_rb_new_event(void) {
  if (event_rb_full())
    return NULL;

  // XXX locking
  EnterCriticalSection(&rb_cs);
  unsigned long idx = (event_rb.rd + event_rb.len) % EVENT_RB_SIZE;
  event_rb.len++;
  midi_event_t *res = event_rb.events + idx;
  LeaveCriticalSection(&rb_cs);
  return res;
}

int event_rb_read_event(midi_event_t *evt) {
  if (event_rb_empty())
    return 0;

  EnterCriticalSection(&rb_cs);
  midi_event_t *res = event_rb.events + event_rb.rd;
  memcpy(evt, res, sizeof(*evt));
  event_rb.rd = (event_rb.rd + 1) % EVENT_RB_SIZE;
  event_rb.len--;
  LeaveCriticalSection(&rb_cs);
  return 1;
}

/* flag so that no new buffers are prepared. */
static int midiCloseApp = 0;

void CALLBACK midiCallback(HMIDIIN handle, UINT uMsg, DWORD dwInstance,
			   DWORD dwParam1, DWORD dwParam2) {
  //  printf("event: %x on handle %lx\n", uMsg, (unsigned long)handle);
  midi_event_t *evt = event_rb_new_event();
  if (evt == NULL) {
    printf("Could not get new event structure\n");
    return;
  }
  evt->midiEvent = MIDI_NONE_EVENT;
  
  switch (uMsg) {
  case MIM_DATA:
    evt->midiEvent = MIDI_DATA_RECEIVED;
    evt->midiShortMsg = dwParam1;
    evt->midiTimestamp = dwParam2;
    break;

  case MIM_LONGDATA:
    {
      printf("midid long\n");
      if (!midiCloseApp) {
	evt->midiEvent = MIDI_LONGDATA_RECEIVED;
	MIDIHDR *midiHdr = (MIDIHDR *)dwParam1;
	int len = 0;
	for (len = 0; len < midiHdr->dwBufferLength; len++)
	  if (((unsigned char)midiHdr->lpData[len]) == 0xF7)
	    break;
	printf("buffer length %d, msg len %d\n", midiHdr->dwBufferLength, len);
	len++;
	evt->midiLongMsgLen = len;
	int i;
	for (i = 0; i < len; i++) {
	  evt->midiLongMsgData[i] = (unsigned char)midiHdr->lpData[i];
	  printf("%2.0x ", (unsigned char)midiHdr->lpData[i]);
	}
	printf("\n");
	memcpy(evt->midiLongMsgData, midiHdr->lpData, len);
	evt->midiTimestamp = dwParam2;
	midiInAddBuffer(handle, midiHdr, sizeof(*midiHdr));
      } else {
	evt->midiEvent = MIDI_NONE_EVENT;
      }
    }
    break;

  case MIM_MOREDATA:
      // XXX set IO_STATUS flag to midiInOpen()
    printf("midi driver is throwing away received data: %x %x %x\n",
	   SHORT_MSG_STATUS(dwParam1), SHORT_MSG_BYTE1(dwParam1),
	   SHORT_MSG_BYTE2(dwParam1));
    break;
    
  case MIM_ERROR:
    evt->midiEvent = MIDI_ERROR;
    evt->midiErrorMsg = dwParam1;
    evt->midiTimestamp = dwParam2;
    break;
    
  case MIM_LONGERROR:
    {
      evt->midiEvent = MIDI_LONGERROR;
      MIDIHDR *midiHdr = (MIDIHDR *)dwParam1;
      evt->midiErrorMsgLen = MIN(midiHdr->dwBufferLength, sizeof(evt->midiErrorMsgData));
      memcpy(midiHdr->lpData, evt->midiErrorMsgData, evt->midiErrorMsgLen);
      evt->midiTimestamp = dwParam2;
    }
    break;

  default:
    printf("received event %x\n", uMsg);
    evt->midiEvent = MIDI_NONE_EVENT;
    /* ignore */
    return;
  }
}

#define MAX_WAIT_COUNT 10

int waitForMidiEvent(midi_event_t *evt, unsigned long timeout) {

  unsigned wait_cnt = 0;
  unsigned loop_cnt = 0;
  while ((event_rb_read_event(evt) == 0) || (evt->midiEvent == MIDI_NONE_EVENT)) {
    if (loop_cnt++ > 20000) {
      //      printf("sleeping\n");
      //      Sleep(200);
      //      if (++wait_cnt > timeout) {
      //      	printf("TImeout while waiting for midi event\n");
      //	return 0;
      //      }
      loop_cnt = 0;
    }
  }

  return 1;
}

void listOutputMidiDevices(void) {
  MIDIOUTCAPS     moc;
  unsigned long   iNumDevs, i;
  
  /* Get the number of MIDI Out devices in this computer */
  iNumDevs = midiOutGetNumDevs();

  printf("%lu output midi devices found\r\n", iNumDevs);
  
  /* Go through all of those devices, displaying their names */
  for (i = 0; i < iNumDevs; i++)
    {
      /* Get info about the next device */
      if (!midiOutGetDevCaps(i, &moc, sizeof(MIDIOUTCAPS)))
	{
	  /* Display its Device ID and name */
	  printf("Device ID #%lu: %s\r\n", i, moc.szPname);
	}
    }  
}

void listInputMidiDevices(void) {
  MIDIINCAPS     mic;
  unsigned long   iNumDevs, i;
  
  /* Get the number of MIDI Out devices in this computer */
  iNumDevs = midiInGetNumDevs();

  printf("%lu input midi devices found\r\n", iNumDevs);
  
  /* Go through all of those devices, displaying their names */
  for (i = 0; i < iNumDevs; i++)
    {
      /* Get info about the next device */
      if (!midiInGetDevCaps(i, &mic, sizeof(MIDIINCAPS))) {
	/* Display its Device ID and name */
	printf("Device ID #%lu: %s\r\n", i, mic.szPname);
      }
    }  
}

int getInDeviceName(int id, char *buf, int len) {
  MIDIINCAPS     mic;

  if (!midiInGetDevCaps(id, &mic, sizeof(MIDIINCAPS))) {
    strncpy(buf, mic.szPname, len);
    return 0;
  } else {
    buf[0] = '\0';
    return -1;
  }
}

int getOutDeviceName(int id, char *buf, int len) {
  MIDIOUTCAPS     moc;

  if (!midiOutGetDevCaps(id, &moc, sizeof(MIDIOUTCAPS))) {
    strncpy(buf, moc.szPname, len);
    return 0;
  } else {
    buf[0] = '\0';
    return -1;
  }
}

int testShortMsg(HMIDIIN in, HMIDIOUT out, uint8_t status, uint8_t byte1, uint8_t byte2) {
  midi_event_t event;
  int result;

  midiSendShort(out, status, byte1, byte2);
  //  printf("sent midi short msg\n");
  result = waitForMidiEvent(&event, 10);
  if (!result)
    return 0;
  //  printf("received midi short\n");
  if ((event.midiEvent == MIDI_DATA_RECEIVED) &&
      (SHORT_MSG_STATUS(event.midiShortMsg) == status) &&
      (SHORT_MSG_BYTE1(event.midiShortMsg) == byte1) &&
      (SHORT_MSG_BYTE2(event.midiShortMsg) == byte2)) {
    return 1;
  } else {
    printf("event received %x, should be %x\n", event.midiEvent, MIDI_DATA_RECEIVED);
    printf("%x:%x:%x should be %x:%x:%x data\n",
	   SHORT_MSG_STATUS(event.midiShortMsg),
	   SHORT_MSG_BYTE1(event.midiShortMsg),
	   SHORT_MSG_BYTE2(event.midiShortMsg),
	   status, byte1, byte2);
    return 0;
  }
}

int testMidiNote(HMIDIIN in, HMIDIOUT out, uint8_t channel, uint8_t type) {
  int note, velocity;

  printf("Testing midi note %x on channel %d\n", type, channel);

  for (note = 0; note < 128; note++) {
    printf(".");
    long start = timeGetTime();
    for (velocity = 0; velocity < 128; velocity++)
      if (!testShortMsg(in, out, type | channel, note, velocity))
	return 0;
    long end = timeGetTime();
    //    printf("384 bytes sent and 384 received in %f s\n", (float)(end - start)/1000.0);
  }
  printf("\n");

  return 1;
}

int testMidiController(HMIDIIN in, HMIDIOUT out, uint8_t channel) {
  int controller, value;

  for (controller = 0; controller < 127; controller++) {
    printf("Testing MIDI CONTROLLER message for controller %d\n", controller);
    for (value = 0; value < 127; value++)
      if (!testShortMsg(in, out, 0xB0 | channel, controller, value))
	return 0;
  }

  printf("Controller test successful\n");

  return 1;
}

int testMidiPatchChange(HMIDIIN in, HMIDIOUT out, uint8_t channel) {
  int program;

  for (program = 0; program < 128; program++)
    if (!testShortMsg(in, out, 0xC0 | channel, program, 0))
      return 0;

  return 1;
}

int testMidiChannelPressure(HMIDIIN in, HMIDIOUT out, uint8_t channel) {
  int pressure;

  for (pressure = 0; pressure < 128; pressure++)
    if (!testShortMsg(in, out, 0xC0 | channel, pressure, 0))
      return 0;

  return 1;
}

int testMidiPitchBend(HMIDIIN in, HMIDIOUT out, uint8_t channel) {
  int msb, lsb;

  for (msb = 0; msb < 128; msb++)
    for (lsb = 0; lsb < 128; lsb++)
      if (!testShortMsg(in, out, 0xB0 | channel, msb, lsb))
	return 0;

  return 1;
}

int testSysex(HMIDIIN in, HMIDIOUT out, uint8_t *buf, unsigned long len) {
  midi_event_t event;
  int result;

  midiSendLong(out, buf, len);
  result = waitForMidiEvent(&event, 10);
  if (!result)
    return 0;
  if ((event.midiEvent == MIDI_LONGDATA_RECEIVED) &&
      (event.midiLongMsgLen == len) && (!memcmp(buf, event.midiLongMsgData, len))) {
    return 1;
  } else {
    printf("event received %x, should be %x\n", event.midiEvent, MIDI_LONGDATA_RECEIVED);
    printf("length should be %lu, was %u\n", len, event.midiLongMsgLen);
    return 0;
  }
}

static void usage(void) {
  printf("Usage: ./midi-test [-h] [-l] [-i input] [-o output]\n");
  printf("-h : this help screen\n");
  printf("-l : list available midi devices\n");
  printf("-i input : use device number [input] as input device (default -1)\n");
  printf("-o output : use device number [output] as output device (default -1)\n");
}

int main(int argc, char *argv[]) {
  int c;
  int inputDevice = -1;
  int outputDevice = -1;
  int send_mode = 0;
  
  while ((c = getopt(argc, argv, "hi:o:ls")) != -1) {
    switch (c) {
    case 'i':
      inputDevice = atoi(optarg);
      break;

    case 'o':
      outputDevice = atoi(optarg);
      break;

    case 'l':
      printf("Output devices found:\n");
      listOutputMidiDevices();
      printf("\r\n");
      printf("Input devices found:\n");
      listInputMidiDevices();
      printf("\r\n");
      exit(0);
      break;

    case 's':
      printf("send mode\n");
      send_mode = 1;
      break;
      
    case 'h':
    default:
      usage();
      exit(0);
      break;

    }
  }

  event_rb_init();
  
  unsigned long result;
  HMIDIIN  inHandle;
  HMIDIOUT outHandle;
  unsigned char inputBuffer[MAX_LONG_SIZE];
  MIDIHDR midiHdr;

  char name[256];
  getOutDeviceName(outputDevice, name, sizeof(name));
  printf("Opening output device %s with ID %d\n", name, outputDevice);
  result = midiOutOpen(&outHandle, outputDevice, 0, 0, CALLBACK_NULL);
  if (result) {
    printf("Error opening output device %s with ID %d, aborting\n", name, outputDevice);
    exit(-1);
  } else {
    printf("Opened output device %s.\n", name);
  }

  getInDeviceName(inputDevice, name, sizeof(name));
  printf("Opening input device %s with ID %d\n", name, inputDevice);
  result = midiInOpen(&inHandle, inputDevice, (DWORD)midiCallback, 0, CALLBACK_FUNCTION);
  if (result) {
    printf("Error opening input device %s with ID %d, aborting\n", name, inputDevice);
    exit(-1);
  } else {
    printf("Opened input device %s.\n", name);
  }
  
  midiHdr.lpData = inputBuffer;
  midiHdr.dwBufferLength = sizeof(inputBuffer);
  midiHdr.dwFlags = 0;
  result = midiInPrepareHeader(inHandle, &midiHdr, sizeof(MIDIHDR));
  if (result) {
    printf("Could not prepare input buffer\n");
    goto end;
  }
  result = midiInAddBuffer(inHandle, &midiHdr, sizeof(MIDIHDR));
  if (result) {
    printf("could not add input buffer\n");
    goto end;
  }
  result = midiInStart(inHandle);
  if (result) {
    printf("Could not start recording on input\n");
    goto end;
  }

  if (send_mode) {
    for (;;) {
      midiSendShort(outHandle, 0x90, 0x00, 0x00);
    }

  } else {
    /*
      if (testShortMsg(inHandle, outHandle, 0x90, 1, 2)) {
      printf("Test successful\n");
      } else {
      printf("Test not successful\n");
      }
    */
    
    int channel;
    for (channel = 0; channel < 1; channel++) {
      printf("testing channel %d\n", channel);
      uint8_t buf[512];
      unsigned int i;
      buf[0] = 0xf0;
      for (i = 1; i < sizeof(buf) - 1; i++) {
	buf[i] = (i & 0x7F);
      }
      int end = 400;
      buf[end] = 0xF7;
      // XXX test
#if 0
      for (i= 0; i < 1; i++) {
	printf("test %d\n", i);
	if (testSysex(inHandle, outHandle, buf, end + 1)) {
	  printf("sysex successful\n");
	} else {
	  printf("sysex error\n");
	}
      }
#endif
      testMidiNote(inHandle, outHandle, channel, 0x80); /* Note OFF */
      testMidiNote(inHandle, outHandle, channel, 0x90); /* Note ON */
      testMidiNote(inHandle, outHandle, channel, 0xA0); /* Aftertouch */
      
      testMidiController(inHandle, outHandle, channel);
      testMidiPatchChange(inHandle, outHandle, channel);
      testMidiChannelPressure(inHandle, outHandle, channel);
      testMidiPitchBend(inHandle, outHandle, channel);
    }
  }

  for (;;) {
    Sleep(1);
  }
  midiInReset(inHandle);

 end:
  midiInUnprepareHeader(inHandle, &midiHdr, sizeof(MIDIHDR));
  midiOutClose(outHandle);
  midiInClose(inHandle);
  
  return 0;
}

