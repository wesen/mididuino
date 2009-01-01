#ifndef ENCODERPAGE_H__
#define ENCODERPAGE_H__

#include <stdlib.h>

#include "WProgram.h"

class Encoder {
 protected:
  uint8_t old, cur;

 public:
  Encoder();
  void clear();
  virtual void update(encoder_t *enc);
  virtual void handle(uint8_t val);
  void checkHandle() {
    if (cur != old) {
      handle(cur);
    }
    
    old = cur;
  }
  bool hasChanged() {
    return old != cur;
  }
  uint8_t getValue() {
    return old;
  }
  uint8_t getCurValue() {
    return cur;
  }
  void setValue(uint8_t value) {
    old = cur = value;
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

class CCEncoder : public RangeEncoder {
 public:
  uint8_t cc;

 CCEncoder(uint8_t _cc) : RangeEncoder(127, 0) {
    cc = _cc;
  }
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
  void update();
  void clear();
  void display();
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
