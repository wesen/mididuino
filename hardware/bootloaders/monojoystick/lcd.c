#include "app.h"
#include "common.h"

#include <avr/pgmspace.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "lcd.h"

#define LCD_DATA_PORT PORTF
#define LCD_DATA_DDR  DDRF

#define LCD_CTRL_PORT PORTE
#define LCD_CTRL_DDR  DDRE

#define LCD_DELAY_US 50

#define LCD_RS     PE2
#define LCD_ENABLE PE6

#define LCD_SET_ENABLE()   { SET_BIT8(LCD_CTRL_PORT, LCD_ENABLE); }
#define LCD_CLEAR_ENABLE() { CLEAR_BIT8(LCD_CTRL_PORT, LCD_ENABLE); }
#define LCD_SET_RS()       { SET_BIT8(LCD_CTRL_PORT, LCD_RS); }
#define LCD_CLEAR_RS()     { CLEAR_BIT8(LCD_CTRL_PORT, LCD_RS); }

void inline lcd_enable(void) {
  LCD_SET_ENABLE();
  //  _delay_us(1);
  //  asm("nop");
  //  asm("nop");
  LCD_CLEAR_ENABLE();
}

void lcd_putnibble(uint8_t nibble) {
  uint8_t tmp = SREG;
  cli();
  LCD_DATA_PORT = (LCD_DATA_PORT & 0xF0) | (nibble & 0xF);
  lcd_enable();
  SREG = tmp;
}

void lcd_putbyte(uint8_t byte) {
  lcd_putnibble(byte >> 4);
  lcd_putnibble(byte & 0xF);
}

void lcd_command(uint8_t command) {
  LCD_CLEAR_RS();

  lcd_putbyte(command);
  _delay_us(LCD_DELAY_US);
}

void lcd_data(uint8_t data) {
  LCD_SET_RS();
  lcd_putbyte(data);
  _delay_us(LCD_DELAY_US);
}

void lcd_putnumber(uint8_t num) {
  lcd_data(num / 100 + '0');
  lcd_data((num % 100) / 10 + '0');
  lcd_data((num % 10) + '0');
}

void lcd_putnumber16(uint16_t num) {
  lcd_data(num / 10000 + '0');
  lcd_data((num % 10000) / 1000 + '0');
  lcd_data((num % 1000) / 100 + '0');
  lcd_data((num % 100) / 10 + '0');
  lcd_data((num % 10) + '0');
}

void lcd_putnumber32(uint32_t num) {
  uint8_t res[10];
  uint8_t i;
  for (i = 0; i < 10; i++) {
    res[i] = num % 10;
    num /= 10;
  }
  for (i = 0; i < 10; i++) {
    lcd_data(res[9-i]);
  }
}

void lcd_putcx(uint8_t i) {
  if (i < 10) {
    lcd_data(i + '0');
  } else {
    lcd_data(i - 10 + 'a');
  }
}
void lcd_putnumberx(uint8_t num) {
  lcd_putcx((num >> 4) & 0xF);
  lcd_putcx(num & 0xF);
}

void lcd_putnumberx16(uint16_t num) {
  lcd_putcx((num >> 12) & 0xF);
  lcd_putcx((num >> 8) & 0xF);
  lcd_putcx((num >> 4) & 0xF);
  lcd_putcx(num & 0xF);
}


void lcd_puts(char *s) {
  while (*s != 0) {
    lcd_data(*s);
    s++;
  }
}

void lcd_puts_fill(char *s, uint8_t i) {
  while (*s != 0) {
    lcd_data(*s);
    s++;
    i--;
  }
  while (i--) {
    lcd_data(' ');
  }
}

void lcd_puts_p(PGM_P s) {
  char c;
  while ((c = pgm_read_byte(s)) != 0) {
    lcd_data(c);
    s++;
  }
}

void lcd_puts_p_fill(char *s, uint8_t i) {
  char c;
  while ((c = pgm_read_byte(s)) != 0) {
    lcd_data(c);
    s++;
    i--;
  }
  while (i--) {
    lcd_data(' ');
  }
}

void lcd_clear_line(void) {
  uint8_t i;
  for (i = 0; i < 16; i++) {
    lcd_data(' ');
  }
}

void lcd_line1(void) {
  lcd_command(0x80);
  //  _delay_ms(1);
}

void lcd_put(uint8_t *data, uint8_t cnt) {
  while (cnt--)
    lcd_data(*data++);
}

void lcd_line2(void) {
  lcd_command(0xc0);
  //  _delay_ms(1);
}

void delay(uint8_t i);
void lcd_init(void) {
  LCD_DATA_DDR |= 0xF;
  LCD_DATA_PORT = 0x00;
  LCD_CTRL_DDR |= _BV(LCD_RS) | _BV(LCD_ENABLE);

  _delay_ms(100);
  // wait for display
  lcd_putnibble(0x03);
  lcd_putnibble(0x02);
  _delay_us(200);
  lcd_command(0x28); // 8 bit, 2 zeilen
  _delay_us(10);
  lcd_command(0xC); // display ein, cursor aus, kein blinken.
  lcd_command(0x4);
  lcd_command(0x1);
  lcd_command(0x2);
  _delay_ms(5);
}
