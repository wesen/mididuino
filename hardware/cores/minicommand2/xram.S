#include <avr/io.h>

        .section .init1,"ax",@progbits

        ldi r16,_BV(SRE)
        out _SFR_IO_ADDR(MCUCR),r16
