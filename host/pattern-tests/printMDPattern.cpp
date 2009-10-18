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
