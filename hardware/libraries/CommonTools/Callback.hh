#ifndef CALLBACK_H__
#define CALLBACK_H__

#include <inttypes.h>
#include "helpers.h"

template <class C, typename M, int N> class CallbackVector {
public:
  struct {
    C* obj;
    M ptr;
  } callbacks[N];

  uint8_t size;

  CallbackVector() {
    CallbackVector<C,M,N>::size = 0;
  }

  bool add(C *obj, M ptr) {
    if (size >= N) {
      return false;
    } else {
      for (uint8_t i = 0; i < size; i++) {
	if (callbacks[i].obj == obj &&
	    callbacks[i].ptr == ptr)
	  return true;
      }
      callbacks[size].obj = obj;
      callbacks[size].ptr = ptr;
      size++;
      return true;
    }
  }

  void remove(C *obj, M ptr) {
    USE_LOCK();
    SET_LOCK();
    for (uint8_t i = 0; i < size; i++) {
      if (callbacks[i].obj == obj &&
	  callbacks[i].ptr == ptr) {
	m_memcpy(callbacks + i, callbacks + i + 1, sizeof(callbacks[0]) * (size - i - 1));
	size--;
	break;
      }
    }
    CLEAR_LOCK();
  }

  void remove(C *obj) {
    USE_LOCK();
    SET_LOCK();
  again:
    for (uint8_t i = 0; i < size; i++) {
      if (callbacks[i].obj == obj) {
	m_memcpy(callbacks + i, callbacks + i + 1, sizeof(callbacks[0]) * (size - i - 1));
	size--;
	goto again;
      }
    }
    CLEAR_LOCK();
  }

  void call() {
    for (uint8_t i = 0; i < size; i++) {
      ((callbacks[i].obj)->*(callbacks[i].ptr))();
    }
  }
};

template <class C, typename M, int N = 4, typename Arg1 = void> class CallbackVector1 :
  public CallbackVector<C, M, N> {
public:
  void call(Arg1 a1) {
    for (uint8_t i = 0; i < CallbackVector<C,M,N>::size; i++) {
      ((CallbackVector<C,M,N>::callbacks[i].obj)->*(CallbackVector<C,M,N>::callbacks[i].ptr))(a1);
    }
  }
};

template <class C, typename M, int N = 4, typename Arg1 = void, typename Arg2 = void>
class CallbackVector2 :
  public CallbackVector<C, M, N> {
public:
  void call(Arg1 a1, Arg2 a2) {
    for (uint8_t i = 0; i < CallbackVector<C,M,N>::size; i++) {
      ((CallbackVector<C,M,N>::callbacks[i].obj)->*(CallbackVector<C,M,N>::callbacks[i].ptr))(a1, a2);
    }
  }
};


#endif /* CALLBACK_H__ */
