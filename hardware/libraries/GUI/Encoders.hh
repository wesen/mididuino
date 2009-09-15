#ifndef ENCODERS_H__
#define ENCODERS_H__

#include <inttypes.h>
#include <Midi.h>
#include "WProgram.h"
#include "GUI.h"

class Encoder;

typedef void (*encoder_handle_t)(Encoder *enc);

void CCEncoderHandle(Encoder *enc);
void TempoEncoderHandle(Encoder *enc);

class Encoder {
 protected:
 public:
  int old, cur;
  char name[4];
  bool fastmode;
  bool pressmode;

  encoder_handle_t handler;
  
  Encoder(const char *_name = NULL, encoder_handle_t _handler = NULL);
  void clear();

  virtual char *getName() { return name; }
  virtual void setName(const char *_name);
  
  bool redisplay;
  virtual int update(encoder_t *enc);
  virtual void checkHandle();
  virtual bool hasChanged();
  
  virtual int getValue() { return cur; }
  virtual int getOldValue() { return old; }
  virtual void setValue(int value, bool handle = false);

  virtual void displayAt(int i);

#ifdef HOST_MIDIDUINO
  virtual ~Encoder() { }
#endif
};

class RangeEncoder : public Encoder {
 public:
  int min;
  int max;

  RangeEncoder(int _max = 127, int _min = 0, const char *_name = NULL, int init = 0,
	       encoder_handle_t _handler = NULL) : Encoder(_name, _handler) {
    initRangeEncoder(_max, _min, _name, init, _handler);
  }
  void initRangeEncoder(int _max = 128, int _min = 0, const char *_name = NULL, int init = 0,
			encoder_handle_t _handler = NULL) {
    setName(_name);
    handler = _handler;
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
  const char **enumStrings;
  int cnt;

  EnumEncoder(const char *strings[] = NULL, int _cnt = 0, const char *_name = NULL, int init = 0,
	      encoder_handle_t _handler = NULL) :
    RangeEncoder(_cnt - 1, 0, _name, init, _handler) {
    enumStrings = strings;
    cnt = _cnt;
  }

  void initEnumEncoder(const char *strings[], int _cnt, const char *_name = NULL, int init = 0) {
    enumStrings = strings;
    cnt = _cnt;
    min = 0;
    max = _cnt - 1;
    setValue(init);
    setName(_name);
  }

  virtual void displayAt(int i);
};

class PEnumEncoder : public EnumEncoder {
public:
  PEnumEncoder(const char *strings[], int _cnt, const char *_name = NULL, int init = 0,
	      encoder_handle_t _handler = NULL) :
    EnumEncoder(strings, _cnt, _name, init, _handler) {
  }

  virtual void displayAt(int i);
};

static const char *boolEnumStrings[] = { "OFF", "ON" };

class BoolEncoder : public EnumEncoder {
public:
  BoolEncoder(const char *_name = NULL, bool init = false, encoder_handle_t _handler = NULL) :
    EnumEncoder(boolEnumStrings, 2, _name, init ? 1 : 0, _handler) {
  }

  void initBoolEncoder(const char *_name = NULL, bool init = false) {
    initEnumEncoder(boolEnumStrings, 2, _name, init ? 1 : 0);
  }
    

  bool getBoolValue() {
    return getValue() == 1;
  }
};

class MidiTrackEncoder : public RangeEncoder {
public:
  MidiTrackEncoder(char *_name = NULL, uint8_t init = 0) : RangeEncoder(15, 0, _name, init) {
  }

  virtual void displayAt(int i);
};

class CCEncoder : public RangeEncoder {
 public:
  uint8_t cc;
  uint8_t channel;

  virtual uint8_t getCC() {
    return cc;
  }
  virtual uint8_t getChannel() {
    return channel;
  }
  virtual void initCCEncoder(uint8_t _channel, uint8_t _cc) {
    cc = _cc;
    channel = _channel;
  }
  
  CCEncoder(uint8_t _cc = 0, uint8_t _channel = 0, const char *_name = NULL, int init = 0) :
    RangeEncoder(127, 0, _name, init) {
    initCCEncoder(_channel, _cc);
    handler = CCEncoderHandle;
  }
};

char hex2c(uint8_t hex);


class AutoNameCCEncoder : public CCEncoder {
public:
  AutoNameCCEncoder(uint8_t _cc = 0, uint8_t _channel = 0, const char *_name = NULL, int init = 0) :
    CCEncoder(_cc, _channel, _name, init) {
    if (_name == NULL) {
      setCCName();
    }
  }

  void setCCName() {
    char name[4];
    name[0] = hex2c(getChannel());
    uint8_t cc = getCC();
    name[1] = hex2c(cc >> 4);
    name[2] = hex2c(cc & 0xF);
    name[3] = '\0';
    setName(name);
  }

  virtual void initCCEncoder(uint8_t _channel, uint8_t _cc);
};

class TempoEncoder : public RangeEncoder {
  public:
  TempoEncoder(const char *_name = NULL) : RangeEncoder(255, 20, _name) {
    handler = TempoEncoderHandle;
  }
};

class CharEncoder : public RangeEncoder {
public:
  CharEncoder();
  char getChar();
  void setChar(char c);
};

class NotePitchEncoder : public RangeEncoder {
public:
  NotePitchEncoder(char *_name = NULL);

  void displayAt(int i);
};

#include "RecordingEncoder.hh"


#endif /* ENCODERS_H__ */
