#ifndef RINGBUFFER_H__
#define RINGBUFFER_H__

#include <WProgram.h>
#include <inttypes.h>

template <int N>
class RingBuffer {
  volatile uint8_t rd, wr;
  volatile uint8_t buf[N];
  
 public:
  volatile uint8_t overflow;

  RingBuffer();
  bool put(uint8_t c) volatile;
  uint8_t get() volatile;
  uint8_t peek() volatile;
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
uint8_t RingBuffer<N>::peek() volatile {
  if (isEmpty())
    return 0;
  else return buf[rd];
}

template <int N>
bool RingBuffer<N>::isEmpty() volatile {
  uint8_t tmp = SREG;
  cli();
  uint8_t ret = (rd == wr);
  SREG = tmp;
  return ret;
}

template <int N>
bool RingBuffer<N>::isFull() volatile {
  uint8_t tmp = SREG;
  cli();
  uint8_t ret = (RB_INC(wr) == rd);
  SREG = tmp;
  return ret;
}

#endif /* RINGBUFFER_H__ */
