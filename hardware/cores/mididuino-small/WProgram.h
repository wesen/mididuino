#ifndef WProgram_h
#define WProgram_h

#define MIDIDUINO 1

#ifdef __cplusplus
extern "C" {
#endif
#include <inttypes.h>
#include <avr/interrupt.h>
#include "helpers.h"
#ifdef __cplusplus
}
#endif

/* default config flags */
#define MIDIDUINO_HANDLE_SYSEX 1
#define MIDIDUINO_MIDI_CLOCK   1
//#define MIDIDUINO_ENABLE_LFOS  0

#include "mididuino_private.h"

#ifdef __cplusplus
#include "MidiUart.h"
#include "MidiClock.h"
#include "WMath.h"
#include "Midi.h"
#include "Stack.h"
#endif

#endif /* WProgram_h */

