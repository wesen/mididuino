/*
 * MidiCtrl - Platform include file
 *
 * (c) July 2011 - Manuel Odendahl - wesen@ruinwesen.com
 */

#ifndef PLATFORM_H__
#define PLATFORM_H__

#include "PlatformConfig.h"

#include <stdio.h>
#include <string.h>

void handleIncomingMidi();

#define setLed()
#define clearLed()
#define setLed2()
#define clearLed2()

#ifdef __cplusplus
extern "C" {
#endif
#include <inttypes.h>
#ifdef __cplusplus
}
#endif

#include "helpers.h"

#ifdef __cplusplus
#include "WMath.h"
#include "Midi.h"
#include "MidiClock.h"
#include "MidiUartHost.h"
#include "GUI_private.h"
#endif

#ifdef __cplusplus
#endif

void hexDump(uint8_t *data, uint16_t len);


#endif /* PLATFORM_H__ */
