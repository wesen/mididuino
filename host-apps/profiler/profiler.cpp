/*
 * MidiCtrl - Prints out profiling data received over sysex
 *
 * (c) 2009 - 2011 - Manuel Odendahl - wesen@ruinwesen.com
 *
 * The profiler samples the code on the destination device. A timer interrupt checks the return
 * address on the stack, encodes it as sysex and send it over MIDI. This receiving application
 * decodes the address and prints it out on stdout. It can then be piped to a script such as
 *   ./profiler | awk '{ system("avr-addr2line -e file.elf -f " $1); }'
 */

#include <inttypes.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


#include "Platform.h"
#include "MidiSysex.hh"

#include "MidiUartOSX.h"

/**
 * Holds the path to the ELF file that is being profiled.
 */
char *file = NULL;

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
    MidiUart.runLoop();
    while (MidiUart.avail()) {
      uint8_t c = MidiUart.getc();
      Midi.handleByte(c);
    }
    usleep(1000);
  }

  return 0;
}
