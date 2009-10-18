#ifndef PRINT_MD_PATTERN_H__
#define PRINT_MD_PATTERN_H__

#include "MD.h"

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
    printf("lockpattern: %llx\n", lockPatterns[track]);

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

#endif /* PRINT_MD_PATTERN_H__ */
