/*
 * MidiCtrl - Platform configuration file for host side compilation
 *
 * (c) July 2011 - Manuel Odendahl - wesen@ruinwesen.com
 */

#ifndef PLATFORM_CONFIG_H__
#define PLATFORM_CONFIG_H__


#include <inttypes.h>

#ifndef HOST_MIDIDUINO
#define HOST_MIDIDUINO
#endif

#define GUI_NUM_ENCODERS 4
#define GUI_NUM_BUTTONS 8

#define BOARD_ID 0x89
#define SYSEX_BUF_SIZE 8192

#define PROGMEM

#define MIDIDUINO_HANDLE_SYSEX

#include "HelperMacros.h"

#ifdef TEST_SUITE
#define MidiUart Midi
#endif

#endif /* PLATFORM_CONFIG_H__ */
