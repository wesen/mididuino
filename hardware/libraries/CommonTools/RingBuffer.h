#ifndef RINGBUFFER_H__
#define RINGBUFFER_H__

#include <inttypes.h>
#include <avr/interrupt.h>

template <class C, int N, class T = uint8_t>
class CRingBuffer {
  volatile T rd, wr;
  volatile C buf[N];
  
 public:
  volatile uint8_t overflow;

  CRingBuffer();
  bool put(C c) volatile;
  bool putp(C *c) volatile;
  C get() volatile;
  bool getp(C *dst) volatile;
  C peek() volatile;
  bool isEmpty() volatile;
  bool isFull() volatile;
};

template <int N, class T = uint8_t>
  class RingBuffer : public CRingBuffer<uint8_t, N, T> {
 public:
  
  RingBuffer() {
  };
};

#define RB_INC(x) (T)(((x) + 1) % N)

template <class C, int N, class T>
  CRingBuffer<C, N, T>::CRingBuffer() {
  rd = 0;
  wr = 0;
  overflow = 0;
}

template <class C, int N, class T >
  bool CRingBuffer<C, N, T>::put(C c) volatile {
  if (isFull()) {
    overflow++;
    return false;
  }
  buf[wr] = c;
  wr = RB_INC(wr);
  return true;
}

template <class C, int N, class T>
  bool CRingBuffer<C, N, T>::putp(C *c) volatile {
  if (isFull()) {
    overflow++;
    return false;
  }
  m_memcpy((void *)&buf[wr], (void *)c, sizeof(*c));
  wr = RB_INC(wr);
  return true;
}


template <class C, int N, class T>
  C CRingBuffer<C, N, T>::get() volatile {
  if (isEmpty())
    return 0;
  C ret = buf[rd];
  rd = RB_INC(rd);
  return ret;
}

template <class C, int N, class T>
  bool CRingBuffer<C, N, T>::getp(C *dst) volatile {
  if (isEmpty())
    return false;
  m_memcpy(dst, (void *)&buf[rd], sizeof(C));
  rd = RB_INC(rd);
  return true;
}

template <class C, int N, class T>
  C CRingBuffer<C, N, T>::peek() volatile {
  if (isEmpty())
    return (C)0;
  else return buf[rd];
}

template <class C, int N, class T>
  bool CRingBuffer<C, N, T>::isEmpty() volatile {
  uint8_t tmp = SREG;
  cli();
  bool ret = (rd == wr);
  SREG = tmp;
  return ret;
}

template <class C, int N, class T>
  bool CRingBuffer<C, N, T>::isFull() volatile {
  uint8_t tmp = SREG;
  cli();
  bool ret = (RB_INC(wr) == rd);
  SREG = tmp;
  return ret;
}

#endif /* RINGBUFFER_H__ */
