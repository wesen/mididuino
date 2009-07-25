#ifndef MIDICLOCKPAGE_H__
#define MIDICLOCKPAGE_H__

#include "WProgram.h"
#include "MidiClock.h"
#include "GUI.h"

class MidiClockPage : public EncoderPage {
 public:
  EnumEncoder clockSourceEncoder;
  BoolEncoder transmitEncoder;
  static const char *clockSourceEnum[];
  
  virtual void setup();

  virtual void show();
  virtual void loop();
  virtual bool handleEvent(gui_event_t *event);
  virtual void display() {
    if (redisplay)
      MidiUart.sendNoteOn(2, 3);
    EncoderPage::display();
  }
};

#endif /* MIDICLOCKPAGE_H__ */
