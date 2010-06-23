#include "WProgram.h"

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

#define LCD_SET_ENABLE()   { SET_BIT8(LCD_CTRL_PORT, LCD_ENABLE); }
#define LCD_CLEAR_ENABLE() { CLEAR_BIT8(LCD_CTRL_PORT, LCD_ENABLE); }
#define LCD_SET_RS()       { SET_BIT8(LCD_CTRL_PORT, LCD_RS); }
#define LCD_CLEAR_RS()     { CLEAR_BIT8(LCD_CTRL_PORT, LCD_RS); }

void LCDClass::enable() {
  LCD_SET_ENABLE();
  LCD_CLEAR_ENABLE();
}

void LCDClass::putnibble(uint8_t nibble) {
  //  uint8_t tmp = SREG;
  //  cli();
  LCD_DATA_PORT = (LCD_DATA_PORT & 0xF0) | (nibble & 0xF);
  enable();
  //  SREG = tmp;
}

void LCDClass::putbyte(uint8_t byte) {
  putnibble(byte >> 4);
  putnibble(byte & 0xF);
}

void LCDClass::putcommand(uint8_t command) {
  //  uint8_t tmp = SREG;
  //  cli();
  LCD_CLEAR_RS();

  putbyte(command);
  //  SREG = tmp;
  delayMicroseconds(LCD_DELAY_US);
}

void LCDClass::putdata(uint8_t data) {
  //  uint8_t tmp = SREG;
  //  cli();
  
  LCD_SET_RS();
  putbyte(data);
  //  SREG = tmp;
  delayMicroseconds(LCD_DELAY_US);
}

LCDClass::LCDClass() {
  LCD_DATA_DDR |= 0xF;
  LCD_DATA_PORT = 0x00;
  LCD_CTRL_DDR |= _BV(LCD_RS) | _BV(LCD_ENABLE);

  // wait for display
  initLCD();
}

void LCDClass::initLCD() {

  putnibble(0x03);
  putnibble(0x02);
  delayMicroseconds(200);
  putcommand(0x28); // 8 bit, 2 zeilen
  putcommand(0xC); // display ein, cursor aus, kein blinken.
  putcommand(0x4);
  putcommand(0x1);
  putcommand(0x2);
  delay(5);
}  

LCDClass LCD;
