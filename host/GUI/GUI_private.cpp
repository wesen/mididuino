/*
 * MidiCtrl - Host implementation of hardware interfacing stubs
 *
 * (c) July 2011 - Manuel Odendahl - wesen@ruinwesen.com
 */

#include "Platform.h"
#include "GUI_private.h"
#include "GUI.h"

EncodersClass Encoders;
ButtonsClass Buttons;

/** Mock encoder class **/
#define ENCODER_NORMAL(i) (encoders[(i)].normal)
#define ENCODER_BUTTON(i) (encoders[(i)].button)

EncodersClass::EncodersClass() {
  clearEncoders();
}

void EncodersClass::clearEncoders() {
  for (uint8_t i = 0; i < GUI_NUM_ENCODERS; i++) {
    ENCODER_NORMAL(i) = ENCODER_BUTTON(i) = 0;
  }
}

void EncodersClass::turnEncoder(uint8_t encoder, int i, bool button) {
  if (button) {
    ENCODER_BUTTON(encoder) = i;
  } else {
    ENCODER_NORMAL(encoder) = i;
  }
}

/** Mock buttons class **/
ButtonsClass::ButtonsClass() {
  clearButtons();
}

void ButtonsClass::clearButtons() {
  for (uint8_t i = 0; i < GUI_NUM_BUTTONS; i++) {
    buttons[i].status = 0;
    /* buttons are up and were up */
    STORE_B_OLD(i, 1);
    STORE_B_CURRENT(i, 1);
  }
}

/** single GUI loop tick **/
void ButtonsClass::tick() {
  for (uint8_t i = 0; i < GUI_NUM_BUTTONS; i++) {
    STORE_B_OLD(i, B_CURRENT(i));
  }
}

void ButtonsClass::pressButton(uint8_t button) {
  STORE_B_CURRENT(button, 0);
}

void ButtonsClass::releaseButton(uint8_t button) {
  STORE_B_CURRENT(button, 1);
}

void ButtonsClass::printButtons() {
  for (uint8_t i = 0; i < GUI_NUM_BUTTONS; i++) {
    printf("Button %d: %x\n", i, B_CURRENT(i));
  }
}

void GUI_tick() {
  pollEventGUI();
  GUI.loop();
  Buttons.tick();
}
