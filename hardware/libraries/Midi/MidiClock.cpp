#include "MidiClock.h"
#include "midi-common.hh"
#include "helpers.h"
#include "MidiUart.h"

MidiClockClass::MidiClockClass() {
  init();
  mode = OFF;
  setTempo(120);
  on96Callback = NULL;
  on32Callback = NULL;
  on16Callback = NULL;
  transmit = false;
}

void MidiClockClass::init() {
  state = PAUSED;
  counter = 0;
  rx_clock = last_clock = 0;
  div96th_counter = 0;
  div32th_counter = 0;
  div16th_counter = 0;
  mod6_counter = 0;
  indiv96th_counter = 0;
  inmod6_counter = 0;
  doUpdateClock = false;
  pll_x = 220;
  isInit = false;
}

uint16_t midi_clock_diff(uint16_t old_clock, uint16_t new_clock) {
  if (new_clock >= old_clock)
    return new_clock - old_clock;
  else
    return new_clock + (65535 - old_clock);
}

void MidiClockClass::handleMidiStart() {
  if (mode == EXTERNAL_MIDI || mode == EXTERNAL_UART2) {
    init();
    state = STARTING;
    mod6_counter = 0;
    div96th_counter = 0;
    div32th_counter = 0;
    div16th_counter = 0;
    counter = 0;
  }
}

void MidiClockClass::handleMidiStop() {
  if (mode == EXTERNAL_MIDI || mode == EXTERNAL_UART2) {
    state = PAUSED;
  }
}

#define PHASE_FACTOR 16
static uint32_t phase_mult(uint32_t val) {
  return (val * PHASE_FACTOR) >> 8;
}

void MidiClockClass::start() {
  if (mode == INTERNAL_MIDI) {
    state = STARTED;
    mod6_counter = 0;
    div96th_counter = 0;
    div32th_counter = 0;
    div16th_counter = 0;
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

/* in interrupt */
void MidiClockClass::handleClock() {
  uint16_t my_clock = clock;
  last_clock = rx_clock;
  rx_clock = my_clock;

  indiv96th_counter++;
  inmod6_counter++;
  if (inmod6_counter == 6)
    inmod6_counter = 0;
  sei();

  if (state == STARTING && indiv96th_counter >= 2) {
    state = STARTED;
  }
  
}

void MidiClockClass::updateClockDiff() {
  USE_LOCK();

  if (state == STARTED) {
    SET_LOCK();
    uint16_t _interval = interval;
    uint16_t _rx_clock = rx_clock;
    uint16_t _last_clock = last_clock;
    CLEAR_LOCK();
    
    uint16_t diff = clock_diff(_last_clock, _rx_clock);
    GUI.setLine(GUI.LINE2);
    GUI.put_value16(1, diff);
    
    if (!isInit) {
      _interval = diff;
      isInit = true;
    } else {
      _interval =
	(((uint32_t)_interval * (uint32_t)pll_x) +
	 (uint32_t)(256 - pll_x) * (uint32_t)diff) >> 8;
    }
    
    SET_LOCK();
    interval = _interval;
    CLEAR_LOCK();
  }
}  

void MidiClockClass::updateClockDiffTimer() {
  if (!MidiClock.doUpdateClock)
    return;

  USE_LOCK();
  SET_LOCK();
  bool _updateSmaller = updateSmaller;
  uint16_t _counter = counter;
  uint16_t _update_clock = update_clock;
  uint16_t _update_last_clock = update_last_clock;
  CLEAR_LOCK();

  uint16_t diff = midi_clock_diff(_update_last_clock, _update_clock);
  GUI.setLine(GUI.LINE2);
  GUI.put_value16(2, diff);
  
  if (_updateSmaller) {
    _counter -= phase_mult(diff);
  } else {
    if (_counter > diff) {
      _counter += phase_mult(_counter - diff);
    }
  }

  SET_LOCK();
  doUpdateClock = false;
  counter = _counter;
  CLEAR_LOCK();
}

void MidiClockClass::handleTimerInt()  {
  //  sei();
  if (counter == 0) {
    uint16_t bla_clock = update_clock;
    update_clock = clock;
    update_last_clock = rx_clock;
    counter = interval;

    if (transmit) {
    setLed2();
      delayMicroseconds(3);
      MidiUart.putc_immediate(MIDI_CLOCK);
    clearLed2();
    }

    uint8_t _mod6_counter = mod6_counter;
    
    div96th_counter++;
    mod6_counter++;
    if (mod6_counter == 6)
      mod6_counter = 0;

#if 0
    GUI.setLine(GUI.LINE1);
    GUI.put_value(1, (uint8_t)div96th_counter % 256);
    GUI.put_value(2, (uint8_t)indiv96th_counter % 256);
    uint16_t diff = midi_clock_diff(update_last_clock, update_clock);
    uint16_t diff2 = midi_clock_diff(bla_clock, update_clock);
    
    GUI.setLine(GUI.LINE2);
    GUI.put_value16(2, diff);
    GUI.put_value16(3, diff2);
#endif

    if (mode == EXTERNAL_MIDI || mode == EXTERNAL_UART2) {
      if (div96th_counter <= indiv96th_counter) {
	updateSmaller = true;
      } else {
	updateSmaller = false;
      }
      
      if ((div96th_counter < indiv96th_counter) ||
	  (div96th_counter > (indiv96th_counter + 1))) {
	div96th_counter = indiv96th_counter;
	mod6_counter = inmod6_counter;
      }
      doUpdateClock = true;
    }

    sei();
    
    if (on96Callback)
      on96Callback();

    if (_mod6_counter == 0) {
      if (on16Callback)
	on16Callback();
      if (on32Callback)
	on32Callback();
      div16th_counter++;
      div32th_counter++;
    }
    if (_mod6_counter == 3) {
      if (on32Callback)
	on32Callback();
      div32th_counter++;
    }
  } else {
    counter--;
  }
}
    
MidiClockClass MidiClock;
