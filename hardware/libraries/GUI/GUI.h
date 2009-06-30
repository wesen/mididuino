#ifndef GUI_H__
#define GUI_H__

#include <stdlib.h>

#include "WProgram.h"
#include "Vector.hh"
#include "Task.hh"


#ifdef MIDIDUINO_USE_GUI

#include "Events.hh"
#include "Pages.hh"

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

class Page;
class Sketch;

extern Sketch _defaultSketch;

typedef bool (*event_handler_t)(gui_event_t *event);

class GuiClass {
 protected:
 public:
  line_t lines[2];
  uint8_t curLine;
  Vector<event_handler_t, 4> eventHandlers;
  Vector<Task *, 8> tasks;
  
  Sketch *sketch;

  GuiClass();

  virtual void loop();

  void setSketch(Sketch *_sketch);
  void setPage(Page *page);
  
  void addEventHandler(event_handler_t handler) {
    eventHandlers.add(handler);
  }
  void removeEventHandler(event_handler_t handler) {
    eventHandlers.remove(handler);
  }

  void addTask(Task *task) {
    tasks.add(task);
  }
  void removeTask(Task *task) {
    tasks.remove(task);
  }

  void update();
  void display();

  void redisplay();
  
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
  void put_string(uint8_t idx, const char *str);
  void put_p_string(uint8_t idx, PGM_P str);
  void put_string(const char *str);
  void put_p_string(PGM_P str);
  void put_string_fill(uint8_t idx, const char *str);
  void put_string_fill(const char *str);
  void put_p_string_fill(uint8_t idx, PGM_P str);
  void put_p_string_fill(PGM_P str);
  void put_string_at(uint8_t idx, const char *str);
  void put_p_string_at(uint8_t idx, PGM_P str);
  void put_string_at_fill(uint8_t idx, const char *str);
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
  void flash_string(const char *str,
		    uint16_t duration = DEFAULT_FLASH_DURATION);
  void flash_p_string(PGM_P str,
		      uint16_t duration = DEFAULT_FLASH_DURATION);
  void flash_string_fill(const char *str,
			 uint16_t duration = DEFAULT_FLASH_DURATION);
  void flash_p_string_fill(PGM_P str,
			   uint16_t duration = DEFAULT_FLASH_DURATION);
  void flash_string_at(uint8_t idx, const char *str,
		       uint16_t duration = DEFAULT_FLASH_DURATION);
  void flash_p_string_at(uint8_t idx, PGM_P str,
			 uint16_t duration = DEFAULT_FLASH_DURATION);
  void flash_string_at_fill(uint8_t idx, const char *str,
			    uint16_t duration = DEFAULT_FLASH_DURATION);
  void flash_p_string_at_fill(uint8_t idx, PGM_P str,
			      uint16_t duration = DEFAULT_FLASH_DURATION);
  void flash_string_clear(const char *str,
			  uint16_t duration = DEFAULT_FLASH_DURATION);
  void flash_p_string_clear(const char *str,
			    uint16_t duration = DEFAULT_FLASH_DURATION);
  void flash_strings_fill(const char *str1, const char *str2,
			  uint16_t duration = DEFAULT_FLASH_DURATION);
  void flash_p_strings_fill(PGM_P str1, PGM_P str2,
			    uint16_t duration = DEFAULT_FLASH_DURATION);
  
  
  void setLine(const uint8_t line) { curLine = line; }
  void clearLine();
  void clearFlashLine();
  void clearFlash(uint16_t duration = DEFAULT_FLASH_DURATION);

  static const uint8_t LINE1 = 0;
  static const uint8_t LINE2 = 1;
  
};

extern GuiClass GUI;

#include "Encoders.hh"
#include "Pages.hh"
#include "Sketch.hh"

#endif

#endif /* GUI_H__ */
