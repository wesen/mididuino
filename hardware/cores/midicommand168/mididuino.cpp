#include "WProgram.h"

extern "C" {
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
}

volatile uint16_t slowclock = 0;
volatile uint16_t clock = 0;

uint16_t read_clock(void) {
  uint8_t tmp = SREG;
  cli();
  uint16_t ret = clock;
  SREG = tmp;
  return ret;
}

uint16_t read_slowclock(void) {
  uint8_t tmp = SREG;
  cli();
  uint16_t ret = slowclock;
  SREG = tmp;
  return ret;
}

uint16_t clock_diff(uint16_t old_clock, uint16_t new_clock) {
  if (new_clock >= old_clock)
    return new_clock - old_clock;
  else
    return new_clock + (65535 - old_clock);
}

void timer_init(void) {
  TCCR1A = _BV(WGM10); //  | _BV(COM1A1) | _BV(COM1B1); 
  TCCR1B |= _BV(CS10) | _BV(WGM12); // every cycle

#ifdef _AVR_IOM168_H_
  TCCR2A = _BV(WGM20) | _BV(WGM21);
  TCCR2B = _BV(CS22); // ) | _BV(CS21); // | _BV(COM21);
  TIMSK2 |= _BV(TOIE2);
  TIMSK1 |= _BV(TOIE1);
#else
  TCCR2 = _BV(WGM20) | _BV(WGM21) | _BV(CS22); // ) | _BV(CS21); // | _BV(COM21);
  TIMSK |= _BV(TOIE2);
  TIMSK |= _BV(TOIE1);
#endif
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

  DDRB |= _BV(PB0);
  DDRC |= _BV(PC3);

  timer_init();
}

void delayMicroseconds(unsigned int us) {
  _delay_us(us);
}

void delay(unsigned int ms) {
  _delay_ms(ms);
}

uint8_t u_limit_value(uint8_t value, int8_t encoder, uint8_t min, uint8_t max) {
  int16_t result = (int16_t)value + encoder;
  if (result < ((int16_t)min)) {
    return min;
  } else if (result > ((int16_t)max)) {
    return max;
  } else {
    return result;
  }
}

void (*jump_to_boot)(void) = (void(*)(void))0x3800;

void start_bootloader(void) {
  cli();
  eeprom_write_word(START_MAIN_APP_ADDR, 0);
  jump_to_boot();
}

