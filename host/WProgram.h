#ifndef WProgram_h
#define WProgram_h

#ifndef HOST_MIDIDUINO
#define HOST_MIDIDUINO
#endif

#include <stdio.h>

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

#define MIDIDUINO_HANDLE_SYSEX
#include "Midi.h"

#include "MidiUartOSX.h"

void hexDump(uint8_t *data, uint16_t len);

#endif /* WProgram_h */
