#include "LCD.h"
#include <string.h>
#include <inttypes.h>

#include "WProgram.h"

extern "C" {
#include <avr/io.h>
#include <avr/interrupt.h>
}

#define LCD_DATA_PORT PORTB
#define LCD_DATA_DDR  DDRB

#define LCD_CTRL_PORT PORTC
#define LCD_CTRL_DDR  DDRC

#define LCD_DELAY_US 50

#define LCD_RS     PC2
#define LCD_ENABLE PC3

#define LINE_LENGTH 16

#define LCD_SET_ENABLE()   { SET_BIT8(LCD_CTRL_PORT, LCD_ENABLE); }
#define LCD_CLEAR_ENABLE() { CLEAR_BIT8(LCD_CTRL_PORT, LCD_ENABLE); }
#define LCD_SET_RS()       { SET_BIT8(LCD_CTRL_PORT, LCD_RS); }
#define LCD_CLEAR_RS()     { CLEAR_BIT8(LCD_CTRL_PORT, LCD_RS); }

void LCDClass::enable() {
  LCD_SET_ENABLE();
  LCD_CLEAR_ENABLE();
}

void LCDClass::putnibble(uint8_t nibble) {
  uint8_t tmp = SREG;
  cli();
  LCD_DATA_PORT = (LCD_DATA_PORT & 0xF0) | (nibble & 0xF);
  enable();
  SREG = tmp;
}

void LCDClass::putbyte(uint8_t byte) {
  putnibble(byte >> 4);
  putnibble(byte & 0xF);
}

void LCDClass::putcommand(uint8_t command) {
  uint8_t tmp = SREG;
  cli();
  LCD_CLEAR_RS();

  putbyte(command);
  SREG = tmp;
  delayMicroseconds(LCD_DELAY_US);
}

void LCDClass::putdata(uint8_t data) {
  uint8_t tmp = SREG;
  cli();
  
  LCD_SET_RS();
  putbyte(data);
  SREG = tmp;
  delayMicroseconds(LCD_DELAY_US);
}

LCDClass::LCDClass() {
  LCD_DATA_DDR |= 0xF;
  LCD_DATA_PORT = 0x00;
  LCD_CTRL_DDR |= _BV(LCD_RS) | _BV(LCD_ENABLE);

  // wait for display
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

void LCDClass::puts_p(PGM_P s) {
  uint8_t len = 0;
  char c;
  while (((c = pgm_read_byte(s)) != 0) && (len++ < LINE_LENGTH)){
    putdata(c);
    s++;
  }
}

void LCDClass::puts(char *s) {
  uint8_t len = 0;
  while (*s != 0 && (len++ < LINE_LENGTH)) {
    putdata(*s);
    s++;
  }
}

void LCDClass::line1() {
  putcommand(0x80);
}

void LCDClass::line1(char *s) {
  line1();
  puts(s);
}

void LCDClass::line1_p(PGM_P s) {
  line1();
  puts_p(s);
}

void LCDClass::line1_fill(char *s) {
  line1();
  puts_fill(s);
}

void LCDClass::line1_p_fill(PGM_P s) {
  line1();
  puts_p_fill(s);
}

void LCDClass::line2() {
  putcommand(0xc0);
}

void LCDClass::line2(char *s) {
  line2();
  puts(s);
}

void LCDClass::line2_p(PGM_P s) {
  line2();
  puts_p(s);
}

void LCDClass::line2_fill(char *s) {
  line2();
  puts_fill(s);
}

void LCDClass::line2_p_fill(PGM_P s) {
  line2();
  puts_p_fill(s);
}

void LCDClass::clearLine() {
  uint8_t i;
  for (i = 0; i < 16; i++) {
    putdata(' ');
  }
}

void LCDClass::puts_fill(char *s, uint8_t i) {
  while (*s != 0) {
    putdata(*s);
    s++;
    i--;
  }
  while (i--) {
    putdata(' ');
  }
}

void LCDClass::puts_fill(char *s) {
  puts_fill(s, LINE_LENGTH);
}

void LCDClass::puts_p_fill(PGM_P s, uint8_t i) {
  char c;
  while ((c = pgm_read_byte(s)) != 0) {
    putdata(c);
    s++;
    i--;
  }
  while (i--) {
    putdata(' ');
  }
}

void LCDClass::puts_p_fill(PGM_P s) {
  puts_p_fill(s, LINE_LENGTH);
}

void LCDClass::put(char *data, uint8_t cnt) {
  while (cnt--)
    putdata(*data++);
}

void LCDClass::putnumber(uint8_t num) {
  putdata(num / 100 + '0');
  putdata((num % 100) / 10 + '0');
  putdata((num % 10) + '0');
}

void LCDClass::putnumber16(uint16_t num) {
  putdata(num / 10000 + '0');
  putdata((num % 10000) / 1000 + '0');
  putdata((num % 1000) / 100 + '0');
  putdata((num % 100) / 10 + '0');
  putdata((num % 10) + '0');
}

void LCDClass::putcx(uint8_t i) {
  if (i < 10) {
    putdata(i + '0');
  } else {
    putdata(i - 10 + 'a');
  }
}

void LCDClass::putnumberx(uint8_t num) {
  putcx((num >> 4) & 0xF);
  putcx(num & 0xF);
}

void LCDClass::putnumberx16(uint16_t num) {
  putcx((num >> 12) & 0xF);
  putcx((num >> 8) & 0xF);
  putcx((num >> 4) & 0xF);
  putcx(num & 0xF);
}

void LCDClass::putnumber32(uint32_t num) {
  uint8_t res[10];
  uint8_t i;
  for (i = 0; i < 10; i++) {
    res[i] = num % 10;
    num /= 10;
  }
  for (i = 0; i < 10; i++) {
    putdata(res[9-i]);
  }
}

LCDClass LCD;
