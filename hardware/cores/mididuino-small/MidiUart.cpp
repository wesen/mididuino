#include "WProgram.h"

#include <avr/interrupt.h>
#include <util/delay.h>

#include "helpers.h"
#include <MidiUart.h>
#include <MidiClock.h>
#include <midi-common.hh>

MidiUartClass MidiUart;

#define UART_BAUDRATE 31250
#define UART_BAUDRATE_REG (((F_CPU / 16)/(UART_BAUDRATE)) - 1)

#define UART_CHECK_EMPTY_BUFFER() IS_BIT_SET8(UCSRA, UDRE)
#define UART_CHECK_RX() IS_BIT_SET8(UCSRA, RXC)
#define UART_WRITE_CHAR(c) (UDR = (c))
#define UART_READ_CHAR() (UDR)

#include <avr/io.h>

#ifdef _AVR_IOM168_H_
#define UBRR  UBRR0
#define UBRRL UBRR0L
#define UBRRH UBRR0H
#define UDRE  UDRE0
#define UDR   UDR0
#define UCSRA UCSR0A
#define UCSRB UCSR0B
#define UCSRC UCSR0C
#define RXC   RXC0
#define RXCIE RXCIE0
#define TXCIE TXCIE0
#define RXEN  RXEN0
#define TXEN  TXEN0


#define USART_TXC_vect USART_TX_vect
#define USART_RXC_vect USART_RX_vect
#endif


MidiUartClass::MidiUartClass() : MidiUartParent() {
  initSerial();
}

void MidiUartClass::initSerial() {
  UBRRH = (UART_BAUDRATE_REG >> 8);
  UBRRL = (UART_BAUDRATE_REG & 0xFF);
  //  UBRRH = 0;
  //  UBRRL = 15;

#ifdef _AVR_IOM168_H_
  /* Set frame format: 8data, 1stop bit */ 
  UCSR0C = (3<<UCSZ00); 
#else
  /** 8 bit, no parity **/
  UCSRC = _BV(UCSZ0) | _BV(UCSZ1) | _BV(URSEL);
#endif
  
  /** enable receive, transmit and receive and transmit interrupts. **/
  //  UCSRB = _BV(RXEN) | _BV(TXEN) | _BV(RXCIE);
  UCSRB = _BV(RXEN) | _BV(TXEN) | _BV(RXCIE);
#ifdef TX_IRQ
  UCSRB |= _BV(TXCIE);
#endif
}

void MidiUartClass::putc_immediate(uint8_t c) {
#ifdef TX_IRQ
  while (!UART_CHECK_EMPTY_BUFFER())
    ;
  UART_WRITE_CHAR(c);
#else
  putc(c);
#endif
}

void MidiUartClass::putc(uint8_t c) {
#ifdef TX_IRQ
 again:
  if (txRb.isEmpty() && UART_CHECK_EMPTY_BUFFER()) {
    UART_WRITE_CHAR(c);
  } else {
    if (txRb.isFull()) {
      while (txRb.isFull()) {
	uint8_t tmp = SREG;
	sei();
	delayMicroseconds(10);
	SREG = tmp;
      }
      goto again;
    } else {
      txRb.put(c);
    }
  }
#else
  while (!UART_CHECK_EMPTY_BUFFER())
    ;
  UART_WRITE_CHAR(c);
#endif
}

bool MidiUartClass::avail() {
  return !rxRb.isEmpty();
}

uint8_t MidiUartClass::getc() {
  return rxRb.get();
}

SIGNAL(USART_RXC_vect) {
  uint8_t c = UART_READ_CHAR();

  if (c == 0xF8 && MidiClock.mode == MidiClock.EXTERNAL) {
    MidiClock.handleClock();
  } else {
    MidiUart.rxRb.put(c);
  }
}

#ifdef TX_IRQ
SIGNAL(USART_TXC_vect) {
  if (!MidiUart.txRb.isEmpty())
    UART_WRITE_CHAR(MidiUart.txRb.get());
}
#endif
