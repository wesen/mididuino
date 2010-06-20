/**# Common AVR macros
 **
 ** This file contains common avr macros use to access hardware
 ** register bits, to test C conditions, to pack structures and
 ** mathematical macros
 **
 **/

#ifndef COMMON_H__
#define COMMON_H__

#include <inttypes.h>
#include <avr/pgmspace.h>

/** Define commonly used C symbols. **/
#define NULL 0
#define FALSE 0
#define TRUE 1

/** Compile-time assertion in C. **/
#define C_ASSERT(e) extern char __C_ASSERT__[(e)?1:-1]

/** Compress a C structure to byte aligned boundaries. **/
#define PACKED __attribute__ ((packed))

/** Math macros. **/
#define MIN(x, y) ((x) > (y) ? (y) : (x))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

/** Bit-level access and test macros. **/
#define SET_BIT(port, bit)   ((port) |= (1 << (bit)))
#define CLEAR_BIT(port, bit) ((port) &= ~(1 << (bit)))
#define IS_BIT_SET(port, bit) (((port) & (1 << (bit))) ? 1 : 0)
#define IS_BIT_CLEAR(port, bit) (((port) & (1 << (bit))) == 0 ? 1 : 0)

#define GET_FLASH(a)        (pgm_read_byte((uint16_t)(a)))
#define GET_FLASH_U16(a)    (256u*(pgm_read_byte((uint16_t)(a)))+(pgm_read_byte(((uint16_t)(a))+1)))
#define GET_FLASH_U16_LE(a) (pgm_read_byte((uint16_t)(a))+256u*(pgm_read_byte(((uint16_t)(a))+1)))
#define GET_FLASH_IDX(a,b)  (pgm_read_byte((uint16_t)(a)+(uint16_t)(b)))

#define GET_FLASH_PTR(a)    (void*)(pgm_read_byte((uint16_t)(a))+256u*(pgm_read_byte(((uint16_t)(a))+1)))

#define SET_BIT8(port, bit)   ((port) |= (uint8_t)(1 << (bit)))
#define CLEAR_BIT8(port, bit) ((port) &= (uint8_t)~(1 << (bit)))
#define IS_BIT_SET8(port, bit) (((port) & (uint8_t)(1 << (bit))) ? 1 : 0)
#define IS_BIT_CLEAR8(port, bit) (((port) & (uint8_t)(1 << (bit))) == 0 ? 1 : 0)

/* direction stuff and init values */
#define DDRIFY1(x)  DDR ## x
#define PINIFY1(x)  PIN ## x
#define PORTIFY1(x) PORT ## x
#define DDRIFY(x)   DDRIFY1(x)
#define PINIFY(x)   PINIFY1(x)
#define PORTIFY(x)  PORTIFY1(x)

#define INIT_PIN_OUTPUT(port, pin) { SET_BIT(DDRIFY(port), pin); }
#define INIT_PIN_INPUT(port, pin)  { CLEAR_BIT(DDRIFY(port), pin); }
#define IS_PIN_SET(port, pin)      IS_BIT_SET(PINIFY(port), pin)
#define IS_PIN_CLEAR(port, pin)    IS_BIT_CLEAR(PINIFY(port), pin)
#define SET_PIN(port, pin)         SET_BIT(PORTIFY(port), pin)
#define CLEAR_PIN(port, pin)       CLEAR_BIT(PORTIFY(port), pin)

#endif /* COMMON_H__ **/
