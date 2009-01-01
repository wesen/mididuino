#ifndef MIDICLOCK_H__
#define MIDICLOCK_H__

#include <inttypes.h>
#include "WProgram.h"

class MidiClockClass {
 public:
  void (*on96Callback)();
  void (*on32Callback)();
  void (*on16Callback)();

  uint32_t div96th_counter;
  uint32_t div32th_counter;
  uint32_t div16th_counter;
  uint16_t interval;
  uint16_t counter;
  uint16_t last_clock;
  uint16_t last_interval;
  uint32_t indiv96th_counter;
  uint16_t pll_x;
  uint16_t tempo;
  bool transmit;


  enum {
    PAUSED = 0,
    STARTING,
    STARTED
  } state;

  enum {
    OFF = 0,
    INTERNAL,
    EXTERNAL
  } mode;

  MidiClockClass();
  void setOn96Callback(void (*callback)()) {
    on96Callback = callback;
  }
  void setOn32Callback(void (*callback)()) {
    on32Callback = callback;
  }
  void setOn16Callback(void (*callback)()) {
    on16Callback = callback;
  }
  void init();
  void handleClock();
  void handleMidiStart();
  void handleMidiStop();
  void handleTimerInt();
  void start();
  void stop();
  void pause();
  void setTempo(uint16_t tempo);
  uint16_t getTempo();
};

extern MidiClockClass MidiClock;

#endif /* MIDICLOCK_H__ */

