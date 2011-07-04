/*
 * MidiCtrl - Platform configuration file for the minicommand2 core
 *
 * (c) July 2011 - Manuel Odendahl - wesen@ruinwesen.com
 */

#ifndef PLATFORM_CONFIG_H__
#define PLATFORM_CONFIG_H__

#include <inttypes.h>

#define MIDIDUINO 1
#define SYSEX_BUF_SIZE 6000

#define FIRMWARE_LENGTH_ADDR ((uint16_t *)0x00)
#define FIRMWARE_CHECKSUM_ADDR ((uint16_t *)0x02)
#define START_MAIN_APP_ADDR ((uint16_t *)0x04)

#define BOARD_ID 0x41

/* LCD configuration variables */
#define LCD_DATA_PORT PORTF
#define LCD_DATA_DDR  DDRF

#define LCD_CTRL_PORT PORTE
#define LCD_CTRL_DDR  DDRE

#define LCD_DELAY_US 50

#define LCD_RS     PE2
#define LCD_ENABLE PE6

/* GUI configuration variables */
#define SR165_OUT    PD5
#define SR165_SHLOAD PD6
#define SR165_CLK    PD7

#define SR165_DATA_PORT PORTD
#define SR165_DDR_PORT  DDRD
#define SR165_PIN_PORT  PIND

/* default config flags */
#define MIDIDUINO_POLL_GUI     1
#define MIDIDUINO_POLL_GUI_IRQ 1
#define MIDIDUINO_HANDLE_SYSEX 1
#define MIDIDUINO_MIDI_CLOCK   1
#define MIDIDUINO_USE_GUI      1
// #define MIDIDUINO_EXTERNAL_RAM 1
// #define MDIIDUINO_SD_CARD      1

#include "HelperMacros.h"

#endif /* PLATFORM_CONFIG_H__ */
