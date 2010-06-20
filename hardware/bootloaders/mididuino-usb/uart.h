/**# UART header file, with definition for access macros and
 ** definition of the baudrate.
 **/

#ifndef UART_H__
#define UART_H__

#include <inttypes.h>
#include <avr/io.h>

#include "common.h"

#define UART_CHECK_EMPTY_BUFFER() IS_BIT_SET(UCSRA, UDRE)
#define UART_WRITE_CHAR(c) (UDR = (c))
#define UART_READ_CHAR() (UDR)

/** 31250 bps for MIDI, works well with the CPU speed of 8 Mhz. **/
#define UART_BAUDRATE 31250
#define UART_BAUDRATE_REG (((F_CPU / 16)/(UART_BAUDRATE)) - 1)

void uart_init(void);
void uart_putc(uint8_t c);
uint8_t uart_getc(void);
uint8_t uart_avail(void);

#endif /* UART_H__ */
