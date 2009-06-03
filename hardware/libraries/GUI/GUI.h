#ifndef GUI_H__
#define GUI_H__

#include <stdlib.h>

#include "ModalGui.hh"

#include "WProgram.h"

#ifdef MIDIDUINO_USE_GUI

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

class Page {
 public:
  Page() { }
  virtual void handle() { }
  virtual void update() { }
  virtual void clear()  { }
  virtual void display(bool redisplay = false) { }
  virtual bool handleGui() { return false; }
};

class EncoderPage : public Page {
 public:
  Encoder *encoders[GUI_NUM_ENCODERS];
  
  EncoderPage(Encoder *e1 = NULL, Encoder *e2 = NULL, Encoder *e3 = NULL, Encoder *e4 = NULL) {
    for (uint8_t i; i < GUI_NUM_ENCODERS; i++) {
      encoders[i] = NULL;
    }
    encoders[0] = e1;
    encoders[1] = e2;
    encoders[2] = e3;
    encoders[3] = e4;
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

  void handle(int val);
};

typedef struct line_s {
  char data[16];
  bool changed;
  char flash[16];
  bool flashChanged;
  bool flashActive;
  uint16_t flashTimer;
  uint16_t duration;
} line_t;

#define DEFAULT_FLASH_DURATION 600

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
  bool handleGui() {
    if (page != NULL)
      return page->handleGui();
    else
      return false;
  }

  static const uint8_t NUM_ENCODERS = GUI_NUM_ENCODERS;
  static const uint8_t NUM_BUTTONS  = GUI_NUM_BUTTONS;
  
  void put_value(uint8_t idx, uint8_t value);
  void put_value(uint8_t idx, int value);
  void put_value16(uint8_t idx, uint16_t value);
  void put_valuex(uint8_t idx, uint8_t value);
  void put_value_at(uint8_t idx, uint8_t value);
  void put_value_at(uint8_t idx, int value);
  void put_value16_at(uint8_t idx, uint16_t value);
  void put_valuex_at(uint8_t idx, uint8_t value);
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


  void flash(uint16_t duration = DEFAULT_FLASH_DURATION);
  void flash_put_value(uint8_t idx, uint8_t value,
		       uint16_t duration = DEFAULT_FLASH_DURATION);
  void flash_put_value16(uint8_t idx, uint16_t value,
		       uint16_t duration = DEFAULT_FLASH_DURATION);
  void flash_put_valuex(uint8_t idx, uint8_t value,
		       uint16_t duration = DEFAULT_FLASH_DURATION);
  void flash_put_value_at(uint8_t idx, uint8_t value,
		       uint16_t duration = DEFAULT_FLASH_DURATION);
  void flash_put_value16_at(uint8_t idx, uint16_t value,
		       uint16_t duration = DEFAULT_FLASH_DURATION);
  void flash_put_valuex_at(uint8_t idx, uint8_t value,
		       uint16_t duration = DEFAULT_FLASH_DURATION);
  void flash_string(char *str,
		    uint16_t duration = DEFAULT_FLASH_DURATION);
  void flash_p_string(PGM_P str,
		      uint16_t duration = DEFAULT_FLASH_DURATION);
  void flash_string_fill(char *str,
			 uint16_t duration = DEFAULT_FLASH_DURATION);
  void flash_p_string_fill(PGM_P str,
			   uint16_t duration = DEFAULT_FLASH_DURATION);
  void flash_string_at(uint8_t idx, char *str,
		       uint16_t duration = DEFAULT_FLASH_DURATION);
  void flash_p_string_at(uint8_t idx, PGM_P str,
			 uint16_t duration = DEFAULT_FLASH_DURATION);
  void flash_string_at_fill(uint8_t idx, char *str,
			    uint16_t duration = DEFAULT_FLASH_DURATION);
  void flash_p_string_at_fill(uint8_t idx, PGM_P str,
			      uint16_t duration = DEFAULT_FLASH_DURATION);
  void flash_string_clear(char *str,
			  uint16_t duration = DEFAULT_FLASH_DURATION);
  void flash_p_string_clear(char *str,
			    uint16_t duration = DEFAULT_FLASH_DURATION);
  void flash_strings_fill(char *str1, char *str2,
			  uint16_t duration = DEFAULT_FLASH_DURATION);
  void flash_p_strings_fill(PGM_P str1, PGM_P str2,
			    uint16_t duration = DEFAULT_FLASH_DURATION);
  
  
  void setLine(const uint8_t line) { curLine = line; }
  void clearLine();
  void clearFlash(uint16_t duration = DEFAULT_FLASH_DURATION);
  void setPage(Page *_page);
  static const uint8_t LINE1 = 0;
  static const uint8_t LINE2 = 1;
  
};

extern GuiClass GUI;

#endif

#endif /* GUI_H__ */
