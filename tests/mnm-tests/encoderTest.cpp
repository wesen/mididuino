#include <sys/time.h>#

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

#include "WProgram.h"
#include "Elektron.hh"

uint8_t origBuf[8192];
uint16_t origLen;

uint8_t encBuf[8192];
uint16_t encLen;

uint8_t decBuf[8192];
uint16_t decLen;

void hexDump(uint8_t *data, uint16_t len) {
  uint8_t cnt = 0;
  for (int i = 0; i < len; i++) {
    printf("%.2x ", data[i]);
    cnt++;
    if (cnt == 8) {
      printf(" ");
    }
    if (cnt == 16) {
      printf("\n");
      cnt = 0;
    }
  }
  if (cnt != 0) {
    printf("\n");
  }
}

void testEnc() {
  origLen = random() % 81;
  for (int i = 0; i < origLen; i++) {
    origBuf[i] = random() % 256;
  }

  encLen = ElektronHelper::MNMDataToSysex(origBuf, encBuf, origLen, sizeof(encBuf));

  decLen = ElektronHelper::MNMSysexToData(encBuf, decBuf, encLen, sizeof(decBuf));

  printf("origLen: %d, encLen: %d, decLen: %d\n", origLen, encLen, decLen);
  
  if (origLen != decLen) {
    printf("wrongLen\n");
    printf("orig\n");
    hexDump(origBuf, origLen);
    printf("encoded\n");
    hexDump(encBuf, encLen);
    printf("decoded\n");
    hexDump(decBuf, decLen);
  }


  for (int i = 0; i < origLen; i++) {
    if (origBuf[i] != decBuf[i]) {
      printf("%.4x: %.2x was %.2x\n", i, decBuf[i], origBuf[i]);
    }
  }

}

int main(void) {
  struct timeval tp;
  gettimeofday(&tp, NULL);
  srandom(tp.tv_usec);
  testEnc();
  return 0;
}
