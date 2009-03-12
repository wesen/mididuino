#include "app.h"
#include "common.h"
#include "sr165.h"

#define SR165_DELAY() { asm("nop"); asm("nop"); asm("nop"); }
static inline void sr165_clk(void) {
  CLEAR_BIT8(SR165_DATA_PORT, SR165_CLK);
  SR165_DELAY();
  SET_BIT8(SR165_DATA_PORT, SR165_CLK);
  SR165_DELAY();
}

static inline void sr165_rst(void) {
  CLEAR_BIT8(SR165_DATA_PORT, SR165_SHLOAD);
  SR165_DELAY();
  SET_BIT8(SR165_DATA_PORT, SR165_SHLOAD);
  SR165_DELAY();
}  

void sr165_init(void) {
  SR165_DDR_PORT |= _BV(SR165_SHLOAD) | _BV(SR165_CLK);
  CLEAR_BIT8(SR165_DDR_PORT, SR165_OUT);
  CLEAR_BIT8(SR165_DATA_PORT, SR165_OUT);
  SET_BIT8(SR165_DATA_PORT, SR165_CLK);
  SET_BIT8(SR165_DATA_PORT, SR165_SHLOAD);
}

uint8_t sr165_read(void) {
  sr165_rst();
  uint8_t res = 0;
  uint8_t i = 0;
  for (i = 0; i < 8; i++) {
    res <<= 1;
    res |= IS_BIT_SET8(SR165_PIN_PORT, SR165_OUT);
    sr165_clk();
  }

  return res;
}

uint8_t sr165_read_norst(void) {
  uint8_t res = 0;
  uint8_t i = 0;
  for (i = 0; i < 8; i++) {
    res <<= 1;
    res |= IS_BIT_SET8(SR165_PIN_PORT, SR165_OUT);
    sr165_clk();
  }

  return res;
}


uint16_t sr165_read16(void) {
  sr165_rst();
  uint16_t res = 0;
  uint8_t i = 0;
  for (i = 0; i < 16; i++) {
    res <<= 1;
    res |= IS_BIT_SET8(SR165_PIN_PORT, SR165_OUT);
    sr165_clk();
  }

  return res;
}
