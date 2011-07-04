/* -*- mode: c++ -*- */

/*
 * MidiCtrl - Various helper methods and macros
 *
 * (c) 2009 - 2011 - Manuel Odendahl - wesen@ruinwesen.com
 */

/**
 * \addtogroup CommonTools
 *
 * @{
 *
 * \file
 * Collection of C helper functions and macros.
 **/

#ifndef HELPERS_H__
#define HELPERS_H__

#include "PlatformConfig.h"
#include "HelperMacros.h"

#include <stdarg.h>

#ifdef __cplusplus

extern "C" {
#endif

  /**
   * \addtogroup helpers_bitmanipulation Bit-manipulation macros
   * @{
   **/

  long map(long x, long in_min, long in_max, long out_min, long out_max);

  uint8_t u_limit_value(uint8_t value, int8_t encoder, uint8_t min, uint8_t max);
  int limit_value(int value, int encoder, int min, int max);
  uint8_t interpolate_8(uint8_t start, uint8_t end, uint8_t amount);


  /** @} **/

  /** \addtogroup helpers_string String manipulation routines
   * @{
   **/
	
  uint16_t m_snprintf(char *dst, uint16_t len, const char *fmt, ...);
  uint16_t m_vsnprintf(char *dst, uint16_t len, const char *fmt, va_list lp);
	
  void m_memcpy(void *dst, const void *src, uint16_t cnt);
  void m_memset(void *dst, uint16_t cnt, uint8_t elt);
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
  uint16_t m_strlen(const char *str);

  /** @} */

  /**
   * \addtogroup helpers_clock Timing functions
   * @{
   **/
			
  extern uint16_t read_clock(void);
  extern uint16_t read_slowclock(void);
  uint16_t clock_diff(uint16_t old_clock, uint16_t new_clock);

#ifdef HOST_MIDIDUINO
#else
  extern volatile uint16_t clock;
  extern volatile uint16_t slowclock;
#endif

  /** @} **/

  /**
   * \addtogroup helpers_debug Debugging functions
   * @{
   **/
			
#ifdef HOST_MIDIDUINO
  void hexdump(uint8_t *data, uint16_t len);
#endif

  /**
   * @}
   */
	
#ifdef __cplusplus
}

#include "BitArray.hh"
#include "Task.hh"
#endif

#endif /* HELPERS_H__ */
