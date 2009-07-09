#ifndef MNM_H__
#define MNM_H__

#include <inttypes.h>

#include "WProgram.h"
#include "Elektron.hh"

extern uint8_t monomachine_sysex_hdr[6];

class MNMEncoder : public CCEncoder {
};

class MNMClass {
 public:
  MNMClass() {
    baseChannel = 0;
  }

  uint8_t baseChannel;
  uint8_t multiTrigChannel;
  uint8_t autoChannel;
  
  int currentGlobal;
  int currentKit;
  int currentPattern;

  void loadGlobal(uint8_t id);
  void loadKit(uint8_t id);
  void loadPattern(uint8_t id);
  void loadSong(uint8_t id);

  void setCurrentKitName(char *name);

  void saveKit(uint8_t id);

  void requestKit(uint8_t kit);
  void requestPattern(uint8_t pattenr);
  void requestSong(uint8_t song);
  
};

#endif /* MNM_H__ */
