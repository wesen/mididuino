#include <stdio.h>
#include "helpers.h"
#include <inttypes.h>
#include "MDPattern.hh"

#include "PrintMDPattern.h"

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
