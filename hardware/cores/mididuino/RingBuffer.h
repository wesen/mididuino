#ifndef RINGBUFFER_H__
#define RINGBUFFER_H__

#include <WProgram.h>
#include <inttypes.h>

template <int N>
class RingBuffer {
  volatile uint8_t rd, wr;
  volatile uint8_t overflow;
  volatile uint8_t buf[N];
  
 public:
  RingBuffer();
  bool put(uint8_t c) volatile;
  uint8_t get() volatile;
  bool isEmpty() volatile;
  bool isFull() volatile;
};

#define RB_INC(x) (uint8_t)(((x) + 1) % N)

template <int N>
RingBuffer<N>::RingBuffer() {
  rd = 0;
  wr = 0;
  overflow = 0;
}

template <int N>
bool RingBuffer<N>::put(uint8_t c) volatile {
  if (isFull()) {
    overflow++;
    return false;
  }
  buf[wr] = c;
  wr = RB_INC(wr);
  return true;
}

template <int N>
uint8_t RingBuffer<N>::get() volatile {
  if (isEmpty())
    return 0;
  uint8_t ret = buf[rd];
  rd = RB_INC(rd);
  return ret;
}

template <int N>
bool RingBuffer<N>::isEmpty() volatile {
  return (rd == wr);
}

template <int N>
bool RingBuffer<N>::isFull() volatile {
  return (RB_INC(wr) == rd);
}

#endif /* RINGBUFFER_H__ */
