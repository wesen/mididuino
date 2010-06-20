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
#define ABS(x) ((x) > 0 ? (x) : (-(x)))
#define MIN(x, y) ((x) > (y) ? (y) : (x))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

  /** Bit-level access and test macros. **/
#define SET_BIT(port, bit)   ((port) |= (1 << (bit)))
#define CLEAR_BIT(port, bit) ((port) &= ~(1 << (bit)))
#define TOGGLE_BIT(port, bit) ((port) ^= (1 << (bit)))
#define IS_BIT_SET(port, bit) (((port) & (1 << (bit))) ? 1 : 0)
#define IS_BIT_CLEAR(port, bit) (((port) & (1 << (bit))) == 0 ? 1 : 0)

#if 1
#define SET_BIT8(port, bit)   ((port) |= (uint8_t)(1 << (bit)))
#define CLEAR_BIT8(port, bit) ((port) &= (uint8_t)~(1 << (bit)))
#define IS_BIT_SET8(port, bit) (((port) & (uint8_t)(1 << (bit))) ? 1 : 0)
#define IS_BIT_CLEAR8(port, bit) (((port) & (uint8_t)(1 << (bit))) == 0 ? 1 : 0)
#else

#define SET_BIT8(port, bit)      SET_BIT(port, bit)
#define CLEAR_BIT8(port, bit)    CLEAR_BIT(port, bit)
#define IS_BIT_SET8(port, bit)   IS_BIT_SET(port, bit)
#define IS_BIT_CLEAR8(port, bit) IS_BIT_CLEAR(port, bit)

#endif

#define GET_FLASH(a)        (pgm_read_byte((uint16_t)(a)))
#define GET_FLASH_U16(a)    (256u*(pgm_read_byte((uint16_t)(a)))+(pgm_read_byte(((uint16_t)(a))+1)))
#define GET_FLASH_U16_LE(a) (pgm_read_byte((uint16_t)(a))+256u*(pgm_read_byte(((uint16_t)(a))+1)))
#define GET_FLASH_IDX(a,b)  (pgm_read_byte((uint16_t)(a)+(uint16_t)(b)))

#define GET_FLASH_PTR(a)    (void*)(pgm_read_byte((uint16_t)(a))+256u*(pgm_read_byte(((uint16_t)(a))+1)))

#define FIRMWARE_LENGTH_ADDR ((uint16_t *)0x00)
#define FIRMWARE_CHECKSUM_ADDR ((uint16_t *)0x02)
#define START_MAIN_APP_ADDR ((uint16_t *)0x04)

    void my_memcpy(void *dst, void *src, uint8_t cnt);
    void my_strncpy(void *dst, char *src, uint8_t cnt);
    void my_strncpy_fill(void *dst, char *src, uint8_t cnt);
    void my_memcpy_p(void *dst, PGM_P src, uint8_t cnt);
    void my_strncpy_p(void *dst, PGM_P src, uint8_t cnt);
    void my_strncpy_p_fill(void *dst, PGM_P src, uint8_t cnt);
    void my_memclr(void *dst, uint8_t cnt);
    void my_str16cpy_fill(void *dst, char *src);
    void my_str16cpy_p_fill(void *dst, PGM_P src);
      void my_str16cpy_p(void *dst, PGM_P src);

#endif /* COMMON_H__ **/
