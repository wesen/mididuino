#ifndef MIDIDUINO_PRIVATE_H__
#define MIDIDUINO_PRIVATE_H__

#ifdef __cplusplus
extern "C" {
#endif
  
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

#define GET_FLASH(a)        (pgm_read_byte((uint16_t)(a)))
#define GET_FLASH_U16(a)    (256u*(pgm_read_byte((uint16_t)(a)))+(pgm_read_byte(((uint16_t)(a))+1)))
#define GET_FLASH_U16_LE(a) (pgm_read_byte((uint16_t)(a))+256u*(pgm_read_byte(((uint16_t)(a))+1)))
#define GET_FLASH_IDX(a,b)  (pgm_read_byte((uint16_t)(a)+(uint16_t)(b)))

#define GET_FLASH_PTR(a)    (void*)(pgm_read_byte((uint16_t)(a))+256u*(pgm_read_byte(((uint16_t)(a))+1)))

/** Define commonly used C symbols. **/
#ifndef NULL
#define NULL 0
#endif
#define FALSE 0
#define TRUE 1

#define countof(arr) ((sizeof(arr)/sizeof(arr[0])))
  
/** Compile-time assertion in C. **/
#define C_ASSERT(e) extern char __C_ASSERT__[(e)?1:-1]

/** Compress a C structure to byte aligned boundaries. **/
#define PACKED __attribute__ ((packed))

/** Math macros. **/
#define MIN(x, y) ((x) > (y) ? (y) : (x))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

void init(void);

void delayMicroseconds(unsigned int us);
void delay(unsigned int ms);

void setLed(void);

void clearLed(void);

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

#include "helpers.h"

#ifdef __cplusplus
}
#endif
  
#endif /* MIDIDUIDNO_PRIVATE_H__ */
