#ifndef GUI_H__
#define GUI_H__

#include <stdlib.h>

#include "WProgram.h"

class Encoder {
 protected:
  uint8_t old, cur;
  char name[4];

 public:
  Encoder(char *_name = NULL);
  void clear();
  char *getName() { return name; }
  void setName(char *_name) {
    if (_name != NULL)
      m_strncpy_fill(name, _name, 4);
  }
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
    return cur;
  }
  uint8_t getOldValue() {
    return old;
  }
  void setValue(uint8_t value) {
    old = cur = value;
  }
};

class RangeEncoder : public Encoder {
 public:
  uint8_t min;
  uint8_t max;

 RangeEncoder(uint8_t _max = 127, uint8_t _min = 0, char *_name = NULL) : Encoder(_name) {
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

 CCEncoder(uint8_t _cc, char *_name = NULL) : RangeEncoder(127, 0) {
    cc = _cc;
  }
  void handle(uint8_t val);
};

class Page {
 public:
  Page() { }
  virtual void handle() { }
  virtual void update() { }
  virtual void clear()  { }
  virtual void display(bool redisplay = false) { }
};

class EncoderPage : public Page {
 public:
  Encoder *encoders[GUI_NUM_ENCODERS];
  
  EncoderPage() {
    for (uint8_t i; i < GUI_NUM_ENCODERS; i++) {
      encoders[i] = NULL;
    }
  }
  virtual void handle();
  virtual void update();
  virtual void clear();
  virtual void display(bool redisplay = false);
  void displayNames();
};

#define INIT_PAGE(page, encoders, size) \
  { for (uint8_t i = 0; i < (size); i++) { (page).encoders[i] = (encoders) + i; } }

class TempoEncoder : public RangeEncoder {
  public:
  TempoEncoder() : RangeEncoder(255, 20) {
  }
  
  void handle(uint8_t val);
};

typedef struct line_s {
  char data[16];
  bool changed;
} line_t;

class GuiClass {
 protected:
 public:
  line_t lines[2];
  uint8_t curLine;
  
  Page *page;
  Page *newPage;

  GuiClass();
  void update();
  void (*handleButtons)();

  void updatePage();

  static const uint8_t NUM_ENCODERS = GUI_NUM_ENCODERS;
  static const uint8_t NUM_BUTTONS  = GUI_NUM_BUTTONS;
  
  void put_value(uint8_t idx, uint8_t value);
  void put_value16(uint8_t idx, uint16_t value);
  void put_valuex(uint8_t idx, uint8_t value);
  void put_string(uint8_t idx, char *str);
  void put_p_string(uint8_t idx, PGM_P str);
  void put_string(char *str);
  void put_p_string(PGM_P str);
  void put_string_fill(char *str);
  void put_p_string_fill(PGM_P str);
  void put_string_at(uint8_t idx, char *str);
  void put_p_string_at(uint8_t idx, PGM_P str);
  void put_string_at_fill(uint8_t idx, char *str);
  void put_p_string_at_fill(uint8_t idx, PGM_P str);
  void setLine(const uint8_t line) { curLine = line; }
  void clearLine();
  void setPage(Page *_page);
  static const uint8_t LINE1 = 0;
  static const uint8_t LINE2 = 1;
  
};

extern GuiClass GUI;

#endif /* GUI_H__ */
