#include <Midi.h>

#include "WProgram.h"
#include <avr/interrupt.h>

void setup();
void loop();
void handleGui();

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

void gui_poll() {
  uint16_t sr = SR165.read16();
  Buttons.clear();
  Buttons.poll(sr >> 8);
  Encoders.poll(sr);
}

uint16_t lastRunningStatusReset = 0;

ISR(TIMER2_OVF_vect) {
  //  setLed2();

#ifdef MIDIDUINO_POLL_GUI_IRQ
  gui_poll();
  handleGui();
#endif
  slowclock++;

  if (abs(slowclock - lastRunningStatusReset) > 3000) {
    MidiUart.resetRunningStatus();
    lastRunningStatusReset = slowclock;
  }
  
  //  clearLed2();
}

MidiClass Midi;
MidiClass Midi2;

void __mainInnerLoop(bool callLoop) {
    if (MidiUart.avail()) {
      Midi.handleByte(MidiUart.getc());
    }

    if (MidiUart2.avail()) {
      Midi2.handleByte(MidiUart2.getc());
    }
    
#if defined(MIDIDUINO_POLL_GUI) && !defined(MIDIDUINO_POLL_GUI_IRQ)
    cli();
    gui_poll();
    handleGui();
    sei();
#endif

    if (callLoop)
      loop();
}

int main(void) {
  init();
  clearLed();
  clearLed2();

  for (int i = 0; i < GUI_NUM_BUTTONS; i++) {
    SET_B_CURRENT(i);
  }

  MidiSysex.addSysexListener(&MididuinoSysexListener);
  setup();
  sei();

  for (;;) {
    __mainInnerLoop();
  }
  return 0;
}
