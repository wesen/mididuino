/*
 * MidiCtrl - Sketch featuring 4 pages with automatic recording controllers
 *
 * (c) 2009 - 2011 - Manuel Odendahl - wesen@ruinwesen.com
 */

#ifndef AUTO_MIDI_CONTROLLER_H__
#define AUTO_MIDI_CONTROLLER_H__

#include "PlatformConfig.h"
#include "CCHandler.h"
#include "AutoEncoderPage.h"

/**
 * \addtogroup MIDI
 *
 * @{
 *
 * \addtogroup midi_sketches MIDI Sketches
 *
 * @{
 *
 * \addtogroup AutoMidiControllerSketch
 *
 * @{
 *
 * \file
 * Automagic MIDI sketch
 **/

/**
 * Creates a sketch featuring 4 autoencoderpages providing magic
 * learning and autorecording functionality for 4 MIDI encoders.
 *
 * Page switching is mapped to BUTTON1 (upper left).
 **/

class AutoMidiControllerSketch : public Sketch {
 public:
  AutoEncoderPage<AutoNameCCEncoder> autoPages[4];
  SwitchPage switchPage;
  
  AutoMidiControllerSketch() {
    delay(100);
  }
  
  void setup() {
    ccHandler.setup();
    
    char name[4] = "P ";
    for (uint8_t i = 0; i < 4; i++) {
      autoPages[i].setup();
      name[1] = '0' + i;
      autoPages[i].setShortName(name);
      for (uint8_t j = 0; j < 4; j++) {
	name[3] = '0' + j;
	autoPages[i].realEncoders[j].initCCEncoder(0, j + i * 4);
        autoPages[i].realEncoders[j].setCCName();
      }
      autoPages[i].redisplay = true;
    }
    
    switchPage.initPages(&autoPages[0], &autoPages[1], &autoPages[2], &autoPages[3]);
    switchPage.parent = this;
    
    setPage(&autoPages[0]);
  }
  
  virtual bool handleEvent(gui_event_t *event) {
    if (BUTTON_DOWN(Buttons.BUTTON4) && EVENT_PRESSED(event, Buttons.BUTTON1)) {
      clearAll();
      return true;
    }
      
    if (EVENT_PRESSED(event, Buttons.BUTTON1)) {
      pushPage(&switchPage);
      return true;
    } else if (EVENT_RELEASED(event, Buttons.BUTTON1)) {
      popPage(&switchPage);
      return true;
    }
    
    return false;
  }
  
  void clearAll() {
    GUI.flash_strings_fill("CLEAR ALL", "");
    for (uint8_t i = 0; i < 4; i++) {
      autoPages[i].clearRecording();
    }
  }

  void getName(char *n1, char *n2) {
    m_strncpy_p(n1, PSTR("MGC "), 5);
    m_strncpy_p(n2, PSTR("MID "), 5);
  }

  virtual void show() {
    if (currentPage() == NULL)
      setPage(&autoPages[0]);
  }

  virtual void mute(bool pressed) {
  }

  virtual void doExtra(bool pressed) {
  }

  virtual Page *getPage(uint8_t i) {
    if (i < 4) {
      return &autoPages[i];
    } else {
      return NULL;
    }
  }
};

/** @} @} @} */

#endif /* AUTO_MIDI_CONTROLLER_H__ */

