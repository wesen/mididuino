#include "helpers.h"

#include <avr/pgmspace.h>

void m_memcpy(void *dst, void *src, uint8_t cnt) {
  while (cnt--) {
    *((uint8_t *)dst++) = *((uint8_t *)src++);
  }
}

void m_memcpy_p(void *dst, PGM_P src, uint8_t cnt) {
  while (cnt--) {
    *((uint8_t *)dst++) = pgm_read_byte(src);
    src++;
  }
}


void m_strncpy(void *dst, char *src, uint8_t cnt) {
  while (cnt-- && *src) {
    *((uint8_t *)dst++) = *((uint8_t *)src++);
  }
}

void m_strncpy_fill(void *dst, char *src, uint8_t cnt) {
  while (cnt && *src) {
    *((uint8_t *)dst++) = *((uint8_t *)src++);
    cnt--;
  }
  while (cnt--) {
    *((uint8_t *)dst++) = ' ';
  }
}

void m_strncpy_p(void *dst, PGM_P src, uint8_t cnt) {
  while (cnt--) {
    char byte = pgm_read_byte(src);
    if (byte == 0)
      break;
    *((uint8_t *)dst++) = byte;
    src++;
  }
}

void m_strncpy_p_fill(void *dst, PGM_P src, uint8_t cnt) {
  while (cnt) {
    char byte = pgm_read_byte(src);
    if (byte == 0)
      break;
    *((uint8_t *)dst++) = byte;
    src++;
    cnt--;
  }
  while (cnt--) {
    *((uint8_t *)dst++) = ' ';
  }
}

void m_memclr(void *dst, uint8_t cnt) {
  while (cnt--)
    *((uint8_t *)dst++) = 0;
}

void m_str16cpy_fill(void *dst, char *src) {
  m_strncpy_fill(dst, src, 16);
}

void m_str16cpy_p_fill(void *dst, PGM_P src) {
  m_strncpy_p_fill(dst, src, 16);
}

void m_str16cpy_p(void *dst, PGM_P src) {
  m_strncpy_p(dst, src, 16);
}

