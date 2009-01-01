#ifndef LFO_H__
#define LFO_H__

#include <inttypes.h>

typedef enum {
  LFO_NONE = 0,
  LFO_TRIANGLE,
  LFO_SQUARE,
  LFO_RANDOM
} lfo_type_t;

class LFOClass {
  uint8_t oldacc;
  uint8_t oldval;

 public:
  LFOClass();

  lfo_type_t type;
  
  uint32_t acc;
  uint32_t inc;

  uint8_t curValue;
  uint8_t oldValue;
  
  uint8_t getValue();
};

extern LFOClass LFO[4];

#endif /* LFO_H__ */
