#ifndef HELPERS_H__
#define HELPERS_H__

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

#define SET_BIT32(port, bit)   ((port) |= (uint32_t)(1UL << (bit)))
#define CLEAR_BIT32(port, bit) ((port) &= (uint32_t)~(1UL << (bit)))
#define TOGGLE_BIT32(port, bit) ((port) ^= (uint32_t)(1UL << (bit)))
#define IS_BIT_SET32(port, bit) (((port) & (uint32_t)(1UL << (bit))) ? 1 : 0)
#define IS_BIT_CLEAR32(port, bit) (((port) & (uint32_t)(1UL << (bit))) == 0 ? 1 : 0)


#ifdef AVR
#define GET_FLASH(a)        (pgm_read_byte((uint16_t)(a)))
#define GET_FLASH_U16(a)    (256u*(pgm_read_byte((uint16_t)(a)))+(pgm_read_byte(((uint16_t)(a))+1)))
#define GET_FLASH_U16_LE(a) (pgm_read_byte((uint16_t)(a))+256u*(pgm_read_byte(((uint16_t)(a))+1)))
#define GET_FLASH_IDX(a,b)  (pgm_read_byte((uint16_t)(a)+(uint16_t)(b)))

#define GET_FLASH_PTR(a)    (void*)(pgm_read_byte((uint16_t)(a))+256u*(pgm_read_byte(((uint16_t)(a))+1)))
#else
#define GET_FLASH(a) *((uint8_t *)(a))
#define GET_FLASH_U16(a) (256u * GET_FLASH(a) + GET_FLASH((a)+1))
#define GET_FLASH_U16_LE(a) (GET_FLASH(a) + 256 * GET_FLASH((a) + 1))
#define GET_FLASH_IDX(a,b) (GET_FLASH((a) + (b)))
#define GET_FLASH_PTR(a) (void*)(GET_FLASH_U16(a))A
#endif

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

/** put data in external ram section **/
#define EXTRAM __attribute__ ((section(".bss.extram")))

/** Math macros. **/
#define MIN(x, y) ((x) > (y) ? (y) : (x))
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define ABS(x) ((x) < 0 ? -(x) : (x))

#ifdef __cplusplus
#include "BitArray.hh"

extern "C" {
#endif

#ifdef AVR
#define USE_LOCK()   uint8_t _irqlock_tmp
#define SET_LOCK()   _irqlock_tmp = SREG; cli()
#define CLEAR_LOCK() SREG = _irqlock_tmp
#else
#define USE_LOCK()   
#define SET_LOCK()   
#define CLEAR_LOCK() 
#endif
  
#include <inttypes.h>
#ifdef AVR
#include <avr/pgmspace.h>
#else
#define PGM_P const char *
#define pgm_read_byte(a) (uint8_t)((*a))
#endif

#ifdef MIDIDUINO
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#define round(x)     ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))

  long map(long x, long in_min, long in_max, long out_min, long out_max);
  
  void delay(unsigned int ms);
  void delayMicroseconds(unsigned int us);
#endif


void m_memcpy(void *dst, void *src, uint16_t cnt);
void m_strncpy(void *dst, const char *src, uint16_t cnt);
void m_strncpy_fill(void *dst, const char *src, uint16_t cnt);
void m_memcpy_p(void *dst, PGM_P src, uint16_t cnt);
void m_strncpy_p(void *dst, PGM_P src, uint16_t cnt);
void m_strncpy_p_fill(void *dst, PGM_P src, uint16_t cnt);
void m_memclr(void *dst, uint16_t cnt);
void m_str16cpy_fill(void *dst, const char *src);
void m_str16cpy_p_fill(void *dst, PGM_P src);
void m_str16cpy_p(void *dst, PGM_P src);
void m_strnappend(void *dst, const char *src, int len);

extern uint16_t read_clock(void);
extern uint16_t read_slowclock(void);
#ifndef HOST_MIDIDUINO
extern volatile uint16_t clock;
extern volatile uint16_t slowclock;
#endif

uint16_t clock_diff(uint16_t old_clock, uint16_t new_clock);
uint8_t u_limit_value(uint8_t value, int8_t encoder, uint8_t min, uint8_t max);
int limit_value(int value, int encoder, int min, int max);

#ifdef __cplusplus
}

#include "Task.hh"

#endif
  
#endif /* HELPERS_H__ */
