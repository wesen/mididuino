#include <avr/io.h>
#include <util/delay.h>

#define CALC_BAUDRATE(br) (((F_CPU) / (16 * BAUDRATE)) - 1)

template <uint32_t BAUDRATE> class Uart {
  USART_t *usart;
  PORT_t *port;
  uint32_t baudrate;
  
public:
  Uart(USART_t *_usart, PORT_t *_port)
    : usart(_usart), port(_port) {

    port->DIRSET |= PIN3_bm; // TX_PIN

    baudrate = BAUDRATE;
    usart->BAUDCTRLA = CALC_BAUDRATE(BAUDRATE) & 0xFF;
    usart->BAUDCTRLB = (CALC_BAUDRATE(BAUDRATE) >> 8) & 0xF;
    usart->CTRLB = USART_TXEN_bm | USART_RXEN_bm;
  }

  void putByte(uint8_t byte) {
    usart->DATA = byte;
    while (!(usart->STATUS & USART_TXCIF_bm))
      ;
    usart->STATUS |= USART_TXCIF_bm;
  }

  void puts(const char *bytes) {
    uint8_t i = 0;
    while (bytes[i] != 0) {
      putByte(bytes[i]);
      i++;
    }
  }
  
  void puts(const uint8_t *bytes, uint8_t cnt) {
    for (uint8_t i = 0; i < cnt; i++) {
      putByte(bytes[i]);
    }
  }

  uint8_t getByte() {
    while (!(usart->STATUS & USART_RXCIF_bm))
      ;
    return usart->DATA;
  }

  bool isAvailable() {
  }
};

Uart<10540> uart(&USARTC0, &PORTC);
Uart<10540> uart2(&USARTD0, &PORTD);

int main(void) {

  PORTE.DIRSET = 0xFF;
  PORTE.OUT = 0xa5;
  
  for (;;) {
    PORTE.OUT = ~PORTE.OUT;
    uint8_t byte = uart.getByte();
    uart.putByte(byte);
  }

  return 0;
}
