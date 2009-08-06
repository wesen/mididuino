#ifndef MIDICLOCK_H__
#define MIDICLOCK_H__

#include "WProgram.h"
#include <inttypes.h>
#include "midi-common.hh"

typedef void (*midi_clock_callback_t)();

class MidiClockClass {
 public:
  volatile uint32_t indiv96th_counter;
  volatile uint32_t outdiv96th_counter;

  volatile uint32_t div96th_counter;
  volatile uint32_t div32th_counter;
  volatile uint32_t div16th_counter;
  volatile uint32_t indiv32th_counter;
  volatile uint32_t indiv16th_counter;
  volatile uint8_t mod6_counter;
  volatile uint8_t inmod6_counter;
  volatile uint16_t interval;

  volatile uint16_t counter;
  volatile uint16_t rx_phase;

  volatile uint16_t rx_last_clock;
  volatile uint16_t rx_clock;
  volatile bool doUpdateClock;

  volatile bool updateSmaller;
  uint16_t pll_x;
  uint16_t tempo;
  bool transmit;
  bool isInit;

  volatile enum {
    PAUSED = 0,
    STARTING = 1,
    STARTED = 2,
  } state;

#ifndef MIDIDUINO
  typedef enum {
    OFF = 0,
    INTERNAL_MIDI,
    EXTERNAL_MIDI,
    EXTERNAL_UART2
  } clock_mode_t;
  // arduino

#ifndef BOARD_ID
#define BOARD_ID 0x80
#endif
#else

  typedef enum {
    OFF = 0,
    INTERNAL,
    EXTERNAL,
    EXTERNAL_UART2
  } clock_mode_t;
  #define INTERNAL_MIDI INTERNAL
  #define EXTERNAL_MIDI EXTERNAL

#endif

  clock_mode_t mode;
  
  MidiClockClass();

  midi_clock_callback_t on96Callback;
  Vector<midi_clock_callback_t, 4> on96Callbacks;
  
  midi_clock_callback_t on32Callback;
  Vector<midi_clock_callback_t, 4> on32Callbacks;
  
  midi_clock_callback_t on16Callback;
  Vector<midi_clock_callback_t, 4> on16Callbacks;

  void setOn96Callback(midi_clock_callback_t cb);
  void addOn96Callback(midi_clock_callback_t cb);
  void removeOn96Callback(midi_clock_callback_t cb);
  
  void setOn32Callback(midi_clock_callback_t cb);
  void addOn32Callback(midi_clock_callback_t cb);
  void removeOn32Callback(midi_clock_callback_t cb);
  
  void setOn16Callback(midi_clock_callback_t cb);
  void addOn16Callback(midi_clock_callback_t cb);
  void removeOn16Callback(midi_clock_callback_t cb);
  
  void init();
  void handleClock();
  void updateClockPhase();
  void updateClockInterval();
  void handleMidiStart();
  void handleMidiStop();
  void handleTimerInt();
  void handleSongPositionPtr(uint8_t *msg);
  void setSongPositionPtr(uint16_t pos);
  void start();
  void stop();
  void pause();
  void setTempo(uint16_t tempo);
  uint16_t getTempo();

  bool isStarted() {
    return state == STARTED;
  }
};

extern MidiClockClass MidiClock;

#endif /* MIDICLOCK_H__ */

