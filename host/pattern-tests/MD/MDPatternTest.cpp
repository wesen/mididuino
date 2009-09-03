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

int main() {
  FILE *f = fopen("/Users/manuel/Documents/test2.syx", "r");
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

  pattern2.init();
  pattern2.origPosition = pattern.origPosition;
  pattern2.patternLength = 16;
  for (int track = 0; track < 16; track++) {
    pattern2.trigPatterns[track] = pattern.trigPatterns[track];
    for (int param = 0; param < 24; param++) {
      if (pattern.isParamLocked(track, param)) {
	for (int i = 0; i < 32; i++) {
	  //	  pattern2.addLock(track, i, param, pattern.getLock(track, i, param));
	}
      }
    }
  }
  printf("\n\n");
  printf("COPY OF PATTERN ======================\n");
  pattern2.print();

  len = pattern.toSysex(data, sizeof(data));
  f = fopen("/Users/manuel/Documents/out.syx", "w");
  if (!f) {
    printf("ERROR WRITE\n");
    return 1;
  }
  fwrite(data, 1, len, f);
  fclose(f);

  if (!pattern3.fromSysex(data + 6, len - 7)) {
    printf("ERROR PARSING!\n");
  } else {
    printf("PARSED OK\n");
    pattern3.print();
  }

  return 0;
}
