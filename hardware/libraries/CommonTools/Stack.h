#ifndef STACK_H__
#define STACK_H__

#include <WProgram.h>
#include <inttypes.h>

// running stack

template <class T, int N>
class Stack {
 public:
  volatile uint8_t wr, start;
  T buf[N];
  
  Stack();
  bool push(T *t);
  bool push(T t);
  bool pop(T *t);
  bool peek(T *t);
  bool isEmpty();
  bool isFull();
  void reset();
  uint8_t size();
};

#define STACK_INC(x) (uint8_t)(((x) + 1) % N)
#define STACK_DEC(x) (uint8_t)(((x) == 0) ? (N - 1) : (x) - 1)
#define STACK_START() (uint8_t)((wr  + start) % N)
#define STACK_LAST() (uint8_t)((STACK_DEC(wr)  + start) % N)

template <class T, int N>
Stack<T,N>::Stack() {
  wr = 0;
  start = 0;
}

template <class T, int N>
void Stack<T,N>::reset() {
  uint8_t tmp = SREG;
  cli();
  wr = start = 0;
  SREG = tmp;
}

template <class T, int N>
bool Stack<T,N>::push(T *t) {
  uint8_t tmp = SREG;
  cli();
  if (isFull()) {
    start = STACK_INC(start);
  }
  m_memcpy(&buf[STACK_START()], t, sizeof(T));
  wr = STACK_INC(wr);
  SREG = tmp;
  return true;
}

template <class T, int N>
bool Stack<T,N>::push(T t) {
  return push(&t);
}

template <class T, int N>
bool Stack<T,N>::pop(T *t) {
  uint8_t tmp = SREG;
  cli();
  bool ret = peek(t);
  if (ret)
    wr = STACK_DEC(wr);
  SREG = tmp;
  return ret;
}

template <class T, int N>
bool Stack<T,N>::peek(T *t) {
  uint8_t tmp = SREG;
  cli();
  if (isEmpty()) {
    SREG = tmp;
    return false;
  }
  m_memcpy(t, &buf[STACK_LAST()], sizeof(T));
  SREG = tmp;
  return true;
}

template <class T, int N>
bool Stack<T,N>::isEmpty() {
  uint8_t tmp = SREG;
  cli();
  bool ret = (wr == start);
  SREG = tmp;
  return ret;
}

template <class T, int N>
uint8_t Stack<T, N>::size() {
  uint8_t tmp = SREG;
  cli();
  uint8_t ret = 0;
  if (start > wr) {
    ret = N - start + wr;
  } else {
    ret = wr - start;
  }
  SREG = tmp;
  return ret;
}

template <class T, int N>
bool Stack<T,N>::isFull() {
  uint8_t tmp = SREG;
  cli();
  bool ret = (size() == (N - 1));
  SREG = tmp;
  return ret;
}

#endif /* STACK_H__ */
