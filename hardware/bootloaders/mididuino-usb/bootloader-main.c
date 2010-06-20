#include "app.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>

#include "common.h"
#include "d12.h"
#include "usb.h"
#include "midi_link_desc.h"
#include "lcd.h"
#include "sr165.h"
#include "usb_midi.h"
#include "bootloader.h"

usb_midi_message_t rw_msg =
  { CIN_SYSEX_START, 0, { 0xf0, 0x00, 0x13 } };
  
usb_midi_message_t usb_ack_msg = 
  { CIN_SYSEX_END_3BYTES, 0, {SYSEX_VENDOR_3, CMD_DATA_BLOCK_ACK, 0xf7 } };

usb_midi_message_t usb_nak_msg =
  { CIN_SYSEX_END_3BYTES, 0, {SYSEX_VENDOR_3, CMD_DATA_BLOCK_NAK, 0xf7 } };

void midi_sysex_send_ack(void) {
  usb_midi_send_message(&rw_msg);
  usb_midi_send_message(&usb_ack_msg);
}

void midi_sysex_send_nak(void) {
  usb_midi_send_message(&rw_msg);
  usb_midi_send_message(&usb_nak_msg);
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
static uint8_t recv_bytes = 0;
static uint8_t recv_buf[64];

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
    /* try to send outgoing data as soon as possible */
    usb_midi_send_buf();
  }
}

uint8_t butstat;

#define LED_ON(led) { SET_BIT(PORTC, PC4); }
#define LED_OFF(led) { CLEAR_BIT(PORTC, PC4); }

/** Main routine for midi-link. **/
int main(void) {
  /** Disable watchdog. **/
  cli();
  asm("wdr");
  wdt_disable();

#ifdef _AVR_IOM168_H_
  MCUCR = _BV(IVCE) | _BV(IVSEL);
#else
  GICR = _BV(IVCE) | _BV(IVSEL);
#endif

  /** setbits for the LEDs. **/
  //  DDRC |= _BV(5);
  DDRC |= _BV(4);

  sr165_init();
  
  /** Initialize D12 pins and stack. **/
  d12_pins_init();
  d12_init();

  /** Enable interrupts. **/
  uint8_t midi_init = 0;

  lcd_init();
  
  butstat = sr165_read16() >> 8;
  if (eeprom_read_word(START_MAIN_APP_ADDR) == 1 && IS_BIT_SET8(butstat, 4)) {
    jump_to_main_program();
  }
  uint8_t button = 0;

  //  lcd_line1();
  //  lcd_puts("BOOTLOADER");

  //  sei();
  
  for (;;) {
    //    cli();
    butstat = sr165_read16() >> 8;
    
    //    sei();
    if (IS_BIT_SET8(butstat, 4)) {
      button = 1;
    } else {
      if (button) {
	jump_to_main_program();
      }
    }

    /** Handle midi from UART and from USB. **/
    midi_link_main();

    /** Handle usb status. **/
    d12_main();
    
    if (d12_device_is_configured() && !midi_init) {
      midi_sysex_send_ack();
      midi_init = 1;
    }
  }
}

