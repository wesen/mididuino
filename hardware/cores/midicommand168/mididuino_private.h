#ifndef MIDIDUINO_PRIVATE_H__
#define MIDIDUINO_PRIVATE_H__

#include <inttypes.h>
#include <avr/pgmspace.h>

  /** Bit-level access and test macros. **/
#define SET_BIT(port, bit)   ((port) |= (1 << (bit)))
#define CLEAR_BIT(port, bit) ((port) &= ~(1 << (bit)))
#define TOGGLE_BIT(port, bit) ((port) ^= (1 << (bit)))
#define IS_BIT_SET(port, bit) (((port) & (1 << (bit))) ? 1 : 0)
#define IS_BIT_CLEAR(port, bit) (((port) & (1 << (bit))) == 0 ? 1 : 0)

#define SET_BIT8(port, bit)   ((port) |= (uint8_t)(1 << (bit)))
#define CLEAR_BIT8(port, bit) ((port) &= (uint8_t)~(1 << (bit)))
#define IS_BIT_SET8(port, bit) (((port) & (uint8_t)(1 << (bit))) ? 1 : 0)
#define IS_BIT_CLEAR8(port, bit) (((port) & (uint8_t)(1 << (bit))) == 0 ? 1 : 0)

void init(void);

void delayMicroseconds(unsigned int us);
void delay(unsigned int ms);

inline void setLed(void) {
  CLEAR_BIT(PORTB, PB0);
}

inline void clearLed(void) {
  SET_BIT(PORTB, PB0);
}

extern uint16_t read_clock(void);
extern uint16_t read_slowclock(void);
extern volatile uint16_t clock;
extern volatile uint16_t slowclock;

uint16_t clock_diff(uint16_t old_clock, uint16_t new_clock);
uint8_t u_limit_value(uint8_t value, int8_t encoder, uint8_t min, uint8_t max);

#define FIRMWARE_LENGTH_ADDR ((uint16_t *)0x00)
#define FIRMWARE_CHECKSUM_ADDR ((uint16_t *)0x02)
#define START_MAIN_APP_ADDR ((uint16_t *)0x04)

void start_bootloader(void);

extern "C" {
  #include "helpers.h"
}

#endif /* MIDIDUIDNO_PRIVATE_H__ */
