#include <inttypes.h>
#include <unistd.h>
#include <stdlib.h>

#include "WProgram.h"
#include "MidiSysex.hh"

#include "MidiUartOSX.h"

#include <stdio.h>

MidiUartOSXClass MidiUart;
MidiClass Midi, Midi2;

char *file = NULL;

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

class DumperSysexListener : public MidiSysexListenerClass {
public:
  char buf[8192];
  
  DumperSysexListener() {
    ids[0] = 0xff;
  }

  virtual ~DumperSysexListener() {
  }

  virtual void start() {
    MidiSysex.startRecord();
  }

  virtual void abort() {
  }

  virtual void end() {
		if (MidiSysex.recordLen == 4) {
			switch (MidiSysex.data[0]) {
			case 1:
				printf("byte: %x\n", (MidiSysex.data[1] << 7) | MidiSysex.data[2]);
				break;

			case 2:
				printf("short: %x\n", (MidiSysex.data[1] << 14) | (MidiSysex.data[2] << 7) | MidiSysex.data[3]);
				break;

			default:
				hexDump(MidiSysex.data, MidiSysex.recordLen);
				break;
			}
		} else {
			hexDump(MidiSysex.data, MidiSysex.recordLen);
		}
  }

  virtual void handleByte(uint8_t byte) {
  }
};

int main(int argc, char *argv[]) {
  // signal(SIGINT, SIG_IGN);
  file = argv[1];

  DumperSysexListener dumperSysex;
  
  MidiUart.init(0, 0);
  MidiSysex.addSysexListener(&dumperSysex);

  for (;;) {
    //    printf("foo\n");
    //    fflush(stdout);
    MidiUart.runLoop();
    while (MidiUart.avail()) {
      uint8_t c = MidiUart.getc();
      //      printf("%x\n", c);
      //      fflush(stdout);
      Midi.handleByte(c);
    }
    usleep(1000);
  }

  return 0;
}
