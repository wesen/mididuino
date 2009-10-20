#ifndef MONOME_MIDI_PAGE_H__
#define MONOME_MIDI_PAGE_H__

#include "MonomeTrigPage.h"

class MonomeMidiPage : public MonomeTrigPage {
 public:
  MonomeMidiPage(MonomeParentClass *monome);
  virtual bool handleEvent(monome_event_t *evt);
  virtual void onTrigNote(uint8_t pitch);
  virtual void onTrigNoteOff(uint8_t pitch);
};

#endif /* MONOME_MIDI_PAGE_H__ */

