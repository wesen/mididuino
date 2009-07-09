#include "MNM.h"

uint8_t monomachine_sysex_hdr[5] = {
  0x00,
  0x20,
  0x3c,
  0x03, /* monomachine ID */
  0x00 /* base channel padding */
};

