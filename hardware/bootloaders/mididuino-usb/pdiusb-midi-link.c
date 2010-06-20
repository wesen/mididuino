#include "app.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#include "common.h"
#include "d12.h"
#include "usb.h"
#include "midi_link_desc.h"
#include "uart.h"
#include "usb_midi.h"
#include "lcd.h"
#include "sr165.h"

void midi_uart_putc(uint8_t  byte) {
  //  sw_uart_putc(&sws[0], byte);
  uart_putc(byte);
}

/** Callback for outgoing MIDI messages. Send midi buffer if not empty. **/
void midi_ep_in_callback(uint8_t ep) {
  uint8_t buf[1];
  d12_read_cmd(D12_CMD_READ_LAST_TX_STATUS + ep, buf, 1);

  if (!d12_device_is_configured())
    return;

  usb_midi_send_buf();
}

/** Buffer for receiving midi messages. **/
static volatile uint8_t recv_bytes = 0;
static volatile uint8_t recv_buf[64];

void handle_midi_ep_out(void) {
  /** Read data from D12. **/
  recv_bytes = d12_read_ep_pkt(D12_MIDI_EP_OUT, recv_buf, sizeof(recv_buf));
  if (!(recv_bytes % 4)) {
    uint8_t count =0;
    uint8_t i;
    /** Dispatch to MIDI stack (same as on AT90USB1286 **/
    for (i = 0; i < recv_bytes; i += 4, count++) {
      if (!usb_midi_handle_out_msg(0, (usb_midi_message_t *)(recv_buf + i))) {
      	break;
      }
    }
  }
  
}

/** Process MIDI data from the host, called when data is received on
 ** the OUT endpoint.
 **/
void midi_ep_out_callback(uint8_t  ep) {
  uint8_t buf[1];
  d12_read_cmd(D12_CMD_READ_LAST_TX_STATUS + ep, buf, 1);
  if (!d12_device_is_configured())
    return;

  handle_midi_ep_out();
}

/** Main loop for MIDI. **/
void midi_link_main(void) {
  if (d12_device_is_configured()) {
    /** Receive data on UART if usb is configured. **/
    while (uart_avail()) {
      uint8_t c = uart_getc();
      usb_midi_handle_rx_byte(c);
    }

    /* try to send outgoing data as soon as possible */
    usb_midi_send_buf();
  }
}

#define LED_ON(led) { SET_BIT(PORTC, PC4); }
#define LED_OFF(led) { CLEAR_BIT(PORTC, PC4); }

/** Main routine for midi-link. **/
int main(void) {
  /** Disable watchdog. **/
  wdt_disable();

  /** setbits for the LEDs. **/
  //  DDRC |= _BV(5);
  DDRC |= _BV(4);

  sr165_init();
  
  lcd_init();
  lcd_line1();
  lcd_puts("HELO");

  for (;;) {
    uint16_t sr = sr165_read16();
    lcd_line1();
    lcd_putnumber16(sr);
  }
  
  /** Initialize UART. **/
  uart_init();
  usb_midi_init();

  /** Initialize D12 pins and stack. **/
  d12_pins_init();
  d12_init();

  /** Enable interrupts. **/
  sei();
  uint8_t uart_configured = 0;

  for (;;) {
    if (d12_device_is_configured() && (uart_configured == 0)) {
      /* discard data in uart buffer */
      while (uart_avail()) {
	uart_getc();
      }
      uart_configured = 1;
    }

    if (uart_configured < 100) {
      _delay_ms(10);
      uart_configured++;
    } else {
      /** Handle midi from UART and from USB. **/
      midi_link_main();
    }

    /** Handle usb status. **/
    d12_main();

    /* check ep2 again because of d12 bug */
    uint8_t status;
    do {
      d12_read_cmd(D12_CMD_SELECT_EP + D12_MIDI_EP_OUT, &status, 1);
      if ((status & 1) && d12_device_is_configured() && (uart_configured > 100)) {
	handle_midi_ep_out();
      }
    } while (status & 1);

  }
}

