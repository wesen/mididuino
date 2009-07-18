#include "MidiClock.h"
#include "midi-common.hh"
#include "helpers.h"
#include "MidiUart.h"

#define DEBUG_MIDI_CLOCK 1

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
  rx_clock = rx_last_clock = 0;
  update_clock = update_last_clock = 0;
  div96th_counter = 0;
  div32th_counter = 0;
  div16th_counter = 0;
  mod6_counter = 0;
  indiv96th_counter = 0;
  inmod6_counter = 0;
  pll_x = 200;
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
  mod6_counter = 0;
  div96th_counter = 0;
  div32th_counter = 0;
  div16th_counter = 0;
  outdiv96th_counter = 0;
  interval_correct = 0;
  running_error = 0;
  running_count = 0;
  counter_phase = 0;
  counter = 0;
}

void MidiClockClass::handleMidiStop() {
  state = PAUSED;
  if (transmit)
    MidiUart.sendRaw(MIDI_STOP);
  
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

void MidiClockClass::handleSongPositionPtr(uint8_t *msg) {
  if (mode == EXTERNAL || mode == EXTERNAL_UART2) {
    uint16_t ptr = (msg[1] & 0x7F) | ((msg[2] & 0x7F) << 7);
    USE_LOCK();
    SET_LOCK();
    indiv96th_counter = ptr;
    CLEAR_LOCK();
  }
}

void MidiClockClass::setSongPositionPtr(uint16_t pos) {
  div96th_counter = pos;
  if (transmit) {
    uint8_t msg[3] = { MIDI_SONG_POSITION_PTR, 0, 0 };
    msg[1] = pos & 0x7F;
    msg[2] = (pos >> 7) & 0x7F;
    MidiUart.sendRaw(msg, 3);
  }
}

void MidiClockClass::updateClockInterval() {
  USE_LOCK();

  if (state == STARTED) {

    SET_LOCK();
    uint16_t _interval = interval;
    uint16_t _rx_clock = rx_clock;
    uint16_t _rx_last_clock = rx_last_clock;
    
    CLEAR_LOCK();

    uint16_t diff_rx = midi_clock_diff(_rx_last_clock, _rx_clock);


    uint16_t new_interval = 0;

#ifdef DEBUG_MIDI_CLOCK
    GUI.setLine(GUI.LINE2);
    GUI.put_value16(1, diff_rx);
#endif
    
    if (!isInit) {
      new_interval = diff_rx;
      isInit = true;
    } else {
      uint32_t bla =
	(((uint32_t)_interval * (uint32_t)pll_x) +
	 (uint32_t)(256 - pll_x) * (uint32_t)diff_rx);
      //      uint8_t rem = bla & 0xFF;
      if (bla > 0xfff) {
	// XXX stop
      }
      bla >>= 8;
      new_interval = bla;
      //      if (rem > 190) // omg voodoo to correct discarded precision induced drift
      //	new_interval++;
    }

    if (new_interval > 0x4FFF) {
      new_interval = 0x4FFF;
    } else if (new_interval < 0x80) {
      new_interval = 0x80;
    }
      
    
    SET_LOCK();
    interval = new_interval;
    CLEAR_LOCK();
  }
}

#define PHASE_FACTOR 32
static uint32_t phase_mult(uint32_t val) {
  //  return (val * PHASE_FACTOR) >> 8;
  return val / 4;
}

/* in interrupt on receiving 0xF8 */
void MidiClockClass::handleClock() {
  //  setLed();

  rx_phase = counter;
  uint16_t my_clock = clock;
  rx_last_clock = rx_clock;
  rx_clock = my_clock;

  indiv96th_counter++;
  inmod6_counter++;
  if (inmod6_counter == 6)
    inmod6_counter = 0;

  static uint16_t last_phase_add;
#ifdef DEBUG_MIDI_CLOCK
  GUI.setLine(GUI.LINE1);
  GUI.put_value16(1, last_phase_add);
#endif
  
  if (mode == EXTERNAL_MIDI || mode == EXTERNAL_UART2) {
    if (div96th_counter < indiv96th_counter) {
      updateSmaller = true;
      uint16_t phase_add = rx_phase / 4;
      if (phase_add == 0) {
	//	phase_add = rx_phase;
      }
      if (counter > phase_add) {
	counter -= phase_add;
	
#ifdef DEBUG_MIDI_CLOCK
	// something happens on brutal tempo jumps
	if (phase_add > 200) {
	  //	  GUI.flash_strings_fill("OHLA", "OHLA");
	}
#endif
      }
      last_phase_add = phase_add;
    } else {
      updateSmaller = false;
      if (interval > rx_phase) {
	uint16_t phase_add = (interval - rx_phase) / 4;
	if (phase_add == 0) {
	  //	  phase_add = (interval - rx_phase);
	}
	counter += phase_add;
	last_phase_add = phase_add;
      }
    }
  }

  if (state == STARTING && indiv96th_counter >= 2) {
    state = STARTED;
  }

  //  clearLed();
  
}

/* in interrupt on timer */
void MidiClockClass::handleTimerInt()  {
  //  sei();
  if (counter == counter_phase) {
    setLed2();
    uint8_t _mod6_counter = mod6_counter;
    
    div96th_counter++;
    mod6_counter++;

    if (mod6_counter == 6)
      mod6_counter = 0;
    
    if (transmit) {
      uint8_t len = (div96th_counter - outdiv96th_counter);
      for (int i = 0; i < len; i++) {
	MidiUart.putc_immediate(MIDI_CLOCK);
	outdiv96th_counter++;
      }
    }

#ifdef DEBUG_MIDI_CLOCK
    GUI.setLine(GUI.LINE1);
    GUI.put_value16(0, div96th_counter);
    GUI.setLine(GUI.LINE2);
    GUI.put_value16(0, outdiv96th_counter);
#endif

    //    uint16_t bla_clock = update_clock;
    uint16_t my_clock = clock;
    update_last_clock = update_clock;
    update_rx_clock = rx_clock;
    update_clock = my_clock;

    counter = interval;

    if (mode == EXTERNAL_MIDI || mode == EXTERNAL_UART2) {
      if ((div96th_counter < indiv96th_counter) ||
	  (div96th_counter > (indiv96th_counter + 1))) {
	div96th_counter = indiv96th_counter;
	mod6_counter = inmod6_counter;
      }
    }

    clearLed2();

    static bool inCallback = false;
    if (inCallback) {
      setLed();
      return;
    } else {
      inCallback = true;
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

    if ((MidiClock.mode == MidiClock.EXTERNAL ||
	 MidiClock.mode == MidiClock.EXTERNAL_UART2)) {
      MidiClock.updateClockInterval();
    }

    inCallback = false;
  } else {
    counter--;
  }

  clearLed2();
}
    
MidiClockClass MidiClock;
