#ifdef AVR
#include "WProgram.h"
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#endif

#include "helpers.h"

uint16_t m_strlen(const char *src) {
  uint16_t result = 0;
  while (src[result++] != '\0')
    ;
  return result;
}

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

#ifdef HOST_MIDIDUINO
#include <sys/time.h>
#include <stdio.h>
#include <math.h>

static double startClock;
static uint8_t clockStarted = 0;

uint16_t read_clock(void) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  double clock = (double)(tv.tv_sec + (double)tv.tv_usec / 1000000.0);
  if (!clockStarted) {
    startClock = clock;
    clockStarted = 1;
  }
  clock -= startClock;
  clock *= 61250;
  clock = fmod(clock, 65536);

  return clock;
}

uint16_t read_slowclock(void) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  double clock = (double)(tv.tv_sec + (double)tv.tv_usec / 1000000.0);
  if (!clockStarted) {
    startClock = clock;
    clockStarted = 1;
  }
  clock -= startClock;
  clock *= 976;
  clock = fmod(clock, 65536);
  return clock;
}

#else
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
#endif

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

uint8_t interpolate_8(uint8_t start, uint8_t end, uint8_t amount) {
  int diff = (end - start);
  return start + ((diff * amount) >> 7);
}

#ifdef HOST_MIDIDUINO
void hexdump(uint8_t *data, uint16_t len) {
  uint8_t cnt = 0;
	uint16_t i;
  for (i = 0; i < len; i++) {
		if (cnt == 0) {
			printf("%.4x: ", i);
		}
    printf("%.2x ", data[i]);
    cnt++;
    if (cnt == 8) {
      printf(" ");
    }
    if (cnt == 16) {
      printf("\n");
      cnt = 0;
    }
  }
  if (cnt != 0) {
    printf("\n");
  }
}

#endif
