#include <unistd.h>
#include <stdio.h>
#include <windows.h>

extern "C" {
#include <mmsystem.h>

#include "midi.h"

 int verbose = 0;
void hexdump(unsigned char *buf, int len);

}

#include "RingBuffer.h"

#define NUM_HDRS 4

RingBuffer<MIDIHDR*, NUM_HDRS> midiHdrFifo;
HMIDIIN  inHandle;
HMIDIOUT outHandle;

#define MIN(a, b) ( a > b ? b : a )

#define MAX_LONG_SIZE 128
unsigned char inputBuffer[NUM_HDRS][MAX_LONG_SIZE];
MIDIHDR midiHdr[NUM_HDRS];

void hexdump(unsigned char *buf, int len) {
  int i;
  for (i = 0; i < len; i+=16) {
    int j;
    for (j = i; j < MIN(i+16, len); j++) {
      printf("%2x ", buf[j]);
    }
    printf("\n");
  }
}

void CALLBACK midiCallback(HMIDIIN handle, UINT uMsg, DWORD dwInstance,
			   DWORD dwParam1, DWORD dwParam2) {
  printf("callback called %d, handle %p\n", uMsg, handle);
  switch (uMsg) {
  case MIM_DATA:
    //    midiFifo.put(SHORT_MSG_STATUS(dwParam1));
    //    midiFifo.put(SHORT_MSG_BYTE1(dwParam1));
    //    midiFifo.put(SHORT_MSG_BYTE2(dwParam1));
    break;

  case MIM_LONGDATA:
    {
      MIDIHDR *midiHdr = (MIDIHDR *)dwParam1;
      printf("flags: %x, length: %d, recorded: %d, data: %p\n", midiHdr->dwFlags,
	     midiHdr->dwBufferLength,
	     midiHdr->dwBytesRecorded,
	     midiHdr->lpData);
      midiHdrFifo.put(midiHdr);
    }
    break;

  case MIM_MOREDATA:
    // XXX set IO_STATUS flag to midiInOpen()
    //    printf("midi driver is throwing away received data: %x %x %x\n",
    //	   SHORT_MSG_STATUS(dwParam1), SHORT_MSG_BYTE1(dwParam1),
    //	   SHORT_MSG_BYTE2(dwParam1));
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
    printf("received event %x\n", uMsg);
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



int sleepCount = 0;


void handleMidiHdrFifo() {
  HMIDIIN handle = inHandle;
  
  while (!midiHdrFifo.isEmpty()) {
    MIDIHDR *midiHdr = midiHdrFifo.get();
    printf("got midihdr in main: %p\n", midiHdr);
    
    if (midiHdr->dwBytesRecorded == 0) {
      continue;
    }
    unsigned int len = 0;
    sleepCount = 0;
    if (verbose >= 3) {
      printf("Receiving\n");
    }
    
    for (len = 0; len < midiHdr->dwBufferLength; len++) {
      //      midiFifo.put(midiHdr->lpData[len]);
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

unsigned char sendBuf[6] = {0xf0, 0x00, 0x13, 0x41, 0x05, 0xf7};

void midiSendLong(unsigned char *buf, unsigned long len) {
  MIDIHDR midiHdr;

  midiHdr.lpData = (CHAR *)buf;
  midiHdr.dwBufferLength = len;
  midiHdr.dwFlags = 0;
  
  UINT err = midiOutPrepareHeader(outHandle, &midiHdr, sizeof(MIDIHDR));
  if (!err) {
    //    memcpy(midiHdr.lpData, buf, len);
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
    while (MIDIERR_STILLPLAYING == midiOutUnprepareHeader(outHandle, &midiHdr, sizeof(MIDIHDR)))
      ;
  }
}


void initSysex() {
  unsigned long result;
  midiInReset(inHandle);
  for (int i = 0; i < NUM_HDRS; i++) {
    MIDIHDR *newHdr = (MIDIHDR*)malloc(sizeof(MIDIHDR));
    unsigned char *data = (unsigned char *)malloc(MAX_LONG_SIZE);
    
    //    midiHdr[i].lpData = (CHAR*)inputBuffer[i];
    //    midiHdr[i].dwBufferLength = sizeof(inputBuffer[i]);
    //    midiHdr[i].dwFlags = 0;

    newHdr->lpData = (char*)data;
    newHdr->dwBufferLength = MAX_LONG_SIZE;
    newHdr->dwFlags = 0;
    
    result = midiInPrepareHeader(inHandle, newHdr, sizeof(MIDIHDR));
    if (result) {
      printf("Could not prepare input buffer\n");
      exit(-1);
    }
    result = midiInAddBuffer(inHandle, newHdr, sizeof(MIDIHDR));
    if (result) {
      printf("could not add input buffer\n");
      exit(-1);
    }
  }

}

int main() {
  int inputDevice = 1;
  char name[256];
  unsigned long result = 0;

  int outputDevice = 2;
  getOutDeviceName(outputDevice, name, sizeof(name));
  result = midiOutOpen(&outHandle, outputDevice, 0, 0, CALLBACK_NULL);
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

  initSysex();
  result = midiInStart(inHandle);
  if (result) {
    printf("Could not start recording on input\n");
      exit(-1);
  }

  midiInStop(inHandle);
  initSysex();
  midiInStart(inHandle);

  sleepCount = 0;
  //  midiSendLong(sendBuf, 6);

  int sendCount = 1;


  for (;;) {
    handleMidiHdrFifo();

    Sleep(1);		      // 
    //    printf("count: %d\n", sleepCount);
    sleepCount++;
    if (sleepCount > 300) {
      //      midiSendLong(sendBuf, 6);
      printf("sendCount: %d\n", sendCount++);

      printf("timeout\n");
      //      break;
      sleepCount = 0;
    }
  }
  //  midiInReset(inHandle);

 end:
  midiClose();
  
  
}
void midiClose(void) {
  printf("stop\n");
  if (MMSYSERR_NOERROR != midiInStop(inHandle)) {
    //    printf("stop error\n");
  }
  printf("reset\n");
  if (MMSYSERR_NOERROR != midiInReset(inHandle)) {
    printf("reset error\n");
  }
  if (MMSYSERR_NOERROR != midiInClose(inHandle)) {
    printf("close error\n");
  }
  //  midiInUnprepareHeader(inHandle, &midiHdr, sizeof(MIDIHDR));
  //midiInReset(inHandle);
  //  midiInClose(inHandle);
}
