/*
 * MidiCtrl - Platform file for minicommand2 cores
 *
 * (c) 2009 - 2011 - Manuel Odendahl - wesen@ruinwesen.com
 */

#ifndef PLATFORM_H__
#define PLATFORM_H__

#include "PlatformConfig.h"

#ifdef __cplusplus
extern "C" {
#endif
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <avr/iom64.h>

#ifdef __cplusplus
  void __mainInnerLoop(bool callLoop = true);
}
#endif

#include "helpers.h"

void init(void);

inline void toggleLed(void) {
  TOGGLE_BIT(PORTE, PE4);
}

inline void setLed(void) {
  CLEAR_BIT(PORTE, PE4);
}

inline void clearLed(void) {
  SET_BIT(PORTE, PE4);
}

inline void setLed2(void) {
  CLEAR_BIT(PORTE, PE5);
}

inline void clearLed2(void) {
  SET_BIT(PORTE, PE5);
}

inline void toggleLed2(void) {
  TOGGLE_BIT(PORTE, PE5);
}

void start_bootloader(void);
void handleIncomingMidi();

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

#endif /* PLATFORM_H__ */
