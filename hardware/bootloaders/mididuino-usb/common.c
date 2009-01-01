#include "app.h"

#include <util/delay.h>

void delay(uint8_t i) {
  uint8_t n;
  for (n = 0; n < i; n++) {
    _delay_us(200);
  }
}

