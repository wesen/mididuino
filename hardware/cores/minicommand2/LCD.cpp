/*
 * MidiCtrl - LCD routines for minicommand
 *
 * (c) July 2011 - Manuel Odendahl - wesen@ruinwesen.com
 */

#include "Platform.h"

#include "LCD.h"
#include <string.h>
#include <inttypes.h>
#include "helpers.h"

extern "C" {
#include <avr/io.h>
#include <avr/interrupt.h>
}

/*
  $LCD_DATA_PORT$    port to which the LCD is connected (lower 4 bits of the port)
  $LCD_DATA_DDR$
	
  $LCD_CTRL_PORT$    port to which the LCD ctrl lines are connected (ENABLE AND RS)
  $LCD_CTRL_DDR$

  $LCD_RS$           pin to which the RS line is connected
  $LCD_ENABLE$       pin to which the enable line is connected

  $LCD_DELAY_US$     delay in us between commands
		
*/

/** Helper macros for bitbanging the LCD. **/
#define LCD_SET_ENABLE()   { SET_BIT8(LCD_CTRL_PORT, LCD_ENABLE); }
#define LCD_CLEAR_ENABLE() { CLEAR_BIT8(LCD_CTRL_PORT, LCD_ENABLE); }
#define LCD_SET_RS()       { SET_BIT8(LCD_CTRL_PORT, LCD_RS); }
#define LCD_CLEAR_RS()     { CLEAR_BIT8(LCD_CTRL_PORT, LCD_RS); }

void LCDClass::enable() {
  LCD_SET_ENABLE();
  LCD_CLEAR_ENABLE();
}

void LCDClass::putnibble(uint8_t nibble) {
  LCD_DATA_PORT = (LCD_DATA_PORT & 0xF0) | (nibble & 0xF);
  enable();
}

void LCDClass::putbyte(uint8_t byte) {
  putnibble(byte >> 4);
  putnibble(byte & 0xF);
}

void LCDClass::putcommand(uint8_t command) {
  LCD_CLEAR_RS();
  putbyte(command);
  delayMicroseconds(LCD_DELAY_US);
}

void LCDClass::putdata(uint8_t data) {
  LCD_SET_RS();
  putbyte(data);
  delayMicroseconds(LCD_DELAY_US);
}

LCDClass::LCDClass() {
  LCD_DATA_DDR |= 0xF;
  LCD_DATA_PORT = 0x00;
  LCD_CTRL_DDR |= _BV(LCD_RS) | _BV(LCD_ENABLE);
  initLCD();
}

void LCDClass::initLCD() {
  putnibble(0x03);
  putnibble(0x02);
  delayMicroseconds(200);
  putcommand(0x28); /* 8 bit, 2 lines */
  putcommand(0xC);  /* enable display, no cursor, no blinking. */
  putcommand(0x4);
  putcommand(0x1);
  putcommand(0x2);
  delay(5);
}  

LCDClass LCD;
