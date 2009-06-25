#ifndef CIRCULAR_H__
#define CIRCULAR_H__

#include <inttypes.h>

template <class C, int N>
class CircularBuffer {
public:
  volatile uint8_t start, count;
  C buf[N];

  CircularBuffer() {
    start = count = 0;
  }

  void clear() {
    start = count = 0;
  }

  void put(C c) {
    buf[start] = c;
    count = MIN(N, count + 1);
    start = (start + 1) % N;
  }
  void putp(C *c) {
    m_memcpy((void *)&buf[(start + count) % N], c, sizeof(C));
    count++;
    if (count > N) {
      start = (start + 1) % N;
      count = N;
    }
  }
  uint8_t size() {
    return count;
  }
  C get(uint8_t i) {
    return buf[(start + i) % N];
  }
  C* getp(uint8_t i) {
    if (count > i)
      return &buf[(start + i) % N];
    else
      return NULL;
  }
  bool getCopy(uint8_t i, C *c) {
    if (count > i) {
      m_memcpy(c, (void *)&buf[(start + i) % N], sizeof(C));
      return true;
    } else {
      return false;
    }
  }
  C getLast() {
    return get(count);
  }
  C* getLastp() {
    return getp(count);
  }
  bool getLastCopy(C *c) {
    return getCopy(count);
  }
      
};

#endif /* CIRCULAR_H__ */
