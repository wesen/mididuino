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

void parseSysex() {
  if (sysexLen > 6) {
    switch (sysexBuf[6]) {
    case MNM_GLOBAL_MESSAGE_ID:
      if (!global.fromSysex(sysexBuf + 6, sysexLen - 7)) {
	fprintf(stderr, "error parsing global\n");
      } else {
	printf("parsed global!\n");
      }
      break;

    case MNM_KIT_MESSAGE_ID:
      if (!kit.fromSysex(sysexBuf + 6, sysexLen - 7)) {
	fprintf(stderr, "error parsing kit\n");
      } else {
	printf("parsed kit!\n");
      }
      break;

    case MNM_PATTERN_MESSAGE_ID:
      if (!pattern.fromSysex(sysexBuf + 6, sysexLen - 7)) {
	fprintf(stderr, "error parsing pattern\n");
      } else {
	printf("parsed pattern!\n");
      }
      break;

    case MNM_SONG_MESSAGE_ID:
      if (!song.fromSysex(sysexBuf + 6, sysexLen - 7)) {
	fprintf(stderr, "error parsing song\n");
      } else {
	printf("parsed song!\n");
      }
      break;

    default:
      fprintf(stderr, "Unknown message type: %x\n", sysexBuf[6]);
      break;
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    usage();
    return 1;
  }

  MNMSysexListener.setup();

  FILE *f = fopen(argv[1], "r");
  if (f == NULL) {
    fprintf(stderr, "error opening file: %s\n", argv[1]);
    return 1;
  }

  bool finished = false;
  uint32_t offset = 0;

  do {
    int c = fgetc(f);
    offset++;
    if (c == EOF) {
      finished = true;
    } else {
      sysexBuf[sysexLen++] = c;
      if (c == 0xF7) {
	printf("parse sysex from %d to %d, size: %d\n", offset - sysexLen, offset, sysexLen);
	parseSysex();
	sysexLen = 0;
      }
    }
  } while (!finished);

  fclose(f);

  return 0;
}
