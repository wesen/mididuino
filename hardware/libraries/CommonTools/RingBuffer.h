#ifndef RINGBUFFER_H__
#define RINGBUFFER_H__

#include <inttypes.h>
#include <avr/interrupt.h>

template <class C, int N>
class CRingBuffer {
  volatile uint8_t rd, wr;
  volatile C buf[N];
  
 public:
  volatile uint8_t overflow;

  CRingBuffer();
  bool put(C c) volatile;
  bool putp(C *c) volatile;
  C get() volatile;
  bool getp(C *dst);
  C peek() volatile;
  bool isEmpty() volatile;
  bool isFull() volatile;
};

template <int N>
class RingBuffer : public CRingBuffer<uint8_t, N> {
 public:
  
  RingBuffer() {
  };
};

#define RB_INC(x) (uint8_t)(((x) + 1) % N)

template <class C, int N>
  CRingBuffer<C, N>::CRingBuffer() {
  rd = 0;
  wr = 0;
  overflow = 0;
}

template <class C, int N>
  bool CRingBuffer<C, N>::put(C c) volatile {
  if (isFull()) {
    overflow++;
    return false;
  }
  buf[wr] = c;
  wr = RB_INC(wr);
  return true;
}

template <class C, int N>
  bool CRingBuffer<C, N>::putp(C *c) volatile {
  if (isFull()) {
    overflow++;
    return false;
  }
  m_memcpy((void *)&buf[wr], (void *)c, sizeof(*c));
  wr = RB_INC(wr);
  return true;
}


template <class C, int N>
  C CRingBuffer<C, N>::get() volatile {
  if (isEmpty())
    return 0;
  C ret = buf[rd];
  rd = RB_INC(rd);
  return ret;
}

template <class C, int N>
  bool CRingBuffer<C, N>::getp(C *dst) {
  if (isEmpty())
    return false;
  m_memcpy(dst, (void *)&buf[rd], sizeof(C));
  rd = RB_INC(rd);
  return true;
}

template <class C, int N>
  C CRingBuffer<C, N>::peek() volatile {
  if (isEmpty())
    return (C)0;
  else return buf[rd];
}

template <class C, int N>
  bool CRingBuffer<C, N>::isEmpty() volatile {
  uint8_t tmp = SREG;
  cli();
  uint8_t ret = (rd == wr);
  SREG = tmp;
  return ret;
}

template <class C, int N>
  bool CRingBuffer<C, N>::isFull() volatile {
  uint8_t tmp = SREG;
  cli();
  uint8_t ret = (RB_INC(wr) == rd);
  SREG = tmp;
  return ret;
}

#endif /* RINGBUFFER_H__ */
