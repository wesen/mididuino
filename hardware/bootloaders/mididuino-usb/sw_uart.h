#ifndef SW_UART_H__
#define SW_UART_H__

#include "app.h"
#include "common.h"

#define SW_UART_BAUDRATE 31250UL
#define SW_UART_TXDDR DDRB
#define SW_UART_RXDDR DDRB

#define SW_UART_TXPORT PORTB
#define SW_UART_RXPIN  PINB

#define SW_UART_TX_HIGH(sw) (SET_BIT(SW_UART_TXPORT, (sw)->tx_pin))
#define SW_UART_TX_LOW(sw)  (CLEAR_BIT(SW_UART_TXPORT, (sw)->tx_pin))
#define SW_UART_GET_RX(sw)  (SW_UART_RXPIN & (1 << (sw)->rx_pin))

#define SW_UART_IN_BUF_SIZE (64)

// stop bit handled by interrupt routine
#define SW_UART_RX_BITS     (8)
// 8 bits + startbit + stopbit
#define SW_UART_TX_BITS     (10)

typedef struct sw_uart_s {
  uint8_t rx_wait_for_stop_bit;
  uint8_t rx_mask;
  char timer_rx_ctr;
  char bits_left_in_rx;
  uint8_t rx_buffer;
  uint8_t rx_off;
  uint8_t rx_ready;
  uint8_t inbuf[SW_UART_IN_BUF_SIZE];
  uint8_t qin, qout;

  uint8_t rx_pin;
  uint8_t tx_pin;

  uint8_t tx_ready;
  char timer_tx_ctr;
  char bits_left_in_tx;
  uint16_t tx_buffer;
} sw_uart_t;

extern volatile sw_uart_t sws[1];

void sw_uart_init();
uint8_t sw_uart_avail(volatile sw_uart_t *sw);
uint8_t sw_uart_getc(volatile sw_uart_t *sw);
void sw_uart_putc(volatile sw_uart_t *sw, uint8_t c);
void sw_uart_putc1(uint8_t c);
void sw_uart_putc2(uint8_t c);
void sw_uart_rx_off(volatile sw_uart_t *sw);
void sw_uart_rx_on(volatile sw_uart_t *sw);
uint8_t sw_uart_tx_ok(volatile sw_uart_t *sw);

#endif /* SW_UART_H__ */
