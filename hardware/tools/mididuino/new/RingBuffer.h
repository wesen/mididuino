#ifndef RINGBUFFER_H__
#define RINGBUFFER_H__

#include <inttypes.h>

template <class C, int N>
class RingBuffer {
  volatile uint8_t rd, wr;
  volatile C buf[N];
  
 public:
  volatile uint8_t overflow;

  RingBuffer();
  bool put(C c) volatile;
  C get() volatile;
  C peek() volatile;
  bool isEmpty() volatile;
  bool isFull() volatile;
};

#define RB_INC(x) (uint8_t)(((x) + 1) % N)

template <class C, int N>
  RingBuffer<C,N>::RingBuffer() {
  rd = 0;
  wr = 0;
  overflow = 0;
}

template <class C, int N>
bool RingBuffer<C, N>::put(C c) volatile {
  if (isFull()) {
    overflow++;
    return false;
  }
  buf[wr] = c;
  wr = RB_INC(wr);
  return true;
}

template <class C, int N>
C RingBuffer<C, N>::get() volatile {
  if (isEmpty())
    return 0;
  C ret = buf[rd];
  rd = RB_INC(rd);
  return ret;
}

template <class C, int N>
C RingBuffer<C, N>::peek() volatile {
  if (isEmpty())
    return 0;
  else return buf[rd];
}

template <class C, int N>
bool RingBuffer<C, N>::isEmpty() volatile {
  uint8_t ret = (rd == wr);
  return ret;
}

template <class C, int N>
bool RingBuffer<C, N>::isFull() volatile {
  uint8_t ret = (RB_INC(wr) == rd);
  return ret;
}

#endif /* RINGBUFFER_H__ */
