#ifndef MONOME_TRIG_PAGE_H__
#define MONOME_TRIG_PAGE_H__

#include "Monome.h"
#include <inttypes.h>

class MonomeTrigPage : public MonomePage {
 public:
  uint8_t start;
	
  MonomeTrigPage(MonomeParentClass *monome, uint8_t _start);
  ~MonomeTrigPage();

  virtual bool handleEvent(monome_event_t *evt);
  
  void toggleRouting(uint8_t x, uint8_t y);
  bool isValidColumn(uint8_t column);
  uint8_t getColumn(uint8_t pitch);
  virtual void onTrigNote(uint8_t pitch);
  virtual void onTrigNoteOff(uint8_t pitch);

};


#endif /* MONOME_TRIG_PAGE_H__ */
