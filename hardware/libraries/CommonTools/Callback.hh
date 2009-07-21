#ifndef CALLBACK_H__
#define CALLBACK_H__

#include <inttypes.h>
#include "Vector.hh"

class Callback {
public:
  virtual void onCallback(uint16_t type, void *obj) = 0;

#ifdef HOST_MIDIDUINO
  virtual ~Callback() { }
#endif


};

typedef void (*callback_func_t)(void *obj);
template <uint8_t CLASSID, uint8_t CBID, int N> class CallbackHandler {
public:
  const static uint16_t id = ((uint16_t)CLASSID << 8) | CBID;

  Vector<Callback *, N> callbacks;
  Vector<callback_func_t, N> callbackFuncs;

  CallbackHandler() {
  }

  void add(Callback *obj) {
    callbacks.add(obj);
  }

  void remove(Callback *obj) {
    callbacks.remove(obj);
  }

  void callback(void *data = NULL) {
    for (int i = 0; i < callbacks.size; i++) {
      if (callbacks.arr[i] != NULL) {
	callbacks.arr[i]->onCallback(id, data);
      }
      if (callbackFuncs.arr[i] != NULL) {
	callbackFuncs.arr[i](data);
      }
    }

  }
};

#endif /* CALLBACK_H__ */
