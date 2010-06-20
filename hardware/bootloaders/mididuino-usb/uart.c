/**# Interrupt driven UART implementation
 **/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart.h"
#include "app.h"

#define UART_RB_SIZE 128
typedef struct uart_rb_s {
  uint8_t rd, wr;
  uint8_t overflow;
  uint8_t buf[UART_RB_SIZE];}
 uart_rb_t;

#define UART_RB_INC(d) (uint8_t)(((d) + 1) % UART_RB_SIZE)

/** Ring buffer for incoming data. **/
static uart_rb_t uart_rx_rb;

void uart_rb_init(uart_rb_t *rb) {
  rb->rd = 0;
  rb->wr = 0;
  rb->overflow = 0;
}

void uart_init(void) {
  /** Initialise ring buffers. **/
  uart_rb_init(&uart_rx_rb);

  /** Set the baudrate of the serial interface. **/
  UBRRH = (UART_BAUDRATE_REG >> 8);
  UBRRL = (UART_BAUDRATE_REG & 0xFF);
  //  UBRRH = 0;
  //  UBRRL = 15;

  /** 8 bit, no parity **/
  UCSRC = _BV(UCSZ0) | _BV(UCSZ1) | _BV(URSEL);
  /** enable receive, transmit and receive and transmit interrupts. **/
  //  UCSRB = _BV(RXEN) | _BV(TXEN) | _BV(RXCIE);
  UCSRB = _BV(RXEN) | _BV(TXEN) | _BV(RXCIE);
}

/**
 ** Send a character on the serial interface.
 **/
void uart_putc(uint8_t c) {
  while (!UART_CHECK_EMPTY_BUFFER())
    ;
  /** The buffer is empty, send the character directly. **/
  UART_WRITE_CHAR(c);
}

/**
 ** Check if the receive ring buffer has place left.
 **/
uint8_t uart_avail(void) {
  return (uart_rx_rb.rd != uart_rx_rb.wr);
}

/**
 ** Get a character form the receive ring buffer.
 **/
uint8_t uart_getc(void) {
  /** uart_getc should never be called when there is no character
   ** available, but we still return 0.
   **/
  
  if (!uart_avail()) {
    return 0;
  }

  uint8_t ret = uart_rx_rb.buf[uart_rx_rb.rd];
  uart_rx_rb.rd = UART_RB_INC(uart_rx_rb.rd);
  return ret;
}

/**
 ** Receive a character from the serial interface and put it into the ring buffer.
 **/
SIGNAL(USART_RXC_vect) {
  if (UART_RB_INC(uart_rx_rb.wr) == uart_rx_rb.rd) {
    /**
     ** the receive buffer has overflown, read the char directly and discard it.
     **/
    uart_rx_rb.overflow++;
    volatile uint8_t a = UART_READ_CHAR();
    a = 0;
    return;
  }

  uint8_t c = UART_READ_CHAR();
  uart_rx_rb.buf[uart_rx_rb.wr] = c;
  uart_rx_rb.wr = UART_RB_INC(uart_rx_rb.wr);
}
