/*
 * MidiCtrl - midi send utility
 *
 * Uploads a firmware in sysex format to a RuinWesen device. This is really ugly and needs to be
 * rewritten. XXX
 *
 * (c) January 2012 - Manuel Odendahl - wesen@ruinwesen.com
 */

#include "Platform.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "MidiSend.h"
#include "HexFile.h"
#include "MidiFirmwareSender.h"

#include <inttypes.h>
#include "midi.h"
#include "midi-common.hh"
#include "logging.h"

int exitMainLoop = 0;

void usage(void) {
  logPrintf(LOG_USAGE, "./midi-send [-l] [-h] [-s] [-b] [-v] [-d] [-I ID in hex (default 38)] -i inputDevice -o outputDevice file\n");
}

int main(int argc, char *argv[]) {
  int c;
  char outputDevice[256] = "";
  char inputDevice[256]  = "";
  int deviceID = MIDI_CTRL_MINICOMMAND2_ID;

  bool statusMessage = false;
  bool startBootloader = false;
  int verbose = 1;

  while ((c = getopt(argc, argv, "ho:l:i:I:bvqsd")) != -1) {
    switch (c) {
    case 'b':
      startBootloader = true;
      break;
    case 'o':
      strncpy(outputDevice, optarg, sizeof(outputDevice));
      break;

    case 'i':
      strncpy(inputDevice, optarg, sizeof(inputDevice));
      break;

    case 'd':
      debugLevel++;
      break;

    case 'I':
      if ((strlen(optarg) > 2) && optarg[0] == '0' && optarg[1] == 'x') {
        deviceID = strtol(optarg + 2, NULL, 16);
      } else {
        deviceID = strtol(optarg, NULL, 10);
      }
      logPrintf(LOG_INFO, "deviceID: %x\n", deviceID);
      break;

    case 'l':
      if (optarg[0] == 'i') {
        MidiUartHostClass::listInputMidiDevices();
      } else {
        MidiUartHostClass::listOutputMidiDevices();
      }
      exit(0);
      break;

    case 'v':
      verbose++;
      break;

    case 's':
      statusMessage = true;
      break;

    case 'q':
      verbose--;
      break;

    case 'h':
    default:
      usage();
      exit(0);
      break;
    }
  }

  if (optind >= argc) {
    usage();
    exit(1);
  }

  char *inputFile = argv[optind];
  HexFile hexFile(deviceID, inputFile);
  if (!hexFile.load()) {
    fprintf(stderr, "Could not load firmware file, aborting\n");
    return 1;
  }

  if (!strcmp(outputDevice, "") || !strcmp(inputDevice, "")) {
    usage();
    exit(1);
  }

  char *endPtr = NULL;
  int inputDeviceNum = strtol(inputDevice, &endPtr, 10);
  char *endPtr2 = NULL;
  int outputDeviceNum = strtol(outputDevice, &endPtr2, 10);

  bool initialized = false;
  if (*endPtr == '\0' && *endPtr2 == '\0') {
    initialized = MidiUart.init(inputDeviceNum, outputDeviceNum);
  } else {
    initialized = MidiUart.init(inputDevice, outputDevice);
  }
  if (!initialized) {
    fprintf(stderr, "Could not open MIDI device, input: %s, output: %s\n", inputDevice, outputDevice);
    return 1;
  }

  MidiFirmwareSender sender(deviceID, statusMessage, verbose);
  Midi.midiSysex.addSysexListener(&sender);

  if (startBootloader) {
    if (statusMessage) {
      logPrintf(LOG_STATUS, "starting bootloader");
    }
  }

  sender.upload(&hexFile, startBootloader);

  for (;!sender.isTransferComplete();) {
    MidiUart.runLoop();
    while (MidiUart.avail()) {
      uint8_t c = MidiUart.getc();
      Midi.handleByte(c);
    }

    usleep(1000);
  }

  return 0;
}
