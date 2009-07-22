#include <inttypes.h>
#include <unistd.h>
#include <stdlib.h>

#include "WProgram.h"
#include "MidiSysex.hh"

#include "MidiUartOSX.h"

#include <stdio.h>

MidiUartOSXClass MidiUart;
MidiClass Midi;

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
    uint8_t *ptr = MidiSysex.data;
    
    for (int i = 1; i < MidiSysex.recordLen; i+=3) {
      uint16_t addr = ptr[i] << 14 | (ptr[i+1] << 7) | ptr[i+2];
      if (addr != 0) {
	//	char foobar[256] = "";
	//	sprintf(foobar, "avr-addr2line -e %s -f %x", file, addr);
	//	//	printf("%s\n", foobar);
	//	system(foobar);
	printf("%x\n", addr);
      }
    }
    fflush(stdout);
    //    exit(1);
    //    hexDump(MidiSysex.data, MidiSysex.recordLen);
  }

  virtual void handleByte(uint8_t byte) {
  }
};

int main(int argc, char *argv[]) {
  // signal(SIGINT, SIG_IGN);
  file = argv[1];

  ProfilerSysexListener profilerSysex;
  
  MidiUart.init(0, 0);
  MidiSysex.addSysexListener(&profilerSysex);

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
