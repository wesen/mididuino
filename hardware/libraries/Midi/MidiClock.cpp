#include "MidiClock.h"
#include "midi-common.hh"
#include "helpers.h"
// #include "MidiUart.h"

// #define DEBUG_MIDI_CLOCK 0

MidiClockClass::MidiClockClass() {
  init();
  mode = OFF;
  setTempo(120);
  transmit = false;
}

void MidiClockClass::init() {
  state = PAUSED;
  counter = 10000;
  rx_clock = rx_last_clock = 0;
  outdiv96th_counter = 0;
  div96th_counter = indiv96th_counter = 0;
  div32th_counter = indiv32th_counter = 0;
  div16th_counter = indiv16th_counter = 0;
  mod6_counter = inmod6_counter = 0;
  pll_x = 200;
  counter = 10000;
  isInit = false;
}


uint16_t midi_clock_diff(uint16_t old_clock, uint16_t new_clock) {
  if (new_clock >= old_clock)
    return new_clock - old_clock;
  else
    return new_clock + (65535 - old_clock);
}

void MidiClockClass::handleMidiStart() {
  if (transmit)
    MidiUart.sendRaw(MIDI_START);
  init();
  state = STARTING;
  outdiv96th_counter = 0;
  counter = 10000;
}

void MidiClockClass::handleMidiStop() {
  state = PAUSED;
  if (transmit)
    MidiUart.sendRaw(MIDI_STOP);
  
}

void MidiClockClass::start() {
  if (mode == INTERNAL_MIDI) {
    init();
    state = STARTED;
    if (transmit)
      MidiUart.sendRaw(MIDI_START);
  }
}

void MidiClockClass::stop() {
  if (mode == INTERNAL_MIDI) {
    state = PAUSED;
    if (transmit)
      MidiUart.sendRaw(MIDI_STOP);
  }
}

void MidiClockClass::pause() {
  if (mode == INTERNAL_MIDI) {
    if (state == PAUSED) {
      start();
    } else {
      stop();
    }
  }
}

void MidiClockClass::setTempo(uint16_t _tempo) {
  USE_LOCK();
  SET_LOCK();
  tempo = _tempo;
  //  interval = 62500 / (tempo * 24 / 60);
  interval = (uint32_t)((uint32_t)156250 / tempo) - 16;
  CLEAR_LOCK();
}

void MidiClockClass::handleSongPositionPtr(uint8_t *msg) {
  if (mode == EXTERNAL || mode == EXTERNAL_UART2) {
    uint16_t ptr = (msg[1] & 0x7F) | ((msg[2] & 0x7F) << 7);
    USE_LOCK();
    SET_LOCK();
    indiv96th_counter = ptr;
    indiv32th_counter = ptr / 3;
    indiv16th_counter = ptr / 6;
    inmod6_counter = indiv96th_counter % 6;
    CLEAR_LOCK();
  }
}

void MidiClockClass::setSongPositionPtr(uint16_t pos) {
  div96th_counter = pos;
  div32th_counter = pos / 3;
  div16th_counter = pos / 6;
  mod6_counter = pos % 6;
  if (transmit) {
    uint8_t msg[3] = { MIDI_SONG_POSITION_PTR, 0, 0 };
    msg[1] = pos & 0x7F;
    msg[2] = (pos >> 7) & 0x7F;
    MidiUart.sendRaw(msg, 3);
  }
}

void MidiClockClass::updateClockInterval() {
}

/* in interrupt on receiving 0xF8 */
void MidiClockClass::handleClock() {
  uint8_t _mod6_counter = mod6_counter;

  if (transmit)
    MidiUart.putc_immediate(0xF8);
  
  if (mod6_counter == 5) {
    div16th_counter++;
    div32th_counter++;
  }

  
  if (div16th_counter % 4 == 0) {
    setLed();
  } else {
    clearLed();
  }

  
  if (mod6_counter == 2) {
    div32th_counter++;
  }
  
  div96th_counter++;
  mod6_counter++;
  if (mod6_counter == 6)
    mod6_counter = 0;

  if (state == STARTING && div96th_counter >= 1) {
    state = STARTED;
  }

  static bool inCallback = false;
  if (inCallback) {
    return;
  } else {
    inCallback = true;
  }

  sei();

  //    on96Callbacks.call();
  
  if (_mod6_counter == 0) {
    on16Callbacks.call(div16th_counter);
    on32Callbacks.call(div32th_counter);
  }
  if (_mod6_counter == 3) {
    on32Callbacks.call(div32th_counter);
  }
  
  inCallback = false;
}

/* in interrupt on timer */
void MidiClockClass::handleTimerInt()  {
}
    
MidiClockClass MidiClock;
