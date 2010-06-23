#ifndef WProgram_h
#define WProgram_h

#define MIDIDUINO 1
#define SYSEX_BUF_SIZE 6000
//#define SYSEX_BUF_SIZE 128

#ifdef __cplusplus
extern "C" {
#endif
#include <inttypes.h>
#include <avr/interrupt.h>

#ifdef __cplusplus
  void __mainInnerLoop(bool callLoop = true);

}
#endif

#include "helpers.h"

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

#include "mididuino_private.h"

#ifdef __cplusplus

#include "LCD.h"
#include "GUI_private.h"
#include "MidiUart.h"
#include "MidiClock.h"
#include "Stack.h"
#include "GUI.h"
#include "Midi.h"
#include "WMath.h"

#endif

#endif /* WProgram_h */
