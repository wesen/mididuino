/*
 * MidiCtrl - General helper macros
 *
 * (c) 2009 - 2011 - Manuel Odendahl - wesen@ruinwesen.com
 */

#ifndef HELPER_MACROS_H__
#define HELPER_MACROS_H__

/**
 * \addtogroup CommonTools
 *
 * @{
 *
 * \file
 * Collection of C helper functions and macros.
 **/

/***************************************************************************
 *
 * Helper and bit manipulation macros
 *
 ***************************************************************************/

/**
 * \addtogroup helpers_bitmanipulation Bit-manipulation macros
 * @{
 **/

/** Bit vector macro **/
#ifndef _BV
#define _BV(i) (1 << (i))
#endif

/** 32 bit bit accessing macro. **/
#define _BV32(i) (uint32_t)((uint32_t)(1UL << i))

/** Bit-level access and test macros. **/
#define SET_BIT(port, bit)   ((port) |= (1 << (bit)))
#define CLEAR_BIT(port, bit) ((port) &= ~(1 << (bit)))
#define TOGGLE_BIT(port, bit) ((port) ^= (1 << (bit)))
#define IS_BIT_SET(port, bit) (((port) & (1 << (bit))) ? 1 : 0)
#define IS_BIT_CLEAR(port, bit) (((port) & (1 << (bit))) == 0 ? 1 : 0)

/** 8-bit macros. **/
#define SET_BIT8(port, bit)   ((port) |= (uint8_t)(1 << (bit)))
#define CLEAR_BIT8(port, bit) ((port) &= (uint8_t)~(1 << (bit)))
#define IS_BIT_SET8(port, bit) (((port) & (uint8_t)(1 << (bit))) ? 1 : 0)
#define IS_BIT_CLEAR8(port, bit) (((port) & (uint8_t)(1 << (bit))) == 0 ? 1 : 0)

/** 32-bit macros. **/
#define SET_BIT32(port, bit)   ((port) |= (uint32_t)(1UL << (bit)))
#define CLEAR_BIT32(port, bit) ((port) &= (uint32_t)~(1UL << (bit)))
#define TOGGLE_BIT32(port, bit) ((port) ^= (uint32_t)(1UL << (bit)))
#define IS_BIT_SET32(port, bit) (((port) & (uint32_t)(1UL << (bit))) ? 1 : 0)
#define IS_BIT_CLEAR32(port, bit) (((port) & (uint32_t)(1UL << (bit))) == 0 ? 1 : 0)

/** 64-bit macros. **/
#define SET_BIT64(port, bit)   ((port) |= (uint64_t)(1ULL << (bit)))
#define CLEAR_BIT64(port, bit) ((port) &= (uint64_t)~(1ULL << (bit)))
#define TOGGLE_BIT64(port, bit) ((port) ^= (uint64_t)(1ULL << (bit)))
#define IS_BIT_SET64(port, bit) (((port) & (uint64_t)(1ULL << (bit))) ? 1 : 0)
#define IS_BIT_CLEAR64(port, bit) (((port) & (uint64_t)(1ULL << (bit))) == 0 ? 1 : 0)

/** @} */

/**
 * \addtogroup helpers_common_symbols Commonly used symbols and macros
 * @{
 **/

/** Define commonly used C symbols. **/
#ifndef NULL
#define NULL 0
#endif
#define FALSE 0
#define TRUE 1

/** Number of elements in a static array. **/
#define countof(arr) ((sizeof(arr)/sizeof(arr[0])))
  
/** Compile-time assertion in C. **/
#define C_ASSERT(e) extern char __C_ASSERT__[(e)?1:-1]

/** Compress a C structure to byte aligned boundaries. **/
#define PACKED __attribute__ ((packed))

/** @} **/

/**
 * \addtogroup helpers_math Math macros
 * @{
 **/

/** Get the minimum of x and y. **/
#define MIN(x, y) ((x) > (y) ? (y) : (x))
/** Get the maximum of x and y. **/
#define MAX(x, y) ((x) > (y) ? (x) : (y))
/** Get the absolute value of x. **/
#define ABS(x) ((x) < 0 ? -(x) : (x))

#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#ifndef HOST_MIDIDUINO
#define round(x)     ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
#endif

/** @} **/

/***************************************************************************
 *
 * Locking macros
 *
 ***************************************************************************/

/**
 * \addtogroup helpers_locking "Locking" and AVR-specific functions
 * @{
 **/
	
#ifdef AVR
#include <avr/pgmspace.h>
#else
/** Host-side empty definitions for access to program space. **/
#define PSTR(s) (s)
#define PGM_P const char *
#define pgm_read_byte(a) (uint8_t)((*a))
#endif

#ifdef AVR
#include <avr/interrupt.h>

/**
 * Declaration of temporary variable to store interrupt status.
 * Use this before using SET_LOCK() and CLEAR_LOCK().
 **/
#define USE_LOCK()   uint8_t _irqlock_tmp

/** Set a "lock" by disabling the interrupts. **/
#define SET_LOCK()   _irqlock_tmp = SREG; cli()
/** Clear the "lock" by restoring the interrupt status. **/
#define CLEAR_LOCK() SREG = _irqlock_tmp
#else

/** On the host, use empty routines to store interrupt status. **/
#define USE_LOCK()   
#define SET_LOCK()   
#define CLEAR_LOCK()
	
#endif

/** @} **/

/***************************************************************************
 *
 * Delay macros
 *
 ***************************************************************************/

/** \category Delay functions. **/

#ifdef MIDIDUINO
#include <util/delay.h>
/** Use the arduino convention to delay inside the code. **/
#define delay(ms) _delay_ms(ms)
#define delayMicroseconds(us) _delay_us(us)
#endif

#ifdef HOST_MIDIDUINO
/** Use empty definitions for now on the host side XXX */
#define delay(ms)
#define delayMicroseconds(us)
#endif

/** @} **/

#endif /* HELPER_MACROS_H__ */
