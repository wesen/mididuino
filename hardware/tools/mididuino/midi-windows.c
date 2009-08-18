#include <unistd.h>
#include <stdio.h>
#include <windows.h>
#include <mmsystem.h>

#include "midi.h"

HMIDIOUT outHandle;
HMIDIIN  inHandle;

int sleepCount = 0;

#define MAX_LONG_SIZE 1024
unsigned char inputBuffer[MAX_LONG_SIZE];
MIDIHDR midiHdr;


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

void midiSendLong(unsigned char *buf, unsigned long len) {
  MIDIHDR midiHdr;

  midiHdr.lpData = (LPBYTE)buf;
  midiHdr.dwBufferLength = len;
  midiHdr.dwFlags = 0;
  
  UINT err = midiOutPrepareHeader(outHandle, &midiHdr, sizeof(MIDIHDR));
  if (!err) {
    //    memcpy(midiHdr.lpData, buf, len);
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

void midiSendShort(unsigned char status, unsigned char byte1, unsigned char byte2) {
  midiOutShortMsg(outHandle, MAKE_SHORT_MSG(status, byte1, byte2));
}

void CALLBACK midiCallback(HMIDIIN handle, UINT uMsg, DWORD dwInstance,
			   DWORD dwParam1, DWORD dwParam2) {
  //  printf("callback called %d, handle %p\n", uMsg, handle);
  switch (uMsg) {
  case MIM_DATA:
    midiReceive(SHORT_MSG_STATUS(dwParam1));
    midiReceive(SHORT_MSG_BYTE1(dwParam1));
    midiReceive(SHORT_MSG_BYTE2(dwParam1));
    break;

  case MIM_LONGDATA:
    {
      MIDIHDR *midiHdr = (MIDIHDR *)dwParam1;
      if (midiHdr->dwBytesRecorded == 0) {
	return;
      }
      int len = 0;
      sleepCount = 0;
      for (len = 0; len < midiHdr->dwBufferLength; len++) {
	midiReceive(midiHdr->lpData[len]);
	if (((unsigned char)midiHdr->lpData[len]) == 0xF7)
	  break;
      }
      midiInUnprepareHeader(handle, midiHdr, sizeof(*midiHdr));
      midiInPrepareHeader(handle, midiHdr, sizeof(*midiHdr));
      midiInAddBuffer(handle, midiHdr, sizeof(*midiHdr));
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

}

void midiMainLoop(void) {
  unsigned long result;

  sleepCount = 0;
  
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

  for (; !exitMainLoop; ) {
    Sleep(10);
    //    printf("count: %d\n", sleepCount);
    sleepCount++;
    if (sleepCount > 2000) {
      printf("timeout\n");
      break;
    }
  }
  //  midiInReset(inHandle);
  //  printf("foo\n");

 end:
  midiClose();
}


void midiClose(void) {
  //  printf("stop\n");
  if (MMSYSERR_NOERROR != midiInStop(inHandle)) {
    printf("stop error\n");
  }
  //  printf("reset\n");
  if (MMSYSERR_NOERROR != midiInReset(inHandle)) {
    printf("reset error\n");
  }
  //  printf("close\n");
  if (MMSYSERR_NOERROR != midiInClose(inHandle)) {
    printf("close error\n");
  }
  //  printf("foobnar\n");
  midiInUnprepareHeader(inHandle, &midiHdr, sizeof(MIDIHDR));
  //  printf("closing b\n");
  //  midiInReset(inHandle);
  midiOutClose(outHandle);
  //  printf("closing b\n");
  Sleep(10);
}
