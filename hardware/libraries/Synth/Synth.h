#ifndef SYNTH_H__
#define SYNTH_H__

#include <inttypes.h>
#include "WProgram.h"

#define AUDIO            9
#define WRITE_AUDIO(val) { OCR1A = (val); }
#define MAX_POLYPHONY    4

typedef struct note_s {
  uint16_t phase;
  uint16_t speed;
  uint8_t  sample;
  uint16_t length;
} note_t;

extern const uint16_t freqtable[91];
extern note_t notes[MAX_POLYPHONY];
class SynthClass {
 protected:
  uint8_t nextNote();
  
 public:
  SynthClass();

  void setPolyphony(uint8_t numNotes);
  uint8_t getPolyphony();

  void init();

  bool isPlaying(uint8_t note);
  void play(uint8_t pitch, uint8_t length, uint8_t note = 0xFF);
};

extern SynthClass Synth;

uint32_t fast_random(uint16_t max = 0);
void fast_srand(uint32_t bla);

#endif /* SYNTH_H__ */

