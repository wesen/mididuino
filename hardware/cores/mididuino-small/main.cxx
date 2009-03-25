#include "WProgram.h"
#include <avr/interrupt.h>
#include <avr/wdt.h>

void setup();
void loop();
void handleGui();

MidiClass Midi;

#define CHECK_RESET() (BUTTON_DOWN(BUTTON_1) && BUTTON_DOWN(ENCODER_1) && BUTTON_DOWN(ENCODER_2))

#ifdef MIDIDUINO_HANDLE_SYSEX
uint8_t mididuino_sysex_data[128];
MididuinoSysexClass mididuinoSysex(mididuino_sysex_data, sizeof(mididuino_sysex_data));

#endif

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
#ifdef MIDIDUINO_ENABLE_LFOS
  uint8_t i;
  for (i = 0; i < 4; i++) {
    LFO[i].acc += LFO[i].inc;
  }
#endif /* MIDIDUINO_ENABLE_LFOS */
  
  slowclock++;
}

/** Buffer for receiving midi messages. **/
static uint8_t recv_bytes = 0;
static uint8_t recv_buf[64];

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

    loop();

  }
  return 0;
}
