#include <stdio.h>
#include "helpers.h"
#include <inttypes.h>
#include "MDPattern.hh"

uint8_t machinedrum_sysex_hdr[5] = {
  0x00,
  0x20,
  0x3c,
  0x02,
  0x00
};


class PrintMDPattern : public MDPattern {
public:
  void printTrig(int track) {
    for (int i = 0; i < patternLength; i++) {
      if (IS_BIT_SET(trigPatterns[track], i)) {
	printf("X   ");
      } else {
	printf(".   ");
      }
    }
    printf("\n");
  }

  void printLocks(int track) {
    for (int param = 0; param < 24; param++) {
      if (isParamLocked(track, param)) {
	printf("P%.2d: ", param);
	//	printf("    ");
	for (int i = 0; i < patternLength; i++) {
	  uint8_t lock = getLock(track, i, param);
	  if (lock != 255) {
	    printf("%.3d ", lock);
	  } else {
	    printf("    ");
	  }
	}
	printf("\n");
      }
    }
  }
  
  void print() {
    for (int i = 0; i < 16; i++) {
      printf("\n%.2d:  ", i);
      printTrig(i);
      printLocks(i);
    }
  }
};

PrintMDPattern pattern;
PrintMDPattern pattern2;
PrintMDPattern pattern3;

uint8_t data[8192];

int main(int argc, char *argv[]) {
  pattern.init();
  pattern.origPosition = 0;
  pattern.kit = 0;
  //    for (int track = 0; track < 16; track++) {
  //      for (int i = 0; i < 16; i++) {
  //         pattern.setTrig(track, i);
  //         pattern.addLock(track, i, 0, i);
  //         pattern.addLock(track, i, 3, 16 - i);
  //      }
  //      pattern.addLock(track, 0, track, track);
  //    }
  pattern.addLock(12, 0, 15, 15);
  pattern.addLock(11, 0, 14, 12);
  pattern.addLock(15, 0, 11, 15);
  //  pattern.addLock(15, 0, 12, 15);
  //  pattern.addLock(15, 0, 15, 15);
  //  pattern.addLock(15, 0, 14, 12);
  pattern.print();
  uint16_t len = pattern.toSysex(data, sizeof(data));
  pattern2.fromSysex(data + 6, len - 7);
  pattern2.print();
  FILE *f = fopen("/Users/manuel/Documents/out.syx", "w");
  if (!f) {
    printf("ERROR WRITE\n");
    return 1;
  }
  fwrite(data, 1, len, f);
  fclose(f);

  return 0;
}
