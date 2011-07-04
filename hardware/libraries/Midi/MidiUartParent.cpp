/* Copyright (c) 2009 - http://ruinwesen.com/ */

#include "Platform.h"
#include "MidiUartParent.hh"

/**
 * Helper method to send debug strings over the UART.
 **/
void MidiUartParent::sendString(const char *data, uint16_t cnt) {
  sendCommandByte(0xF0);
  uint8_t _data[4] = { MIDIDUINO_SYSEX_VENDOR_1, MIDIDUINO_SYSEX_VENDOR_2,
                       MIDIDUINO_SYSEX_VENDOR_3, CMD_STRING };
  puts(_data, 4);
  puts((uint8_t *)data, cnt);
  sendCommandByte(0xF7);
}
