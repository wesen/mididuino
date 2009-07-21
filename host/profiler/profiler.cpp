#include <inttypes.h>
#include <unistd.h>

#include "WProgram.h"
#include "MidiSysex.hh"

#include "MidiUartOSX.h"

#include <stdio.h>

MidiUartOSXClass MidiUart;
MidiClass Midi;

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

class ProfilerSysexListener : public MidiSysexListenerClass {
public:
  char buf[8192];
  
  ProfilerSysexListener() {
    ids[0] = 0x01;
    ids[1] = 0x00;
    ids[2] = 0x00;
  }

  virtual ~ProfilerSysexListener() {
  }

  virtual void start() {
    MidiSysex.startRecord();
  }

  virtual void abort() {
  }

  virtual void end() {
    hexDump(MidiSysex.data, MidiSysex.recordLen);
  }

  virtual void handleByte(uint8_t byte) {
  }
};

int main(int argc, char *argv[]) {

  ProfilerSysexListener profilerSysex;
  
  MidiUart.init(0, 0);
  MidiSysex.addSysexListener(&profilerSysex);

  for (;;) {
    MidiUart.runLoop();
    while (MidiUart.avail()) {
      uint8_t c = MidiUart.getc();
      Midi.handleByte(c);
    }
    usleep(1000);
  }

  return 0;
}
