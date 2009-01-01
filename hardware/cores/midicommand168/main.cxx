#include "WProgram.h"
#include <avr/interrupt.h>

void setup();
void loop();
void handleGui();

#ifdef MIDIDUINO_HANDLE_SYSEX
uint8_t mididuino_sysex_data[64];
MididuinoSysexClass mididuinoSysex(mididuino_sysex_data, sizeof(mididuino_sysex_data));
#endif

#define PHASE_FACTOR 16
static inline uint32_t phase_mult(uint32_t val) {
  return (val * PHASE_FACTOR) >> 8;
}


ISR(TIMER1_OVF_vect) {
  setLed();
  clock++;
#ifdef MIDIDUINO_MIDI_CLOCK
  if (MidiClock.state == MidiClock.STARTED)
    MidiClock.handleTimerInt();
#endif
  clearLed();
}

ISR(TIMER2_OVF_vect) {
#ifdef MIDIDUINO_POLL_GUI_IRQ
  GUI.poll();
  handleGui();
#endif
  slowclock++;
}

int main(void) {
  init();

#ifdef MIDIDUINO_HANDLE_SYSEX
  Midi.setSysex(&mididuinoSysex);
#endif

  setup();
  sei();

  for (;;) {
    if (MidiUart.avail()) {
      Midi.handleByte(MidiUart.getc());
    }

#if defined(MIDIDUINO_POLL_GUI) && !defined(MIDIDUINO_POLL_GUI_IRQ)
    GUI.poll();
    handleGui();
#endif

    loop();

#ifdef MIDIDUINO_POLL_GUI
    GUI.update();
#endif
  }
  return 0;
}
