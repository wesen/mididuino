#include "helpers.h"
#include <stdio.h>

uint8_t machinedrum_sysex_hdr[5] = {
  0x00,
  0x20,
  0x3c,
  0x02,
  0x00
};

int main(void) {
  uint32_t val = 0;
  SET_BIT(val, 12);
  SET_BIT(val, 15);

  printf("%lx\n", val);

  return 0;
    
}
