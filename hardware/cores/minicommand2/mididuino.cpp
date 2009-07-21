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
  TCCR0 = _BV(CS01);
  //  TIMSK |= _BV(TOIE0);
  
  TCCR1A = _BV(WGM10); //  | _BV(COM1A1) | _BV(COM1B1); 
  TCCR1B |= _BV(CS10) | _BV(WGM12); // every cycle
#ifdef MIDIDUINO_MIDI_CLOCK
  TIMSK |= _BV(TOIE1);
#endif

  TCCR2 = _BV(WGM20) | _BV(WGM21) | _BV(CS20) | _BV(CS21); // ) | _BV(CS21); // | _BV(COM21);
  TIMSK |= _BV(TOIE2);
}

void init(void) {
  /** Disable watchdog. **/
  wdt_disable();
  //  wdt_enable(WDTO_15MS);

  /* move interrupts to bootloader section */
  DDRC = 0xFF;
  PORTC = 0x00;
  MCUCR = _BV(IVCE);
  MCUCR = _BV(SRE);

  // activate lever converter
  SET_BIT(DDRD, PD4);
  SET_BIT(PORTD, PD4);

  // activate background pwm
  TCCR3B = _BV(WGM32) | _BV(CS30);
  TCCR3A = _BV(WGM30) | _BV(COM3A1);
  OCR3A = 160;

  DDRE |= _BV(PE4) | _BV(PE5);
  //  DDRB |= _BV(PB0);
  //  DDRC |= _BV(PC3);

  timer_init();
}


void (*jump_to_boot)(void) = (void(*)(void))0xF000;

void start_bootloader(void) {
  cli();
  eeprom_write_word(START_MAIN_APP_ADDR, 0);
  jump_to_boot();
}

