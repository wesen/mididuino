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
    printf("lockpattern: %x\n", lockPatterns[track]);
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
  if (argc != 2)
    return 1;
  
  FILE *f = fopen(argv[1], "r");
  if (!f) {
    printf("ERROR\n");
    return 1;
  }
  size_t len = fread(data, 1, 8192, f);
  fclose(f);
  printf("len: %x\n", len);
  if (!pattern.fromSysex(data + 6, len - 7)) {
    printf("ARGS\n");
  } else {
    printf("SUCCESS\n");
  }
  pattern.print();

  return 0;
}
