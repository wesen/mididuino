#include "WProgram.h"
#include "MidiSysex.hh"

#include "MNMSysex.hh"
#include "MNMMessages.hh"
#include "MNMPattern.hh"
#include "MNMParams.hh"

#include <stdio.h>

MNMPattern pattern;
MNMKit kit;
MNMGlobal global;
MNMSong song;

static void usage() {
  fprintf(stderr, "Usage: mnmTest file.syx\n");
}

uint8_t sysexBuf[8192];
uint16_t sysexLen;

void onGlobalMessageCallback() {
  for (uint16_t i = 0; i < MidiSysex.recordLen; i++) {
    printf("%x ", MidiSysex.data[i]);
  }
  printf("\n\n");
      
  if (!global.fromSysex(MidiSysex.data, MidiSysex.recordLen)) {
    fprintf(stderr, "error parsing global\n");
  } else {
    printf("parsed global!\n");
  }
}

void onKitMessageCallback() {
  for (uint16_t i = 0; i < MidiSysex.recordLen; i++) {
    printf("%x ", MidiSysex.data[i]);
  }
  printf("\n\n");
      
  if (!kit.fromSysex(MidiSysex.data, MidiSysex.recordLen)) {
    fprintf(stderr, "error parsing kit\n");
  } else {
    printf("parsed kit!\n");
  }
}

void onSongMessageCallback() {
}

void onPatternMessageCallback() {
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
