#ifndef HELPERS_H__
#define HELPERS_H__

#include <inttypes.h>
#include <avr/pgmspace.h>

void m_memcpy(void *dst, void *src, uint8_t cnt);
void m_strncpy(void *dst, char *src, uint8_t cnt);
void m_strncpy_fill(void *dst, char *src, uint8_t cnt);
void m_memcpy_p(void *dst, PGM_P src, uint8_t cnt);
void m_strncpy_p(void *dst, PGM_P src, uint8_t cnt);
void m_strncpy_p_fill(void *dst, PGM_P src, uint8_t cnt);
void m_memclr(void *dst, uint8_t cnt);
void m_str16cpy_fill(void *dst, char *src);
void m_str16cpy_p_fill(void *dst, PGM_P src);
void m_str16cpy_p(void *dst, PGM_P src);


#endif /* HELPERS_H__ */
