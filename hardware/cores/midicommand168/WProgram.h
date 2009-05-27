#ifndef WProgram_h
#define WProgram_h

extern "C" {
#include <inttypes.h>
#include <avr/interrupt.h>
}

#include "mididuino_private.h"

/* default config flags */
#define MIDIDUINO_POLL_GUI     1
#define MIDIDUINO_POLL_GUI_IRQ 1
#define MIDIDUINO_HANDLE_SYSEX 1
#define MIDIDUINO_MIDI_CLOCK  1
#define MIDIDUINO_USE_GUI      1

#ifdef __cplusplus
#include "LCD.h"
#include "GUI.h"
#include "Midi.h"
#include "MidiUart.h"
#include "MidiDuino.h"
#include "EncoderPage.h"
#include "MidiClock.h"

uint16_t makeWord(uint16_t w);
uint16_t makeWord(uint8_t h, uint8_t l);

#define word(...) makeWord(__VA_ARGS__)

unsigned long pulseIn(uint8_t pin, uint8_t state, unsigned long timeout = 1000000L);

// WMath prototypes
long random(long);
long random(long, long);
void randomSeed(unsigned int);
long map(long, long, long, long, long);
#endif

#endif /* WProgram_h */
