#ifndef GUI_H__
#define GUI_H__

#include <stdlib.h>

#include "WProgram.h"

#ifdef MIDIDUINO_USE_GUI

#define INIT_PAGE(page, encoders, size)					\
  { for (uint8_t i = 0; i < (size); i++) { (page).encoders[i] = (encoders) + i; } }

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
  bool handleGui();

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
  void clearFlashLine();
  void clearFlash(uint16_t duration = DEFAULT_FLASH_DURATION);
  void setPage(Page *_page);
  static const uint8_t LINE1 = 0;
  static const uint8_t LINE2 = 1;
  
};

extern GuiClass GUI;

#include "Encoders.hh"
#include "Pages.hh"
#include "Events.hh"

#endif

#endif /* GUI_H__ */
