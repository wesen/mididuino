#ifndef EVENTS_H__
#define EVENTS_H__

#include <inttypes.h>
#include "RingBuffer.h"

#define EVENT_BUTTON_PRESSED  _BV(0)
#define EVENT_BUTTON_RELEASED _BV(1)

#define EVENT_PRESSED(event, button) ((event)->mask & EVENT_BUTTON_PRESSED && (event)->source == button)
#define EVENT_RELEASED(event, button) ((event)->mask & EVENT_BUTTON_RELEASED && (event)->source == button)

typedef struct gui_event_s {
  uint8_t mask;
  uint8_t source;
} gui_event_t;

// CRingBuffer<uint8_t, 8, uint8_t> EventRB;
// extern volatile CRingBuffer<gui_event_t, 8, uint8_t> EventRB;

void pollEventGUI();

#endif /* EVENTS_H__ */
