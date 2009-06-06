#ifndef ENCODERS_H__
#define ENCODERS_H__

#include <inttypes.h>
#include "WProgram.h"

class Encoder {
 protected:
  int old, cur;
  char name[4];

 public:
  Encoder(char *_name = NULL);
  void clear();
  char *getName() { return name; }
  void setName(char *_name) {
    if (_name != NULL)
      m_strncpy_fill(name, _name, 4);
    name[3] = '\0';
  }
  bool redisplay;
  virtual void update(encoder_t *enc);
  virtual void handle(int val);
  void checkHandle() {
    if (cur != old) {
      handle(cur);
    }
    
    old = cur;
  }
  bool hasChanged() {
    return old != cur;
  }
  int getValue() {
    return cur;
  }
  int getOldValue() {
    return old;
  }
  void setValue(int value) {
    old = cur = value;
    redisplay = true;
  }
};

class RangeEncoder : public Encoder {
 public:
  int min;
  int max;

 RangeEncoder(int _max = 127, int _min = 0, char *_name = NULL, int init = 0) : Encoder(_name) {
    if (_min > _max) {
      min = _max;
      max = _min;
    } else {
      min = _min;
      max = _max;
    }
    setValue(init);
  }
  virtual void handle(int val) { }
  void update(encoder_t *enc);
};

class CCEncoder : public RangeEncoder {
 public:
  int cc;
  int channel;

 CCEncoder(int _cc = 0, int _channel = 0, char *_name = NULL, int init = 0) : RangeEncoder(127, 0, name, init) {
    cc = _cc;
    channel = _channel;
  }
  virtual void handle(int val);
};

class TempoEncoder : public RangeEncoder {
  public:
  TempoEncoder() : RangeEncoder(255, 20) {
  }

  void handle(int val);
};

class CharEncoder : public RangeEncoder {
public:
  CharEncoder();
  char getChar();
  void setChar(char c);
};


#endif /* ENCODERS_H__ */
