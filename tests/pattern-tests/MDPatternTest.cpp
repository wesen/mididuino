#include <stdio.h>
#include "helpers.h"
#include <inttypes.h>
#include "MDPattern.hh"

#include "Midi.h"
#include "MidiUartHost.h"

#include "PrintMDPattern.h"

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
