#include "WProgram.h"
#include "GUI.h"
#include "Pages.hh"

void Page::update() {
}

void Page::redisplayPage() {
  GUI.setLine(GUI.LINE1);
  GUI.clearLine();
  GUI.setLine(GUI.LINE2);
  GUI.clearLine();
  redisplay = true;
}
  

void EncoderPage::update() {
  encoder_t _encoders[GUI_NUM_ENCODERS];

  USE_LOCK();
  SET_LOCK();
  m_memcpy(_encoders, Encoders.encoders, sizeof(_encoders));
  Encoders.clearEncoders();
  CLEAR_LOCK();
  
  for (uint8_t i = 0; i < GUI_NUM_ENCODERS; i++) {
    if (encoders[i] != NULL) 
      encoders[i]->update(_encoders + i);
  }
}

void EncoderPage::clear() {
  for (uint8_t i = 0; i < GUI_NUM_ENCODERS; i++) {
    if (encoders[i] != NULL)
      encoders[i]->clear();
  }
}

void EncoderPage::display() {
  if (redisplay) {
    displayNames();
  }
  GUI.setLine(GUI.LINE2);
  for (uint8_t i = 0; i < 4; i++) {
    if (encoders[i] != NULL)
      if (encoders[i]->hasChanged() || redisplay || encoders[i]->redisplay) {
	encoders[i]->displayAt(i);
      }
  }
}

void EncoderPage::finalize() {
  for (uint8_t i = 0; i < GUI_NUM_ENCODERS; i++) {
    if (encoders[i] != NULL) 
      encoders[i]->checkHandle();
  }
}

void EncoderPage::displayNames() {
  GUI.setLine(GUI.LINE1);
  for (uint8_t i = 0; i < 4; i++) {
    if (encoders[i] != NULL)
      GUI.put_string(i, encoders[i]->getName());
    else
      GUI.put_p_string(i, PSTR("    "));
  }
}


void SwitchPage::display() {
  if (redisplay) {
    GUI.setLine(GUI.LINE1);
    GUI.put_string_at_fill(0, name);
    GUI.setLine(GUI.LINE2);
    for (int i = 0; i < 4; i++) {
      if (pages[i] != NULL) {
	GUI.put_string_fill(i, pages[i]->shortName);
      }
    }
  }
}

bool SwitchPage::handleEvent(gui_event_t *event) {
  for (int i = Buttons.ENCODER1; i <= Buttons.ENCODER4; i++) {
    if (pages[i] != NULL && EVENT_PRESSED(event, i)) {
      if (parent != NULL) {
	parent->setPage(pages[i]);
      }
      return true;
    }
  }
  return false;
}
