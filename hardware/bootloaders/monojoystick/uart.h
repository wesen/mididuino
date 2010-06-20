/**# UART header file, with definition for access macros and
 ** definition of the baudrate.
 **/

#ifndef UART_H__
#define UART_H__

#include <inttypes.h>
#include <avr/io.h>

#include "common.h"

#define UART_RB_SIZE 32
typedef struct uart_rb_s {
  uint8_t rd, wr;
  uint8_t overflow;
  uint8_t buf[UART_RB_SIZE];}
 uart_rb_t;
#define UART_RB_INC(d) (uint8_t)(((d) + 1) % UART_RB_SIZE)

#define UART_CHECK_EMPTY_BUFFER() IS_BIT_SET8(UCSR0A, UDRE)
#define UART_CHECK_RX() IS_BIT_SET8(UCSR0A, RXC)
#define UART_WRITE_CHAR(c) (UDR0 = (c))
#define UART_READ_CHAR() (UDR0)

/** 31250 bps for MIDI, works well with the CPU speed of 8 Mhz. **/
#define UART_BAUDRATE 31250
#define UART_BAUDRATE_REG (((F_CPU / 16)/(UART_BAUDRATE)) - 1)

void uart_rb_init(uart_rb_t *rb);
uint8_t uart_sending(void);
void uart_puts(uint8_t *data, uint8_t cnt);
void uart_init(void);
void uart_putc(uint8_t c);
uint8_t uart_getc(void);
uint8_t uart_avail(void);

#endif /* UART_H__ */
