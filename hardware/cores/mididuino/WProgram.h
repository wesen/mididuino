#ifndef WProgram_h
#define WProgram_h

extern "C" {
#include <inttypes.h>
#include <avr/interrupt.h>
#include "helpers.h"
}

/* default config flags */
#define MIDIDUINO_POLL_GUI     1
#define MIDIDUINO_POLL_GUI_IRQ 1
#define MIDIDUINO_HANDLE_SYSEX 1
#define MIDIDUINO_MIDI_CLOCK   1
#define MIDIDUINO_ENABLE_USB   1
#define MIDIDUINO_RESET_COMBO  1
#define MIDIDUINO_USE_GUI      1
//#define MIDIDUINO_ENABLE_LFOS  0

#include "mididuino_private.h"

#ifdef __cplusplus
#include "LCD.h"
#include "GUI_private.h"
#include "MidiUart.h"
#include "USBMidiUart.h"
#include "MidiClock.h"
#include "WMath.h"
#include "Midi.h"
#include "Stack.h"
#include "GUI.h"
#endif

#endif /* WProgram_h */

