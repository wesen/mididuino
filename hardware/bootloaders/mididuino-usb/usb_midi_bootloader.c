/**# USB MIDI implementation
 **
 **/

#include "app.h"

#include "lcd.h"

#include <avr/interrupt.h>
#include <util/delay.h>
#include "common.h"
#include "usb.h"
#include "usb_midi.h"
#include "uart.h"
#include "d12.h"
#include "midi_link_desc.h"
#include "bootloader.h"

static usb_midi_message_t in_msg;

static uint8_t midi_buf[MIDI_BUF_SIZE];
static uint8_t midi_buf_len;

#define PRINTF_APP(args, ...)

/** Initialise the usb midi implementation (the receiving state machine).
 **/
void usb_midi_init(void) {
  in_msg.cable = 0x00;
  midi_buf_len = 0;
}

// XXX optimize sending without buffers
void usb_midi_send_buf(void) {
  if (midi_buf_len > 0) {
    if (!d12_write_ep_pkt(D12_MIDI_EP_IN, midi_buf, midi_buf_len))
      return;
    midi_buf_len = 0;
  }
}

/** Send a MIDI message on the usb interface by writing a usb package to the endpoint.
 **/
void usb_midi_send_message(usb_midi_message_t *msg) {
 again:
  if (midi_buf_len + 4 <= MIDI_BUF_SIZE) {
    midi_buf[midi_buf_len++] = (msg->cin) | (msg->cable << 4);
    midi_buf[midi_buf_len++] = msg->msg[0];
    midi_buf[midi_buf_len++] = msg->msg[1];
    midi_buf[midi_buf_len++] = msg->msg[2];
    return;
  } else {
    /** If midi send buffer is full, loop a bit through USB stack to
     ** get rid of the buffer (through midi EP in callback.
     **/
    usb_midi_send_buf();
    
    goto again;
  }
}

/**
 ** Handle an incoming USB MIDI message. Running status is commented
 ** out.
 **/
uint8_t usb_midi_handle_out_msg(uint8_t ep, usb_midi_message_t *out_msg) {
  static uint8_t in_sysex = 0;

  if (out_msg->cable != 0) {
    return 0;
  }

  if (out_msg->msg[0] == 0xF7)
    goto handle;

  switch (out_msg->cin) {
    /* 2 bytes */
  case CIN_SYSEX_END_2BYTES:
    if (!in_sysex) {
      goto end;
    }
    handle_sysex_byte(out_msg->msg[0]);
    handle_sysex_byte(out_msg->msg[1]);
    goto handle;

    /* 3 bytes */
  case CIN_SYSEX_START:
  case CIN_SYSEX_END_3BYTES:
    if (out_msg->msg[0] == 0xF0) {
      sysex_cnt = 0;
      in_sysex = 1;
    }
    handle_sysex_byte(out_msg->msg[0]);
    handle_sysex_byte(out_msg->msg[1]);
    handle_sysex_byte(out_msg->msg[2]);
    if (out_msg->cin == CIN_SYSEX_END_3BYTES) {
      goto handle;
    }
    break;

  default:
    break;
  }

  goto end;

 handle:
  handle_sysex();

 end:
  return 1;
}

