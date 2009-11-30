/* Copyright (c) 2009 - http://ruinwesen.com/ */

#ifndef GUI_H__
#define GUI_H__

#include <stdlib.h>

#include "WProgram.h"
#include "Vector.hh"
#include "Task.hh"

#if defined(MIDIDUINO_USE_GUI) || defined(HOST_MIDIDUINO)

#define MIDIDUINO_GUI_ACTIVE 1

#include "Events.hh"
#include "Encoders.hh"
#include "Pages.hh"

/**
 * \addtogroup GUI
 *
 * \file
 * GUI class
 *
 * @{
 *
 **/


/**
 * \addtogroup gui_lcd LCD routines
 *
 * @{
 **/

/** Structure used to hold displaying information (flash lines, flash timer, flash duration, and display data). **/
typedef struct line_s {
  char data[16];
  bool changed;
  char flash[16];
  bool flashChanged;
  bool flashActive;
  uint16_t flashTimer;
  uint16_t duration;
} line_t;

/** Default duration of a flashed message (in timer ticks). **/
#define DEFAULT_FLASH_DURATION 600

/**
 * @}
 **/

/**
 * \addtogroup gui_class GUI class
 *
 * @{
 **/
class Page;
class Sketch;

/** The default sketch that is always available. **/
extern Sketch _defaultSketch;

typedef bool (*event_handler_t)(gui_event_t *event);

/**
 * The GUI class acting as a frontend to the display system. This
 * class is used to:
 *
 * - display data on the screen,
 * - add/remove tasks,
 * - add/remove event handlers
 * - handle the toplevel display loop (dispatching events, drawing pages, handling encoders).
 *
 * Although some methods are declared virtual, there is usually no need to subclass the GUI class.
 **/
class GuiClass {
 protected:
 public:
	/**
	 * \section gui_innen GUI innen
	 **/
	
	/** The two display lines of the MiniCommand display. **/
  line_t lines[2];
	/** The currently active line index. **/
  uint8_t curLine;
	/** A vector storing the registered event handlers (max 4). **/
  Vector<event_handler_t, 4> eventHandlers;
	/** A vector storing the registered tasks (max 8). **/
  Vector<Task *, 8> tasks;

#ifdef GUI_NUM_ENCODERS
	/** The number of encoders present in the GUI (4 on the minicommand). **/
  static const uint8_t NUM_ENCODERS = GUI_NUM_ENCODERS;
#endif
	/** The number of buttons present in the GUI (4 on the minicommand). **/
  static const uint8_t NUM_BUTTONS  = GUI_NUM_BUTTONS;

  GuiClass();

#ifdef HOST_MIDIDUINO
  virtual ~GuiClass() { }
#endif

	/**
	 * \section gui_class_sketch GUI class sketch and page handling
	 **/
	 
	/** The currently active sketch (default NULL). **/
  Sketch *sketch;
	
	/**
	 * Set the active sketch (its top page will be displayed).
	 * 
	 * If a sketch is already active, its hide() method is called.
	 * After the sketch is switched, the show() method of the new sketch
	 * is called, and the currentPage() is redisplayed by calling
	 * redisplayPage().
	 **/
  void setSketch(Sketch *_sketch);
	/** Returns a pointer to the current sketches currentPage(). **/
  Page *currentPage();

	/** When not using sketches, set the current page (all the page stack will be cleared). **/
  void setPage(Page *page);
	/** Push a new page on top of the currently active one. **/
  void pushPage(Page *page);
	/** Pop the top page. **/
  void popPage();
	/** Pop the top page if it is the same as page. **/
  void popPage(Page *page);

	/**
	 * \section gui_class_mainloop GUI class main loop
	 **/
	
	/**
	 * Add a new event handler to the event handler vector (max 4). The
	 * event handler will then be called on each update loop.
	 **/
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


	/** This is the main toplevel loop. **/
  virtual void loop();
	
  void update();
  void display();

  void redisplay();

	/**
	 * \section gui_lcd2 GUI lcd display handling 2
	 **/
	
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

	void printf(const char *fmt, ...);
	void printf_fill(const char *fmt, ...);
	void printf_at(uint8_t idx, const char *fmt, ...);
	void printf_at_fill(uint8_t idx, const char *fmt, ...);

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
  
	void flash_printf(const char *fmt, ...);
	void flash_printf_fill(const char *fmt, ...);
	void flash_printf_at(uint8_t idx, const char *fmt, ...);
	void flash_printf_at_fill(uint8_t idx, const char *fmt, ...);

	
  void setLine(const uint8_t line) { curLine = line; }
  void clearLine();
  void clearFlashLine();
  void clearFlash(uint16_t duration = DEFAULT_FLASH_DURATION);

  static const uint8_t LINE1 = 0;
  static const uint8_t LINE2 = 1;
	/** @} **/
};

/** The single instance of the GUI class. **/
extern GuiClass GUI;

char hex2c(uint8_t hex);

#include "Encoders.hh"
#include "Pages.hh"
#include "Sketch.hh"

/** @} **/
/** @} **/

#endif

#endif /* GUI_H__ */
