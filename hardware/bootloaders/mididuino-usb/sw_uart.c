#include <avr/io.h>
#include <avr/interrupt.h>
#include "common.h"
#include "sw_uart.h"

volatile sw_uart_t sws[1];

#define SW_UART_CYCLES 3

void sw_uart_struct_init(volatile sw_uart_t *sw) {
  sw->rx_wait_for_stop_bit = 0;
  sw->qin = 0;
  sw->qout = 0;
  sw->tx_ready = 0;
  sw->rx_ready = 0;
  sw->rx_off   = 0;

  /* init io ports */
  SET_BIT(SW_UART_TXDDR, sw->tx_pin);
  CLEAR_BIT(SW_UART_RXDDR, sw->rx_pin);
  SW_UART_TX_HIGH(sw);
}

void sw_uart_init(void) {
  uint8_t sreg_tmp = SREG;
  cli();

  //  SET_BIT(DDRB, 1);
  //  CLEAR_BIT(PORTB, 1);
  // XXX hardcoded
  sws[0].rx_pin = 3;
  sws[0].tx_pin = 4;
  //  sws[1].rx_pin = 5;
  //  sws[1].tx_pin = 4;
  sw_uart_struct_init(&sws[0]);
  //  sw_uart_struct_init(&sws[1]);
  
  /* init timer */
  //  TCCR1B = _BV(CS10) | _BV(WGM12); /* no prescaling */
  TCCR1B = _BV(CS10) | _BV(WGM12); /* no prescaling */
  TCCR1A = 0;
  // XXX
  /* erstmal ganz schnell */
  //  OCR1A = (F_CPU / (10 * SW_UART_BAUDRATE)); /* 31250 * 3 bps at 12 Mhz */
  //  OCR1A = 20;
  OCR1A = (F_CPU / (SW_UART_CYCLES * SW_UART_BAUDRATE));
  //  OCR1B = (F_CPU / (SW_UART_CYCLES * SW_UART_BAUDRATE)); /* 31250 * 3 bps at 12 Mhz */
  //  OCR1A = 128;
  SET_BIT(TIMSK, OCIE1A);

  TCCR2 = _BV(CS20) | _BV(WGM21);
  OCR2 = (F_CPU / (SW_UART_CYCLES * SW_UART_BAUDRATE)) - 1;
  //  OCR2 = 0xFF;
  SET_BIT(TIMSK, OCIE2);
  //  SET_BIT(TIMSK, OCIE1B);
  TCNT1 = 0;
  TCNT2 = 0;

  SREG = sreg_tmp;
  sei();
}

uint8_t sw_uart_tx_ok(volatile sw_uart_t *sw) {
  return sw->tx_ready;
}

void sw_uart_putc(volatile sw_uart_t *sw, const uint8_t c) {
  while (sw->tx_ready == 1) {
    ;
  }

  sw->timer_tx_ctr = SW_UART_CYCLES;
  sw->bits_left_in_tx = SW_UART_TX_BITS;
  sw->tx_buffer = (c << 1) | 0x200;
  sw->tx_ready = 1;
}

void sw_uart_putc1(uint8_t c) {
  sw_uart_putc(&sws[0], c);
}

void sw_uart_putc2(uint8_t c) {
  sw_uart_putc(&sws[1], c);
}

void sw_uart_rx_on(volatile sw_uart_t *sw) {
  sw->rx_off = 0;
}

void sw_uart_rx_off(volatile sw_uart_t *sw) {
  sw->rx_off = 1;
}

void sw_uart_flush_input(volatile sw_uart_t *sw) {
  sw->qin = sw->qout = 0;
}

uint8_t sw_uart_avail(volatile sw_uart_t *sw) {
  return (sw->qin != sw->qout);
}
  
uint8_t sw_uart_getc(volatile sw_uart_t *sw) {
  uint8_t ch;

  /* return directly if no data available */
  if (sw->qout == sw->qin)
    return 0;

  ch = sw->inbuf[sw->qout];
  if (++sw->qout >= SW_UART_IN_BUF_SIZE)
    sw->qout = 0;

  return ch;
}

SIGNAL(TIMER2_COMP_vect) {
  volatile sw_uart_t *sw = &sws[0];
  /* TX handling */
  if (sw->tx_ready) {
    char tmp = sw->timer_tx_ctr;
    if (--tmp <= 0) {
      if (sw->tx_buffer & 1) {
	SET_BIT(SW_UART_TXPORT, sw->tx_pin);
      } else {
	CLEAR_BIT(SW_UART_TXPORT, sw->tx_pin);
      }
      
      sw->tx_buffer >>= 1;
      tmp = SW_UART_CYCLES;
      if (--sw->bits_left_in_tx <= 0) {
	sw->tx_ready = 0;
      }
    }
    sw->timer_tx_ctr = tmp;
  }

}

SIGNAL(TIMER1_COMPA_vect) {
  //  SET_BIT(PORTB, 1);
  //  asm("nop");
  //  CLEAR_BIT(PORTB, 1);

  volatile sw_uart_t *sw = &sws[0];
  // RX handling
  char start_bit, flag_in;
  
  if (sw->rx_wait_for_stop_bit) {
    if (--sw->timer_rx_ctr <= 0) {
      //      SET_BIT(PORTB, 1);
      sw->rx_wait_for_stop_bit = 0;
      sw->rx_ready = 0;
      //      OCR1A = F_CPU / (10 * SW_UART_BAUDRATE);
      sw->inbuf[sw->qin] = sw->rx_buffer;
      if (++sw->qin >= SW_UART_IN_BUF_SIZE) {
	/* XXX overflow */
	sw->qin = 0;
      }
    }
  } else {
    if (!sw->rx_ready) {
      start_bit = IS_BIT_SET(SW_UART_RXPIN, sw->rx_pin);
      if (start_bit == 0) {
	//	CLEAR_BIT(PORTB, 1);
	
	sw->rx_ready = 1;
	sw->rx_buffer = 0;
	sw->timer_rx_ctr = SW_UART_CYCLES + 1;
	// sw->timer_rx_ctr = 3;
	sw->bits_left_in_rx = SW_UART_RX_BITS;
	sw->rx_mask = 1;
	//	OCR1A = F_CPU / (SW_UART_CYCLES * SW_UART_BAUDRATE);
      }
    } else {
      if (--sw->timer_rx_ctr <= 0) {
	sw->timer_rx_ctr = SW_UART_CYCLES;
	flag_in = IS_BIT_SET(SW_UART_RXPIN, sw->rx_pin);
	if (flag_in) {
	  //	  SET_BIT(PORTB, 1);
	  sw->rx_buffer |= sw->rx_mask;
	} else {
	  //	  CLEAR_BIT(PORTB, 1);
	}
	sw->rx_mask <<= 1;
	if (--sw->bits_left_in_rx <= 0) {
	  sw->rx_wait_for_stop_bit = 1;
	}
      }
    }
  }
  
  return;
}
