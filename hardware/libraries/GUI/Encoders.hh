#ifndef ENCODERS_H__
#define ENCODERS_H__

#include <inttypes.h>
#include <MidiClock.h>
#include <Midi.h>
#include "WProgram.h"
#include "GUI.h"

class Encoder;

typedef void (*encoder_handle_t)(Encoder *enc);

void CCEncoderHandle(Encoder *enc);
void TempoEncoderHandle(Encoder *enc);

class Encoder {
 protected:
  int old, cur;
  char name[4];

 public:
  encoder_handle_t handler;
  
  Encoder(char *_name = NULL, encoder_handle_t _handler = NULL);
  void clear();

  virtual char *getName() { return name; }
  virtual void setName(char *_name);
  
  bool redisplay;
  virtual int update(encoder_t *enc);
  virtual void checkHandle();
  virtual bool hasChanged();
  
  virtual int getValue() { return cur; }
  virtual int getOldValue() { return old; }
  virtual void setValue(int value, bool handle = false);

  virtual void displayAt(int i);
};

class RangeEncoder : public Encoder {
 public:
  int min;
  int max;

  RangeEncoder(int _max = 127, int _min = 0, char *_name = NULL, int init = 0,
	       encoder_handle_t _handler = NULL) : Encoder(_name, _handler) {
    if (_min > _max) {
      min = _max;
      max = _min;
    } else {
      min = _min;
      max = _max;
    }
    setValue(init);
  }
  virtual int update(encoder_t *enc);
};

class EnumEncoder : public RangeEncoder {
public:
  char **enumStrings;
  int cnt;

  EnumEncoder(char *strings[], int _cnt, char *_name = NULL, int init = 0,
	      encoder_handle_t _handler = NULL) :
    RangeEncoder(_cnt - 1, 0, _name, init, _handler) {
    enumStrings = strings;
    cnt = _cnt;
  }

  virtual void displayAt(int i);
};

class CCEncoder : public RangeEncoder {
 public:
  int cc;
  int channel;

 CCEncoder(int _cc = 0, int _channel = 0, char *_name = NULL, int init = 0) :
   RangeEncoder(127, 0, _name, init) {
    cc = _cc;
    channel = _channel;
    handler = CCEncoderHandle;
  }
};

class TempoEncoder : public RangeEncoder {
  public:
  TempoEncoder(char *_name = NULL) : RangeEncoder(255, 20, _name) {
    handler = TempoEncoderHandle;
  }
};

class CharEncoder : public RangeEncoder {
public:
  CharEncoder();
  char getChar();
  void setChar(char c);
};

#include "RecordingEncoder.hh"


#endif /* ENCODERS_H__ */
