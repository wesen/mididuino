#include "WProgram.h"

extern "C" {
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
}

void timer_init(void) {
  // 62.5 khz
  TCCR1A = _BV(WGM10); //  | _BV(COM1A1) | _BV(COM1B1); 
  TCCR1B |= _BV(CS10) | _BV(WGM12); // every cycle

  // 64 prescaler = 980 hz
#ifdef _AVR_IOM168_H_
  TCCR2A = _BV(WGM20) | _BV(WGM21);
  TCCR2B = _BV(CS22); // ) | _BV(CS21); // | _BV(COM21);
  TIMSK2 |= _BV(TOIE2);
  TIMSK1 |= _BV(TOIE1);
#else
  TCCR2 = _BV(WGM20) | _BV(WGM21) | _BV(CS20) | _BV(CS21); // ) | _BV(CS21); // | _BV(COM21);
  TIMSK |= _BV(TOIE2);
  TIMSK |= _BV(TOIE1);
#endif
}

void setLed() {
  SET_BIT(PORTC, PC4);
}

void clearLed() {
  CLEAR_BIT(PORTC, PC4);
}

void init(void) {
  /** Disable watchdog. **/
  wdt_disable();
  //  wdt_enable(WDTO_15MS);

  /* move interrupts to bootloader section */
#ifdef _AVR_IOM168_H_
  MCUCR = _BV(IVCE);
  MCUCR = 0;
#else
  GICR = _BV(IVCE);
  GICR = 0;
#endif

  DDRC |= _BV(4);
  timer_init();
}

void (*jump_to_boot)(void) = (void(*)(void))0x3800;

void start_bootloader(void) {
  cli();
  eeprom_write_word(START_MAIN_APP_ADDR, 0);
  jump_to_boot();
}

