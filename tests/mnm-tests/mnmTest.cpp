#include "WProgram.h"
#include "MidiSysex.hh"

#include "MNMSysex.hh"
#include "MNMMessages.hh"
#include "MNMPattern.hh"
#include "MNMParams.hh"

#include "MidiUartOSX.h"

#include <stdio.h>

MidiUartOSXClass MidiUart;
MidiClass Midi;

MNMPattern pattern;
MNMKit kit;
MNMGlobal global;
MNMSong song;

void hexDump(uint8_t *data, uint16_t len) {
  uint8_t cnt = 0;
  for (int i = 0; i < len; i++) {
    if (cnt == 0) {
      printf("%.4x: ", i & 0xFFF0);
    }
    printf("%.2x ", data[i]);
    cnt++;
    if (cnt == 8) {
      printf(" ");
    }
    if (cnt == 16) {
      printf("\n");
      cnt = 0;
    }
  }
  if (cnt != 0) {
    printf("\n");
  }
}

static void usage() {
  fprintf(stderr, "Usage: mnmTest file.syx\n");
}

uint8_t sysexBuf[8192];
uint16_t sysexLen;

void parseSysex(uint8_t *data, uint16_t len) {
  bool inSysex = false;
  for (int i = 0; i < len; i++) {
    uint8_t c = data[i];
    
    if (c == 0xF0) {
      inSysex = true;
      MidiSysex.reset();
    } else {
      if (inSysex) {
	if (c == 0xF7) {
	  inSysex = false;
	  MidiSysex.end();
	} else {
	  MidiSysex.handleByte(c);
	}
      }
    }
  }
}

uint8_t origBuf[8192];
uint16_t origLen;

bool inCallback = false;

void printBuf() {
  hexDump(MidiSysex.data + 3, MidiSysex.recordLen - 3);
  if (!inCallback) {
    m_memcpy(origBuf, MidiSysex.data, MidiSysex.recordLen);
    origLen = MidiSysex.recordLen;
  } else {
    printf("origLen: %d, newLen: %d\n", origLen, MidiSysex.recordLen);
    for (int i = 0; i < origLen; i++) {
      if (MidiSysex.data[i + 3] != origBuf[i + 3]) {
	printf("%.3x diff: %.2x was %.2x\n", i, MidiSysex.data[i + 3], origBuf[i + 3]);
      }
    }
  }
}

void onGlobalMessageCallback() {
  printBuf();
  
  if (!global.fromSysex(MidiSysex.data, MidiSysex.recordLen)) {
    fprintf(stderr, "error parsing global\n");
  } else {
    printf("parsed global!\n");
  }
  printf("\n");

  if (!inCallback) {
    inCallback = true;
    uint16_t len = global.toSysex(sysexBuf, sizeof(sysexBuf));
    //    hexDump(sysexBuf, len);
    parseSysex(sysexBuf, len);
  }

  inCallback = false;
}

void onKitMessageCallback() {
  printBuf();
  
  if (!kit.fromSysex(MidiSysex.data, MidiSysex.recordLen)) {
    fprintf(stderr, "error parsing kit\n");
  } else {
    printf("parsed kit!\n");
  }
  printf("\n");

  if (!inCallback) {
    inCallback = true;
    uint16_t len = kit.toSysex(sysexBuf, sizeof(sysexBuf));
    //    hexDump(sysexBuf, len);
    parseSysex(sysexBuf, len);
  }

  inCallback = false;
}

void onSongMessageCallback() {
  printBuf();
      
  if (!song.fromSysex(MidiSysex.data, MidiSysex.recordLen)) {
    fprintf(stderr, "error parsing song\n");
  } else {
    printf("parsed song!\n");
  }
  printf("\n");

  if (!inCallback) {
    inCallback = true;
    uint16_t len = song.toSysex(sysexBuf, sizeof(sysexBuf));
    hexDump(sysexBuf, len);
    parseSysex(sysexBuf, len);
  }

  inCallback = false;
}

void onPatternMessageCallback() {
  //  hexDump(MidiSysex.data, MidiSysex.recordLen);
  printBuf();
  
  if (!pattern.fromSysex(MidiSysex.data, MidiSysex.recordLen)) {
    fprintf(stderr, "error parsing pattern\n");
  } else {
    printf("parsed pattern!\n");
    pattern.print();
  }
  printf("\n");

  if (!inCallback) {
    inCallback = true;
    uint16_t len = pattern.toSysex(sysexBuf, sizeof(sysexBuf));
    //    hexDump(sysexBuf, len);
    parseSysex(sysexBuf, len);
  }

  inCallback = false;
}

void handleIncomingMidi() {
}
int main(int argc, char *argv[]) {
  if (argc != 2) {
    usage();
    return 1;
  }

  MNMSysexListener.setup();
  MNMSysexListener.setOnGlobalMessageCallback(onGlobalMessageCallback);
  MNMSysexListener.setOnKitMessageCallback(onKitMessageCallback);
  MNMSysexListener.setOnSongMessageCallback(onSongMessageCallback);
  MNMSysexListener.setOnPatternMessageCallback(onPatternMessageCallback);

  FILE *f = fopen(argv[1], "r");
  if (f == NULL) {
    fprintf(stderr, "error opening file: %s\n", argv[1]);
    return 1;
  }

  bool finished = false;
  uint32_t offset = 0;

  bool inSysex = false;
  
  do {
    int c = fgetc(f);
    offset++;
    if (c == EOF) {
      finished = true;
    } else {
      if (c == 0xF0) {
	inSysex = true;
	MidiSysex.reset();
      } else {
	if (inSysex) {
	  if (c == 0xF7) {
	    inSysex = false;
	    MidiSysex.end();
	  } else {
	    MidiSysex.handleByte(c);
	  }
	}
      }
    }
  } while (!finished);

  fclose(f);

  return 0;
}
