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

  clock++;
#ifdef MIDIDUINO_MIDI_CLOCK
  if (MidiClock.state == MidiClock.STARTED) {
    MidiClock.handleTimerInt();
  }
#endif

  //  clearLed2();
}

// XXX CMP to have better time

void gui_poll() {
  static bool inGui = false;
  if (inGui)
    return;
  else
    inGui = true;
  sei(); // reentrant interrupt

  static uint16_t oldsr = 0;
  uint16_t sr = SR165.read16();
  if (sr != oldsr) {
    Buttons.clear();
    Buttons.poll(sr >> 8);
    Encoders.poll(sr);
    oldsr = sr;
    pollEventGUI();
  }
  inGui = false;
}

uint16_t lastRunningStatusReset = 0;

#define OUTPUTPORT PORTD
#define OUTPUTDDR  DDRD
#define OUTPUTPIN PD0

ISR(TIMER2_OVF_vect) {
  slowclock++;
  if (abs(slowclock - lastRunningStatusReset) > 3000) {
    MidiUart.resetRunningStatus();
    lastRunningStatusReset = slowclock;
  }
  
  //  SET_BIT(OUTPUTPORT, OUTPUTPIN);

#ifdef MIDIDUINO_POLL_GUI_IRQ
  gui_poll();
#endif
  //  CLEAR_BIT(OUTPUTPORT, OUTPUTPIN);
}

MidiClass Midi;
MidiClass Midi2;

void handleIncomingMidi() {
  while (MidiUart.avail()) {
    Midi.handleByte(MidiUart.getc());
  }
  
  while (MidiUart2.avail()) {
    Midi2.handleByte(MidiUart2.getc());
  }
}

void __mainInnerLoop(bool callLoop) {
  //  SET_BIT(OUTPUTPORT, OUTPUTPIN);
  //  setLed2();

  //  CLEAR_BIT(OUTPUTPORT, OUTPUTPIN);
  handleIncomingMidi();
  
  if (callLoop) {
    GUI.loop();
  }
}

int main(void) {
  delay(100);
  init();
  clearLed();
  clearLed2();

  for (int i = 0; i < GUI_NUM_BUTTONS; i++) {
    SET_B_CURRENT(i);
  }

  MidiSysex.addSysexListener(&MididuinoSysexListener);

  OUTPUTDDR |= _BV(OUTPUTPIN);
  setup();
  sei();

  for (;;) {
    __mainInnerLoop();
  }
  return 0;
}

void handleGui() {
  pollEventGUI();
}
