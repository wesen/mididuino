#include <unistd.h>
#include <stdio.h>
#include <windows.h>

extern "C" {
#include <mmsystem.h>

#include "midi.h"

  extern int verbose;
void hexdump(unsigned char *buf, int len);

}

#include "RingBuffer.h"

HMIDIOUT outHandle;
HMIDIIN  inHandle;

RingBuffer<uint8_t, 1024> midiFifo;
RingBuffer<MIDIHDR*, 8> midiHdrFifo;

int sleepCount = 0;

#define MAX_LONG_SIZE 1024
unsigned char inputBuffer[8][MAX_LONG_SIZE];
MIDIHDR midiHdr[8];


#define MAKE_SHORT_MSG(status, byte1, byte2) \
       ((unsigned long)((status & 0xFF) | ((byte1 & 0xFF) << 8) | ((byte2 & 0xFF) << 16)))

#define SHORT_MSG_STATUS(s) ((unsigned char)(s & 0xFF))
#define SHORT_MSG_BYTE1(s) ((unsigned char)((s >> 8) & 0xFF))
#define SHORT_MSG_BYTE2(s) ((unsigned char)((s >> 16) & 0xFF))


void listInputMidiDevices(void) {
  MIDIINCAPS     mic;
  unsigned long   iNumDevs, i;
  
  /* Get the number of MIDI Out devices in this computer */
  iNumDevs = midiInGetNumDevs();

  //  printf("%lu input midi devices found\r\n", iNumDevs);
  
  /* Go through all of those devices, displaying their names */
  for (i = 0; i < iNumDevs; i++)
    {
      /* Get info about the next device */
      if (!midiInGetDevCaps(i, &mic, sizeof(MIDIINCAPS))) {
	/* Display its Device ID and name */
	printf("%lu) %s\r\n", i, mic.szPname);
      }
    }  
}

void listOutputMidiDevices(void) {
  MIDIOUTCAPS     moc;
  unsigned long   iNumDevs, i;
  
  /* Get the number of MIDI Out devices in this computer */
  iNumDevs = midiOutGetNumDevs();

  //  printf("%lu output midi devices found\r\n", iNumDevs);
  
  /* Go through all of those devices, displaying their names */
  for (i = 0; i < iNumDevs; i++)
    {
      /* Get info about the next device */
      if (!midiOutGetDevCaps(i, &moc, sizeof(MIDIOUTCAPS)))
	{
	  /* Display its Device ID and name */
	  printf("%lu) %s\r\n", i, moc.szPname);
	}
    }  
}

MIDIHDR outMidiHdr;


void midiSendLong(unsigned char *buf, unsigned long len) {
  MIDIHDR midiHdr;

  midiHdr.lpData = (CHAR *)buf;
  midiHdr.dwBufferLength = len;
  midiHdr.lpNext = NULL;
  midiHdr.dwFlags = 0;
  
  UINT err = midiOutPrepareHeader(outHandle, &midiHdr, sizeof(MIDIHDR));
  if (!err) {
    if (verbose >= 4) {
      printf("Sending\n");
      hexdump(buf, len);
    }
    
    err = midiOutLongMsg(outHandle, &midiHdr, sizeof(MIDIHDR));
    if (err) {
      char errBuf[256];
      midiOutGetErrorText(err, errBuf, sizeof(errBuf));
      printf("error sending long message: %s\n", errBuf);
      
    }
    int ret;
    do {
      ret = midiOutUnprepareHeader(outHandle, &midiHdr, sizeof(MIDIHDR));
    } while (ret == MIDIERR_STILLPLAYING);
  }
}

void midiSendShort(unsigned char status, unsigned char byte1, unsigned char byte2) {
  midiOutShortMsg(outHandle, MAKE_SHORT_MSG(status, byte1, byte2));
}

void CALLBACK midiCallback(HMIDIIN handle, UINT uMsg, DWORD dwInstance,
			   DWORD dwParam1, DWORD dwParam2) {
  //  printf("callback called %d, handle %p\n", uMsg, handle);
  switch (uMsg) {
  case MIM_DATA:
    midiFifo.put(SHORT_MSG_STATUS(dwParam1));
    midiFifo.put(SHORT_MSG_BYTE1(dwParam1));
    midiFifo.put(SHORT_MSG_BYTE2(dwParam1));
    break;

  case MIM_LONGDATA:
    {
      MIDIHDR *midiHdr = (MIDIHDR *)dwParam1;
      /*
      printf("flags: %x, length: %d, recorded: %d, data: %p\n", midiHdr->dwFlags,
	     midiHdr->dwBufferLength,
	     midiHdr->dwBytesRecorded,
	     midiHdr->lpData);
      */
      midiHdrFifo.put(midiHdr);
    }
    break;

  case MIM_MOREDATA:
    // XXX set IO_STATUS flag to midiInOpen()
    printf("midi driver is throwing away received data: %x %x %x\n",
	   SHORT_MSG_STATUS(dwParam1), SHORT_MSG_BYTE1(dwParam1),
	   SHORT_MSG_BYTE2(dwParam1));
    break;
    
  case MIM_ERROR:
    printf("error\n");
    break;
    
  case MIM_LONGERROR:
    {
      printf("long error\n");
    }
    break;

  default:
    //    printf("received event %x\n", uMsg);
    /* ignore */
    return;
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

void midiInitialize(char *inputDeviceStr, char *outputDeviceStr) {
  int inputDevice = atoi(inputDeviceStr);
  int outputDevice = atoi(outputDeviceStr);
  char name[256];
  getOutDeviceName(outputDevice, name, sizeof(name));
  unsigned long result = midiOutOpen(&outHandle, outputDevice, 0, 0, CALLBACK_NULL);
  if (result) {
    printf("Error opening output device %s with ID %d, aborting\n", name, outputDevice);
    exit(-1);
  } else {
    printf("Opened output device %s.\n", name);
  }

  getInDeviceName(inputDevice, name, sizeof(name));
  result = midiInOpen(&inHandle, inputDevice, (DWORD)midiCallback, 0, CALLBACK_FUNCTION);
  if (result) {
    printf("Error opening input device %s with ID %d, aborting\n", name, inputDevice);
    exit(-1);
  } else {
    printf("Opened input device %s.\n", name);
  }

  for (int i = 0; i < 8; i++) {
    midiHdr[i].lpData = (CHAR*)inputBuffer[i];
    midiHdr[i].dwBufferLength = sizeof(inputBuffer[i]);
    midiHdr[i].dwFlags = 0;
    result = midiInPrepareHeader(inHandle, &midiHdr[i], sizeof(MIDIHDR));
    if (result) {
      printf("Could not prepare input buffer\n");
      exit(-1);
    }
    result = midiInAddBuffer(inHandle, &midiHdr[i], sizeof(MIDIHDR));
    if (result) {
      printf("could not add input buffer\n");
      exit(-1);
    }
  }

  result = midiInStart(inHandle);
  if (result) {
    printf("Could not start recording on input\n");
      exit(-1);
  }
}

void handleMidiHdrFifo();

void midiMainLoop(void) {
  sleepCount = 0;

  for (; !exitMainLoop; ) {
    handleMidiHdrFifo();
    if (!midiFifo.isEmpty()) {
      while (!midiFifo.isEmpty()) {
	midiReceive(midiFifo.get());
      }
    } else {
      Sleep(1);		      // 
      //    printf("count: %d\n", sleepCount);
      sleepCount++;
      if (sleepCount > 200) {
	printf("timeout\n");
	break;
      }
    }
  }

  midiClose();
}

void handleMidiHdrFifo() {
  HMIDIIN handle = inHandle;
  
  while (!midiHdrFifo.isEmpty()) {
    MIDIHDR *midiHdr = midiHdrFifo.get();
    //    printf("got midihdr in main: %p\n", midiHdr);
    
    if (midiHdr->dwBytesRecorded == 0) {
      continue;
    }
    unsigned int len = 0;
    sleepCount = 0;
    if (verbose >= 3) {
      printf("Receiving\n");
    }
    
    for (len = 0; len < midiHdr->dwBufferLength; len++) {
      midiFifo.put(midiHdr->lpData[len]);
      if (((unsigned char)midiHdr->lpData[len]) == 0xF7) {
	len++;
	break;
      }
    }
    
    if (verbose >= 3) {
      hexdump((unsigned char *)midiHdr->lpData, len);
    }
    
    midiInUnprepareHeader(handle, midiHdr, sizeof(*midiHdr));
    midiInPrepareHeader(handle, midiHdr, sizeof(*midiHdr));
    midiInAddBuffer(handle, midiHdr, sizeof(*midiHdr));
  }
}
  


void midiClose(void) {
  //  printf("stop\n");
  if (MMSYSERR_NOERROR != midiInStop(inHandle)) {
    //    printf("stop error\n");
  }
  //  printf("reset\n");
  if (MMSYSERR_NOERROR != midiInReset(inHandle)) {
    printf("reset error\n");
  }
  if (MMSYSERR_NOERROR != midiInClose(inHandle)) {
    printf("close error\n");
  }
  //  midiInUnprepareHeader(inHandle, &midiHdr, sizeof(MIDIHDR));
  midiInReset(inHandle);
  //  midiInClose(inHandle);
  midiOutClose(outHandle);

}
