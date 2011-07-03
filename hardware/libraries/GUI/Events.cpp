/*
 * MidiCtrl - GUI event polling
 *
 * (c) 2009 - 2011 - Manuel Odendahl - wesen@ruinwesen.com
 */

#include "GUI.h"

volatile CRingBuffer<gui_event_t, 8> EventRB;

/**
 * Gather the current button status and insert GUI events into the
 * event ringbuffer if necessary.
 **/
void pollEventGUI() {
  for (int i = 0; i < GUI_NUM_BUTTONS; i++) {
    gui_event_t event;
    event.source = i;
    if (BUTTON_PRESSED(i)) {
      event.mask = EVENT_BUTTON_PRESSED;
      EventRB.putp(&event);
    }
    if (BUTTON_RELEASED(i)) {
      event.mask = EVENT_BUTTON_RELEASED;
      EventRB.putp(&event);
    }
  }
}
