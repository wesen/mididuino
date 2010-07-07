#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <MidiUart.h>
#include <midi-common.hh>
#include <MidiUartParent.hh>

#include <helpers.h>

#define UART_BAUDRATE 31250
#define UART_BAUDRATE_REG (((F_CPU / 16)/(UART_BAUDRATE)) - 1)

#define UART_CHECK_EMPTY_BUFFER() IS_BIT_SET8(UCSR1A, UDRE1)
#define UART_CHECK_RX() IS_BIT_SET8(UCSR1A, RXC)
#define UART_WRITE_CHAR(c) (UDR1 = (c))
#define UART_READ_CHAR() (UDR1)

#define UART2_CHECK_RX() IS_BIT_SET8(UCSR1A, RXC)
#define UART2_READ_CHAR() (UDR1)

#include <avr/io.h>

bool framingError = false;

void MidiUartClass::initSerial() {
  running_status = 0;
  UBRR1H = (UART_BAUDRATE_REG >> 8);
  UBRR1L = (UART_BAUDRATE_REG & 0xFF);
  //  UBRRH = 0;
  //  UBRRL = 15;

  UCSR1C = (3<<UCSZ10); 
}

void MidiUartClass::putByte_immediate(uint8_t c) {
#ifdef TX_IRQ
  while (!UART_CHECK_EMPTY_BUFFER())
    ;
  UART_WRITE_CHAR(c);
#else
  putByte(c);
#endif
}

void MidiUartClass::putByte(uint8_t c) {
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

void MidiUartClass::enable() {
  /** enable receive, transmit and receive and transmit interrupts. **/
  UCSR1B = _BV(RXEN1) | _BV(TXEN1) | _BV(RXCIE1);
#ifdef TX_IRQ
  UCSR1B |= _BV(TXCIE1);
#endif
	framingError = false;
}

void MidiUartClass::disable() {
  /** disable receive, transmit and receive and transmit interrupts. **/
  UCSR1B = 0;
  SET_BIT(DDRD, DDD3);
  CLEAR_BIT(PORTD, PORTD3);
}

bool MidiUartClass::isAvailable() {
  return !rxRb.isEmpty();
}

uint8_t MidiUartClass::getByte() {
  return rxRb.get();
}

SIGNAL(USART1_RX_vect) {
	if (IS_BIT_SET(UCSR1A, FE1)) {
		framingError = true;
	} else {
		if (framingError) {
			framingError = false;
		}
	}

  uint8_t c = UART_READ_CHAR();

	if (!framingError) {
		MidiUart.rxRb.put(c);
	}

#if 0
    // show overflow debug
    if (MidiUart.rxRb.overflow) {
      //      setLed();
    }
#endif
}

#ifdef TX_IRQ
SIGNAL(USART1_TX_vect) {
  if (!MidiUart.txRb.isEmpty()) {
    UART_WRITE_CHAR(MidiUart.txRb.peek());
    MidiUart.txRb.get();
  }
}
#endif
