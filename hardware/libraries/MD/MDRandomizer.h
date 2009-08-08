#ifndef MDRANDOMIZER_H__
#define MDRANDOMIZER_H__

#include <inttypes.h>
#include "Stack.h"

class MDRandomizerClass {
 public:
  static const uint32_t paramSelectMask[13];
  static const char *selectNames[13];

  MDRandomizerClass() {
    track = 0;
  }
  
  Stack<uint8_t [24], 8> undoStack;
  uint8_t origParams[24];
  uint8_t track;

  void setTrack(uint8_t _track);
  void randomize(int amount, uint8_t mask);
  void undo();
  void loadKit();
};


#endif /* MDRANDOMIZER_H__ */
