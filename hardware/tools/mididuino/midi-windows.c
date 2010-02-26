#include <unistd.h>
#include <stdio.h>
#include <windows.h>
#include <mmsystem.h>

#include "logging.h"
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

  debugPrintf(1, "%lu input midi devices found\r\n", iNumDevs);
  
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

  debugPrintf(1, "%lu output midi devices found\r\n", iNumDevs);
  
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
  HANDLE hBuffer;

  hBuffer = GlobalAlloc(GHND, len);
  if (!hBuffer) {
    logPrintf(LOG_ERROR, "error allocating buffer for sysex\n");
    return;
  } 

  midiHdr.lpData = (LPBYTE)GlobalLock(hBuffer);
  if (midiHdr.lpData) {
    midiHdr.dwBufferLength = len;
    midiHdr.dwFlags = 0;
    
    debugPrintf(2, "midiSendLong: \n");
    debugHexdump(2, buf, len);
    
    UINT err = midiOutPrepareHeader(outHandle, &midiHdr, sizeof(MIDIHDR));
    if (!err) {
      memcpy(midiHdr.lpData, buf, len);
      err = midiOutLongMsg(outHandle, &midiHdr, sizeof(MIDIHDR));
      if (err) {
	char errBuf[256];
	midiOutGetErrorText(err, errBuf, sizeof(errBuf));
	logPrintf(LOG_ERROR, "error sending long message: %s\n", errBuf);
	
      }
      while (MIDIERR_STILLPLAYING == midiOutUnprepareHeader(outHandle, &midiHdr, sizeof(MIDIHDR))) {
	;
	
      }
      debugPrintf(2, "midiSendLong finished\n");
    }
  }

  GlobalUnlock(hBuffer);
  GlobalFree(hBuffer);
    
}

void midiSendShort(unsigned char status, unsigned char byte1, unsigned char byte2) {
  debugPrintf(2, "midiSendShort %x %x %x\n", status, byte1, byte2);
  midiOutShortMsg(outHandle, MAKE_SHORT_MSG(status, byte1, byte2));
}

void CALLBACK midiCallback(HMIDIIN handle, UINT uMsg, DWORD dwInstance,
			   DWORD dwParam1, DWORD dwParam2) {
  debugPrintf(1, "callback called %d, handle %p\n", uMsg, handle);
  
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
      unsigned int len = 0;
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
    debugPrintf(1, "midi driver is throwing away received data: %x %x %x\n",
	      SHORT_MSG_STATUS(dwParam1), SHORT_MSG_BYTE1(dwParam1),
	      SHORT_MSG_BYTE2(dwParam1));
    break;
    
  case MIM_ERROR:
    logPrintf(LOG_ERROR, "error\n");
    break;
    
  case MIM_LONGERROR:
    {
      logPrintf(LOG_ERROR, "long error\n");
    }
    break;

  default:
    debugPrintf(1, "received event %x\n", uMsg);

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
    logPrintf(LOG_ERROR,
	      "Error opening output device %s with ID %d, aborting\n", name, outputDevice);
    exit(-1);
  } else {
    debugPrintf(1, "Opened output device %s.\n", name);
  }

  getInDeviceName(inputDevice, name, sizeof(name));
  result = midiInOpen(&inHandle, inputDevice, (DWORD)midiCallback, 0, CALLBACK_FUNCTION);
  if (result) {
    logPrintf(LOG_ERROR,
	      "Error opening input device %s with ID %d, aborting\n", name, inputDevice);
    exit(-1);
  } else {
    debugPrintf(1, "Opened input device %s.\n", name);
  }

  sleepCount = 0;
  
  midiHdr.lpData = inputBuffer;
  midiHdr.dwBufferLength = sizeof(inputBuffer);
  midiHdr.dwFlags = 0;
  result = midiInPrepareHeader(inHandle, &midiHdr, sizeof(MIDIHDR));

  if (result) {
    logPrintf(LOG_ERROR, "Could not prepare input buffer\n");
    exit(-1);
  }

  result = midiInAddBuffer(inHandle, &midiHdr, sizeof(MIDIHDR));

  if (result) {
    logPrintf(LOG_ERROR, "could not add input buffer\n");
    exit(-1);
  }

  result = midiInStart(inHandle);
  if (result) {
    logPrintf(LOG_ERROR, "Could not start recording on input\n");
    exit(-1);
  }
}

void midiMainLoop(void) {
  unsigned long result;

  for (; !exitMainLoop; ) {
    Sleep(10);
    debugPrintf(3, "count: %d\n", sleepCount);
    sleepCount++;
    if (sleepCount > 2000) {
      debugPrintf(1, "timeout\n");
      break;
    }
  }

  debugPrintf(2, "end of midiMainLoop\n");
  //  midiInReset(inHandle);

 end:
  midiClose();
}


void midiClose(void) {
  MMRESULT res;
  
  //  printf("stop\n");
  if (MMSYSERR_NOERROR != midiInStop(inHandle)) {
    logPrintf(LOG_ERROR, "input stop error\n");
  }
  //  printf("reset\n");
  if (MMSYSERR_NOERROR != midiInReset(inHandle)) {
    logPrintf(LOG_ERROR, "input reset error\n");
  }

  int retry = 0;
  do {
    res = midiInClose(inHandle);
    if (res != MMSYSERR_NOERROR) {
      logPrintf(LOG_ERROR, "input close error\n");
    }
    if (res == MIDIERR_STILLPLAYING) {
      midiInReset(inHandle);
    }
    Sleep(10);
    retry++;
  } while ((res == MIDIERR_STILLPLAYING) && (retry < 10));
  
  midiInUnprepareHeader(inHandle, &midiHdr, sizeof(MIDIHDR));

  retry = 0;
  res = midiOutReset(outHandle);
  if (res != MMSYSERR_NOERROR) {
    logPrintf(LOG_ERROR, "output reset error\n");
  }
  do {
    res = midiOutClose(outHandle);
    if (res != MMSYSERR_NOERROR) {
      logPrintf(LOG_ERROR, "output close error\n");
    }
    Sleep(10);
    retry++;
  } while ((res == MIDIERR_STILLPLAYING) && (retry < 10));
}
