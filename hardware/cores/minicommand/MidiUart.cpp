#include <avr/interrupt.h>
#include <util/delay.h>

#include <MidiUart.h>
#include <MidiClock.h>
#include <midi-common.hh>

MidiUartClass MidiUart;
MidiUartClass2 MidiUart2;

#define UART_BAUDRATE 31250
#define UART_BAUDRATE_REG (((F_CPU / 16)/(UART_BAUDRATE)) - 1)

#define UART_CHECK_EMPTY_BUFFER() IS_BIT_SET8(UCSR0A, UDRE)
#define UART_CHECK_RX() IS_BIT_SET8(UCSR0A, RXC)
#define UART_WRITE_CHAR(c) (UDR0 = (c))
#define UART_READ_CHAR() (UDR0)

#define UART2_CHECK_RX() IS_BIT_SET8(UCSR1A, RXC)
#define UART2_READ_CHAR() (UDR1)

#include <avr/io.h>

MidiUartClass::MidiUartClass() {
  currentChannel = 0x0;
  initSerial();
}

void MidiUartClass::initSerial() {
  UBRR0H = (UART_BAUDRATE_REG >> 8);
  UBRR0L = (UART_BAUDRATE_REG & 0xFF);
  //  UBRRH = 0;
  //  UBRRL = 15;

  UCSR0C = (3<<UCSZ00); 
  
  /** enable receive, transmit and receive and transmit interrupts. **/
  //  UCSRB = _BV(RXEN) | _BV(TXEN) | _BV(RXCIE);
  UCSR0B = _BV(RXEN) | _BV(TXEN) | _BV(RXCIE);
#ifdef TX_IRQ
  UCSR0B |= _BV(TXCIE);
#endif
}

void MidiUartClass::puts(uint8_t *data, uint8_t cnt) {
  while (cnt--)
    putc(*(data++));
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

void MidiUartClass::sendNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
  putc(MIDI_NOTE_ON | channel);
  putc(note);
  putc(velocity);
}

void MidiUartClass::sendNoteOff(uint8_t channel, uint8_t note, uint8_t velocity) {
  putc(MIDI_NOTE_OFF | channel);
  putc(note);
  putc(velocity);
}

void MidiUartClass::sendCC(uint8_t channel, uint8_t cc, uint8_t value) {
  putc(MIDI_CONTROL_CHANGE | channel);
  putc(cc);
  putc(value);
}

void MidiUartClass::sendProgramChange(uint8_t channel, uint8_t program) {
  putc(MIDI_PROGRAM_CHANGE | channel);
  putc(program);
}

void MidiUartClass::sendPolyKeyPressure(uint8_t channel, uint8_t note, uint8_t pressure) {
  putc(MIDI_AFTER_TOUCH | channel);
  putc(note);
  putc(pressure);
}

void MidiUartClass::sendChannelPressure(uint8_t channel, uint8_t pressure) {
  putc(MIDI_CHANNEL_PRESSURE | channel);
  putc(pressure);
}

void MidiUartClass::sendPitchBend(uint8_t channel, int16_t pitchbend) {
  putc(MIDI_PITCH_WHEEL | channel);
  pitchbend += 8192;
  putc(pitchbend & 0x7F);
  putc((pitchbend >> 7) & 0x7F);
}

void MidiUartClass::sendNRPN(uint8_t channel, uint16_t parameter, uint8_t value) {
  sendCC(channel, 99, (parameter >> 7) & 0x7F);
  sendCC(channel, 98, (parameter & 0x7F));
  sendCC(channel, 6, value);
}
void MidiUartClass::sendNRPN(uint8_t channel, uint16_t parameter, uint16_t value) {
  sendCC(channel, 99, (parameter >> 7) & 0x7F);
  sendCC(channel, 98, (parameter & 0x7F));
  sendCC(channel, 6, (value >> 7) & 0x7F);
  sendCC(channel, 38, (value & 0x7F));
}

void MidiUartClass::sendRPN(uint8_t channel, uint16_t parameter, uint8_t value) {
  sendCC(channel, 101, (parameter >> 7) & 0x7F);
  sendCC(channel, 100, (parameter & 0x7F));
  sendCC(channel, 6, value);
}
void MidiUartClass::sendRPN(uint8_t channel, uint16_t parameter, uint16_t value) {
  sendCC(channel, 101, (parameter >> 7) & 0x7F);
  sendCC(channel, 100, (parameter & 0x7F));
  sendCC(channel, 6, (value >> 7) & 0x7F);
  sendCC(channel, 38, (value & 0x7F));
}

SIGNAL(USART0_RX_vect) {
  uint8_t c = UART_READ_CHAR();

  if (c == 0xF8 && MidiClock.mode == MidiClock.EXTERNAL) {
    MidiClock.handleClock();
  } else {
    MidiUart.rxRb.put(c);
  }
}

#ifdef TX_IRQ
SIGNAL(USART0_TX_vect) {
  if (!MidiUart.txRb.isEmpty()) {
    UART_WRITE_CHAR(MidiUart.txRb.peek());
    MidiUart.txRb.get();
  }
}
#endif

MidiUartClass2::MidiUartClass2() {
  initSerial();
}

void MidiUartClass2::initSerial() {
  UBRR1H = (UART_BAUDRATE_REG >> 8);
  UBRR1L = (UART_BAUDRATE_REG & 0xFF);
  //  UBRRH = 0;
  //  UBRRL = 15;

  UCSR1C = (3<<UCSZ00); 
  
  /** enable receive, transmit and receive and transmit interrupts. **/
  //  UCSRB = _BV(RXEN) | _BV(TXEN) | _BV(RXCIE);
  UCSR1B = _BV(RXEN) | _BV(RXCIE);
}

bool MidiUartClass2::avail() {
  return !rxRb.isEmpty();
}

uint8_t MidiUartClass2::getc() {
  return rxRb.get();
}

SIGNAL(USART1_RX_vect) {
  uint8_t c = UART2_READ_CHAR();

  // XXX clock on second input
  if (c == 0xF8 && MidiClock.mode == MidiClock.EXTERNAL_UART2) {
    MidiClock.handleClock();
  } else {
    MidiUart2.rxRb.put(c);
  }
}

