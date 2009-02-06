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
  //  setLed2();

  clock++;
#ifdef MIDIDUINO_MIDI_CLOCK
  if (MidiClock.state == MidiClock.STARTED)
    MidiClock.handleTimerInt();
#endif

  //  clearLed2();
}

// XXX CMP to have better time

ISR(TIMER2_OVF_vect) {
  //  setLed2();

#ifdef MIDIDUINO_POLL_GUI_IRQ
  uint16_t sr = SR165.read16();
  Buttons.clear();
  Buttons.poll(sr >> 8);
  Encoders.clearEncoders();
  Encoders.poll(sr);

  handleGui();
#endif
  slowclock++;

  //  clearLed2();
}

int main(void) {
  init();
  clearLed();
  clearLed2();

#ifdef MIDIDUINO_HANDLE_SYSEX
  Midi.setSysex(&mididuinoSysex);
#endif

  setup();
  sei();

  for (;;) {
    if (MidiUart.avail()) {
      Midi.handleByte(MidiUart.getc());
    }

    if (MidiUart2.avail()) {
      Midi2.handleByte(MidiUart2.getc());
    }
    
#if defined(MIDIDUINO_POLL_GUI) && !defined(MIDIDUINO_POLL_GUI_IRQ)
    GUI.poll();
    handleGui();
#endif

    loop();

#ifdef MIDIDUINO_POLL_GUI
    cli();
    GUI.update();
    sei();
#endif
  }
  return 0;
}
