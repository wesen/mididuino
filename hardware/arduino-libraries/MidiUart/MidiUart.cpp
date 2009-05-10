#include <MidiUart.h>
#include <midi-common.hh>
#include <MidiClock.h>
#include "RingBuffer.h"

MidiUartClass MidiUart;

#define IS_BIT_SET(port, bit) (((port) & (uint8_t)(1 << (bit))) ? 1 : 0)

#define UART_CHECK_EMPTY_BUFFER() IS_BIT_SET(UCSRA, UDRE)
#define UART_CHECK_RX() IS_BIT_SET(UCSRA, RXC)
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

MidiUartClass::MidiUartClass() {
  currentChannel = 0x0;
}

void MidiUartClass::init(long speed) {
  Serial.begin(speed);

  TCCR1A = _BV(WGM10); //  | _BV(COM1A1) | _BV(COM1B1); 
  TCCR1B = _BV(CS10) | _BV(WGM12); // every cycle
  TIMSK1 |= _BV(TOIE1);

  #ifdef TX_IRQ
  UCSRB |= _BV(TXCIE);
#endif

}

void MidiUartClass::puts(uint8_t *data, uint8_t cnt) {
  while (cnt--)
    putc(*(data++));
}

#ifdef TX_IRQ
SIGNAL(USART_TXC_vect) {
  if (!MidiUart.txRb.isEmpty())
    UART_WRITE_CHAR(MidiUart.txRb.get());
}
#endif


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
  Serial.print(c, BYTE);
#endif
}

void MidiUartClass::putc_immediate(uint8_t c) {
#ifdef TX_IRQ
  while (!UART_CHECK_EMPTY_BUFFER())
    ;
  UART_WRITE_CHAR(c);
#else
  Serial.print(c, BYTE);
#endif
}

bool MidiUartClass::avail() {
  return Serial.available();
}

uint8_t MidiUartClass::getc() {
  return Serial.read();
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

volatile uint16_t clock = 0;

ISR(TIMER1_OVF_vect) {
  if (MidiClock.state == MidiClock.STARTED) {
    clock++;
    MidiClock.handleTimerInt();
  }
}

