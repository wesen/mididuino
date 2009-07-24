#ifndef WProgram_h
#define WProgram_h

#ifndef HOST_MIDIDUINO
#define HOST_MIDIDUINO
#endif

#define _BV(i) (1 << (i))
#define PSTR(s) (s)

#define GUI_NUM_ENCODERS 4
#define GUI_NUM_BUTTONS 4

#define BUTTON_PRESSED(i) false
#define BUTTON_RELEASED(i) false
#define BUTTON_DOWN(i) false
#define BUTTON_UP(i) true

#include <stdio.h>

void handleIncomingMidi();

#define BOARD_ID 0x89
#define SYSEX_BUF_SIZE 8192

#define PROGMEM

#ifdef __cplusplus
extern "C" {
#endif
#include <inttypes.h>
#include "helpers.h"

#ifdef __cplusplus
}
#endif

#include "WMath.h"

#define MIDIDUINO_HANDLE_SYSEX
#include "Midi.h"

#include "MidiUartOSX.h"
#include "GUI_private.h"

void hexDump(uint8_t *data, uint16_t len);

#endif /* WProgram_h */
