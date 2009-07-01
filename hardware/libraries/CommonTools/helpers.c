#ifdef AVR
#include "WProgram.h"
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#endif

#include "helpers.h"

void m_memcpy(void *dst, void *src, uint16_t cnt) {
  while (cnt) {
    *((uint8_t *)dst++) = *((uint8_t *)src++);
    cnt--;
  }
}

void m_memcpy_p(void *dst, PGM_P src, uint16_t cnt) {
  while (cnt) {
    *((uint8_t *)dst++) = pgm_read_byte(src);
    src++;
    cnt--;
  }
}


void m_strncpy(void *dst, const char *src, uint16_t cnt) {
  while (cnt && *src) {
    *((uint8_t *)dst++) = *((uint8_t *)src++);
    cnt--;
  }
  if (cnt > 0) {
    *((uint8_t *)dst++) = 0;
  }
}

void m_strncpy_fill(void *dst, const char *src, uint16_t cnt) {
  while (cnt && *src) {
    *((uint8_t *)dst++) = *((uint8_t *)src++);
    cnt--;
  }
  while (cnt > 1) {
    cnt--;
    *((uint8_t *)dst++) = ' ';
  }
  if (cnt > 0)
    *((uint8_t *)dst++) = 0;
}

void m_strncpy_p(void *dst, PGM_P src, uint16_t cnt) {
  while (cnt) {
    char byte = pgm_read_byte(src);
    if (byte == 0)
      break;
    *((uint8_t *)dst++) = byte;
    src++;
    cnt--;
  }
  if (cnt > 0) {
    *((uint8_t *)dst++) = 0;
  }
}

void m_strncpy_p_fill(void *dst, PGM_P src, uint16_t cnt) {
  while (cnt) {
    char byte = pgm_read_byte(src);
    if (byte == 0)
      break;
    *((uint8_t *)dst++) = byte;
    src++;
    cnt--;
  }
  while (cnt > 1) {
    *((uint8_t *)dst++) = ' ';
    cnt--;
  }
  if (cnt > 0)
    *((uint8_t *)dst++) = 0;
}

void m_memclr(void *dst, uint16_t cnt) {
  while (cnt) {
    *((uint8_t *)dst++) = 0;
    cnt--;
  }
}

void m_str16cpy_fill(void *dst, const char *src) {
  m_strncpy_fill(dst, src, 16);
}

void m_str16cpy_p_fill(void *dst, PGM_P src) {
  m_strncpy_p_fill(dst, src, 16);
}

void m_str16cpy_p(void *dst, PGM_P src) {
  m_strncpy_p(dst, src, 16);
}

void m_strnappend(void *dst, const char *src, int len) {
  char *ptr = dst;
  int i;
  for (i = 0; i < len; i++) {
    if (ptr[0] == '\0')
      break;
    ptr++;
  }
  m_strncpy(ptr, src, len - i);
}

volatile uint16_t slowclock = 0;
volatile uint16_t clock = 0;

uint16_t read_clock(void) {
  USE_LOCK();
  SET_LOCK();
  uint16_t ret = clock;
  CLEAR_LOCK();
  return ret;
}

uint16_t read_slowclock(void) {
  USE_LOCK();
  SET_LOCK();
  uint16_t ret = slowclock;
  CLEAR_LOCK();
  return ret;
}

uint16_t clock_diff(uint16_t old_clock, uint16_t new_clock) {
  if (new_clock >= old_clock)
    return new_clock - old_clock;
  else
    return new_clock + (65535 - old_clock);
}

#ifdef MIDIDUINO

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void delayMicroseconds(unsigned int us) {
  _delay_us(us);
}

void delay(unsigned int ms) {
  _delay_ms(ms);
}
#endif

uint8_t u_limit_value(uint8_t value, int8_t encoder, uint8_t min, uint8_t max) {
  int16_t result = (int16_t)value + encoder;
  if (result < ((int16_t)min)) {
    return min;
  } else if (result > ((int16_t)max)) {
    return max;
  } else {
    return result;
  }
}

int limit_value(int value, int encoder, int min, int max) {
  int result = value + encoder;
  if (result < min) {
    return min;
  } else if (result > max) {
    return max;
  } else {
    return result;
  }
}
