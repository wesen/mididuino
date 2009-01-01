#ifndef ENCODERPAGE_H__
#define ENCODERPAGE_H__

#include <stdlib.h>

#include "WProgram.h"
#include "MidiUart.h"

class Encoder {
 protected:
  uint8_t old, cur;

 public:
  Encoder();
  void clear();
  virtual void update(encoder_t *enc);
  virtual void handle(uint8_t val);
  void checkHandle();
  bool hasChanged() {
    return old != cur;
  }
  uint8_t getValue() {
    return cur;
  }
  uint8_t getOldValue() {
    return old;
  }
  void setValue(uint8_t value) {
    cur = value;
  }
};

class RangeEncoder : public Encoder {
 public:
  uint8_t min;
  uint8_t max;

 RangeEncoder(uint8_t _max = 127, uint8_t _min = 0) : Encoder() {
    if (_min > _max) {
      min = _max;
      max = _min;
    } else {
      min = _min;
      max = _max;
    }
  }
  void handle(uint8_t val) { }
  void update(encoder_t *enc);
};

class MidiUartParentClass;
class MidiUartClass;
extern MidiUartClass MidiUart;

class CCEncoder : public RangeEncoder {
 public:
  uint8_t cc;
  MidiUartParentClass *uart;

  CCEncoder(uint8_t _cc);
  void handle(uint8_t val);
};

class EncoderPage {
 public:
  Encoder *encoders[NUM_ENCODERS];
  
  EncoderPage() {
    for (uint8_t i; i < NUM_ENCODERS; i++) {
      encoders[i] = NULL;
    }
  }
  void handle();
  virtual void update();
  void clear();
  virtual void display(bool redisplay = false);
};

#define INIT_PAGE(page, encoders, size) \
  { for (uint8_t i = 0; i < (size); i++) { (page).encoders[i] = (encoders) + i; } }

class TempoEncoder : public RangeEncoder {
  public:
  TempoEncoder() : RangeEncoder(255, 20) {
  }
  
  void handle(uint8_t val);
};


#endif /* ENCODERPAGE_H__ */
