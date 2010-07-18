#ifndef FORMATTING_RINGBUFFER_H__
#define FORMATTING_RINGBUFFER_H__

#include "WProgram.h"
#include "RingBuffer.h"

template <int N, class T = uint8_t> class FormattingRingBuffer :
	public CRingBuffer<char, N, T> {
 public:
 FormattingRingBuffer() {
 }

 bool puts(char *ptr) {
	 while (*ptr) {
		 if (!put(*ptr)) {
			 return false;
		 }
		 ptr++;
	 }
	 return true;
 }

 bool puts(char *ptr, uint8_t len) {
	 while (*ptr && (len > 0)) {
		 if (!put(*ptr)) {
			 return false;
		 }
		 ptr++;
		 len--;
	 }
	 return true;
 }

 bool vprintf(const char *fmt, va_list lp) {
	 char buf[32];
	 m_vsnprintf(buf, sizeof(buf) - 1, fmt, lp);
	 return puts(buf);
 }

 bool printf(const char *fmt, ...) {
	 va_list lp;
	 va_start(lp, fmt);
	 bool ret = vprintf(fmt, lp);
	 va_end(lp);
	 return ret;
 }
};

#endif /* FORMATTING_RINGBUFFER_H__ */
