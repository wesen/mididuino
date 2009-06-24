#ifndef CIRCULAR_H__
#define CIRCULAR_H__

#include <inttypes.h>

template <class C, int N>
class CircularBuffer {
  volatile uint8_t start, count;
  volatile C buf[N];

public:
  volatile uint8_t overflow;

  CircularBuffer() {
    start = count = 0;
    overflow = 0;
  }

  bool put(C c) {
    buf[start] = c;
    count = MIN(N, count + 1);
    start = (start + 1) % N;
  }
  bool putp(C *c) {
    m_memcpy(&buf[start], c, sizeof(C));
    count = MIN(N, count + 1);
    start = (start + 1) % N;
  }
  uint8_t size() {
    return count;
  }
  C get(uint8_t i) {
    return buf[(start + i) % N];
  }
  void getp(uint8_t i, C *c) {
    m_memcpy(c, &buf[(start + i) % N];
  }
};

#endif /* CIRCULAR_H__ */
