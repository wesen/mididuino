#include <inttypes.h>
#include <unistd.h>
#include <stdlib.h>

#include "WProgram.h"
#include "MidiSysex.hh"

#include "MidiUartOSX.h"

#include <stdio.h>

MidiUartOSXClass MidiUart;
MidiUartOSXClass MidiUart2;

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


int main(int argc, char *argv[]) {
  MidiUart.init(0, 0);
  MidiUart2.init(1, 1);

  for (;;) {
    MidiUart.runLoop();
    MidiUart2.runLoop();
    while (MidiUart.avail()) {
      MidiUart2.putc(MidiUart.getc());
    }
    while (MidiUart2.avail()) {
      MidiUart.putc(MidiUart2.getc());
    }
    usleep(1000);
  }

  return 0;
}
