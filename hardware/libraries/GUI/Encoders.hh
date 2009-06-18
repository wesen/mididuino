#ifndef ENCODERS_H__
#define ENCODERS_H__

#include <inttypes.h>
#include "WProgram.h"
#include "GUI.h"

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
  void setValue(int value, bool handle = false) {
    if (handle) {
      cur = value;
      checkHandle();
    } else {
      old = cur = value;
    }
    redisplay = true;
  }

  virtual void displayAt(int i) {
    GUI.put_value(i, getValue());
    redisplay = false;
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
  virtual void update(encoder_t *enc);
};

class EnumEncoder : public RangeEncoder {
public:
  char **enumStrings;
  int cnt;

  EnumEncoder(char *strings[], int _cnt, char *_name = NULL, int init = 0) : RangeEncoder(_cnt - 1, 0, _name, init) {
    enumStrings = strings;
    cnt = _cnt;
  }

  virtual void displayAt(int i) {
    GUI.put_string(i, enumStrings[getValue()]);
    redisplay = false;
  }
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
