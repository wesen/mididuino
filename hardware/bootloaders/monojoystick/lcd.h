#ifndef LCD_H__
#define LCD_H__

#include <inttypes.h>
#include <avr/pgmspace.h>

void lcd_putnumber(uint8_t num);
void lcd_putcx(uint8_t i);
void lcd_putnumberx(uint8_t num);
void lcd_putnumberx16(uint16_t num);
void lcd_putnumber16(uint16_t num);
void lcd_puts(char *s);
void lcd_puts_fill(char *s, uint8_t i);
void lcd_puts_p(PGM_P s);
void lcd_puts_p_fill(char *s, uint8_t i);
void lcd_clear_line(void);
void lcd_init(void);
void lcd_put(uint8_t *data, uint8_t cnt);
void lcd_line1(void);
void lcd_line2(void);

#endif /* LCD_H__ */
