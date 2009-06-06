#include <avr/io.h>

void my_init_ram (void) __attribute__ ((naked))	\
    __attribute__ ((section (".init1")));

void
my_init_ram (void)
{
  MCUCR |= _BV(SRE);
  //  uint8_t *ptr = 0x2000;
  //  unsigned long i = 0;
  //  for (i = 0; i < 60000; i++) {
    //    ptr[i] = 0;
  //  }
}

