/**# USB MIDI implementation
 **
 **/

#include "app.h"

#include <avr/interrupt.h>
#include <util/delay.h>
#include "common.h"
#include "usb.h"
#include "usb_midi.h"
#include "uart.h"
#include "d12.h"
#include "midi_link_desc.h"

/**
 ** Conversion table from "real" midi to USB MIDI.
 **
 ** First parameter is the "real" midi message type, second is the
 ** state-machine state used to parse the incoming "real" midi message.
 **/
const midi_to_usb_t midi_to_usb[] = {
  { MIDI_NOTE_OFF,         midi_wait_byte_2,     CIN_NOTEOFF },
  { MIDI_NOTE_ON,          midi_wait_byte_2,     CIN_NOTEON },
  { MIDI_AFTER_TOUCH,      midi_wait_byte_2,     CIN_POLYKEYPRESS }, /* ???? */
  { MIDI_CONTROL_CHANGE,   midi_wait_byte_2,     CIN_CONTROL_CHANGE },
  { MIDI_PROGRAM_CHANGE,   midi_wait_byte_1,     CIN_PROGRAM_CHANGE },
  { MIDI_CHANNEL_PRESSURE, midi_wait_byte_1,     CIN_CHANNEL_PRESSURE },
  { MIDI_PITCH_WHEEL,      midi_wait_byte_2,     CIN_PITCHBEND_CHANGE },
  /* special handling for SYSEX */
  { MIDI_MTC_QUARTER_FRAME, midi_wait_byte_1,    CIN_SYSCOMMON_2BYTES },
  { MIDI_SONG_POSITION_PTR, midi_wait_byte_2,    CIN_SYSCOMMON_3BYTES },
  { MIDI_SONG_SELECT,       midi_wait_byte_1,    CIN_SYSCOMMON_2BYTES },
  { MIDI_TUNE_REQUEST,      midi_wait_status,    CIN_SYSCOMMON_1BYTE },
  { 0, 0, 0}
};

/** volatile is important -> gcc bug **/

static volatile midi_state_t       in_state;
static volatile uint8_t            last_status = 0;
static volatile uint8_t            running_status = 0;
static volatile uint8_t            in_msg_len;
static volatile usb_midi_message_t in_msg;

static volatile uint8_t midi_buf[MIDI_BUF_SIZE];
static volatile uint8_t midi_buf_len;

#define PRINTF_APP(args, ...)

/** Initialise the usb midi implementation (the receiving state machine).
 **/
void usb_midi_init(void) {
  last_status = 0;
  in_state = midi_ignore_message;
  in_msg.cable = 0x00;
  midi_buf_len = 0;
}

// XXX optimize sending without buffers
void usb_midi_send_buf(void) {
  if (midi_buf_len > 0) {
    uint8_t status;
    if (!d12_write_ep_pkt(D12_MIDI_EP_IN, midi_buf, midi_buf_len))
      return;
    midi_buf_len = 0;
  }
}

/** handle a byte from the MIDI IN port. It parses midi messages and
 ** generates USB messages appropriately.
 **/
void usb_midi_handle_rx_byte(uint8_t  byte) {
 again:
  /* realtime messages have to be sent directly */
  if (MIDI_IS_REALTIME_STATUS_BYTE(byte)) {
    usb_midi_message_t msg;
    msg.cable = 0x00;
    msg.cin = CIN_SINGLE_BYTE;
    msg.msg[0] = byte;
    msg.msg[1] = msg.msg[2] = 0x00;
    usb_midi_send_message(&msg);
    return;
  }

  switch (in_state) {
  case midi_ignore_message:
    if (MIDI_IS_STATUS_BYTE(byte)) {
      in_state = midi_wait_status;
      goto again;
    } else {
      /* ignore */
    }
    break;

    /* handle sysex messages */
  case midi_wait_sysex:
    if (MIDI_IS_STATUS_BYTE(byte) && (byte != MIDI_SYSEX_END)) {
      /* a status byte interrupts a sysex request */
      in_state = midi_wait_status;
      goto again;
    }

    /* copy byte into message */
    in_msg.msg[in_msg_len] = byte;
      
    if (byte == MIDI_SYSEX_END) {
      /* end a sysex request */
      if (in_msg_len == 0) {
	in_msg.cin = CIN_SYSCOMMON_1BYTE;
	in_msg.msg[1] = in_msg.msg[2] = 0;
      } else if (in_msg_len == 1) {
	in_msg.cin = CIN_SYSEX_END_2BYTES;
	in_msg.msg[2] = 0;
      } else if (in_msg_len == 2) {
	in_msg.cin = CIN_SYSEX_END_3BYTES;
      }

      usb_midi_send_message(&in_msg);
      in_state = midi_wait_status;
      return;
    }

    in_msg_len++;
    if (in_msg_len >= 3) {
      in_msg.cin = CIN_SYSEX_START;
      usb_midi_send_message(&in_msg);
      in_msg_len = 0;
    }
    break;
    
  case midi_wait_status:
    {
      /* special handling for sysex */
      if (byte == MIDI_SYSEX_START) {
	in_state = midi_wait_sysex;
	in_msg_len = 1;
	in_msg.cin = CIN_SYSEX_START;
	in_msg.msg[0] = byte;
	running_status = 0;
	last_status = 0;
	return;
      }
      
      if (MIDI_IS_STATUS_BYTE(byte)) {
	last_status = byte;
	running_status = 0; 
      } else {
	if (last_status == 0)
	  break;
	running_status = 1;
      }
      
      uint8_t  status = last_status;
      if (MIDI_IS_VOICE_STATUS_BYTE(status)) {
	status = MIDI_VOICE_TYPE_NIBBLE(status);
      }
      
      int i;
      for (i = 0; midi_to_usb[i].midi_status != 0; i++) {
	if (midi_to_usb[i].midi_status == status) {
	  in_msg.cin = midi_to_usb[i].cin;
	  in_state = midi_to_usb[i].next_state;
	  in_msg.msg[0] = last_status;
	  in_msg_len = 1;
	  
	  /* if we don't have to wait for data send message */
	  if (in_state == midi_wait_status)
	    usb_midi_send_message(&in_msg);
	  break;
	}
      }
      /* status byte not found */
      if (midi_to_usb[i].midi_status == 0) {
	in_state = midi_ignore_message;
	return;
      }
      if (running_status) {
	goto again;
      }
    }
    break;

  case midi_wait_byte_1:
    in_msg.msg[in_msg_len++] = byte;
    usb_midi_send_message(&in_msg);
    in_state = midi_wait_status;
    break;

  case midi_wait_byte_2:
    in_msg.msg[in_msg_len++] = byte;
    in_state = midi_wait_byte_1;
    break;
  }
}

/**
 ** helper to send 16 bit value over sysex
 **/
void usb_midi_send_time(uint16_t timer) {
  uint8_t buf[16];
  uint8_t i = 0;
  buf[i++] = 0xf0;

  if (timer == 0) {
    buf[i++] = '0';
  } else {
    for (; (i < 14) && (timer != 0) ; i++) {
      uint8_t ch =  timer & 0xf;
      if (ch < 10)
	buf[i] = '0' + ch;
      else
	buf[i] = 'a' + (ch - 10);
      timer >>= 4;
    }
  }
  uint8_t len = i;
  buf[len] = 0xF7;
  if (i > 1) {
    for (i--; i > (len / 2); i--) {
      uint8_t tmp = buf[len - i];
      buf[len - i] = buf[i];
      buf[i] = tmp;
    }
  }
  usb_midi_send_sysex(buf, len + 1);
}

/**
 ** helper function to send a description of an array over MIDI to
 ** debug on the receiving side.
 **/
void usb_midi_send_key_state(uint8_t *arr, uint8_t len) {
  uint8_t buf[32];
  uint8_t i = 0;
  uint8_t j = 0;
  buf[i++] = 0xf0;

  for (j = 0; j < len; j++) {
    uint8_t ch = arr[j] & 0xf;
    if (ch < 10)
      buf[i++] = '0' + ch;
    else
      buf[i++] = 'a' + (ch - 10);
    ch = (arr[j] >> 4) & 0xf;
    if (ch < 10)
      buf[i++] = '0' + ch;
    else
      buf[i++] = 'a' + (ch - 10);
  }
  buf[i++] = 0xF7;
  usb_midi_send_sysex(buf, i);
}

/** Send a MIDI message on the usb interface by writing a usb package to the endpoint.
 **/
void usb_midi_send_message(usb_midi_message_t *msg) {
  if (msg->msg[0] != 0xFE)
    PRINTF_APP("in_msg: cin: %x, b0: %x, b1: %x, b2: %x\n",
	       msg->cin, msg->msg[0], msg->msg[1], msg->msg[2]);

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
 ** Send a note message on the given channel, with given code, key and
 ** velocity.
 **/
void usb_midi_send_note_message(uint8_t channel, uint8_t code, uint8_t key, uint8_t velocity) {
  usb_midi_message_t msg;
  if (key > 0xF7)
    return;
  msg.cable = 0x00;
  if (code == MIDI_NOTE_ON) {
    msg.cin = CIN_NOTEON;
  } else {
    msg.cin = CIN_NOTEOFF;
  }
  msg.msg[0] = code | channel;
  msg.msg[1] = key;
  msg.msg[2] = velocity;

  usb_midi_send_message(&msg);
}

/**
 ** Send a program change on the given channel.
 **/
void usb_midi_send_program_change(uint8_t channel, uint8_t program) {
  usb_midi_message_t msg;
  if (program > 0x7F)
    return;
  msg.cable = 0x00;
  msg.cin = CIN_PROGRAM_CHANGE;
  msg.msg[0] = MIDI_PROGRAM_CHANGE | channel;
  msg.msg[1] = program;
  msg.msg[2] = 0;

  usb_midi_send_message(&msg);
}

/**
 ** Send a channel pressure message on the given channel.
 **/
void usb_midi_send_channel_pressure(uint8_t channel, uint8_t pressure) {
  usb_midi_message_t msg;
  if (pressure > 0x7F)
    return;
  msg.cable = 0x00;
  msg.cin = CIN_CHANNEL_PRESSURE;
  msg.msg[0] = MIDI_CHANNEL_PRESSURE | channel;
  msg.msg[1] = pressure;
  msg.msg[2] = 0;

  usb_midi_send_message(&msg);
}

/**
 ** Send a sysex message.
 **/
void usb_midi_send_sysex(uint8_t *buf, uint16_t len) {
  usb_midi_message_t msg;
  int i;

  msg.cable = 0x00;
  
  for (i = 0; i < len - 3; i+=3) {
    msg.cin = CIN_SYSEX_START;
    msg.msg[0] = buf[i];
    msg.msg[1] = buf[i+1];
    msg.msg[2] = buf[i+2];
    usb_midi_send_message(&msg);
  }
  uint8_t togo = len - i;
  if (togo == 1) {
    msg.cin = CIN_SYSCOMMON_1BYTE;
    msg.msg[0] = buf[i];
    msg.msg[1] = 0;
    msg.msg[2] = 0;
    usb_midi_send_message(&msg);
  } else if (togo == 2) {
    msg.cin = CIN_SYSEX_END_2BYTES;
    msg.msg[0] = buf[i];
    msg.msg[1] = buf[i+1];
    msg.msg[2] = 0;
    usb_midi_send_message(&msg);
  } else if (togo == 3) {
    msg.cin = CIN_SYSEX_END_3BYTES;
    msg.msg[0] = buf[i];
    msg.msg[1] = buf[i+1];
    msg.msg[2] = buf[i+2];
    usb_midi_send_message(&msg);
  }

}

/**
 ** Send a debug string stored in program memory as sysex.
 **/
void usb_midi_send_debug(PGM_VOID_P buf, uint16_t len) {
  uint8_t buf2[len + 2];
  buf2[0] = 0xF0;
  uint8_t i;
  for (i = 0; i < len; i++) {
    buf2[i+1] = pgm_read_byte_near(buf + i);
  }
  buf2[i+1] = 0xF7;
  usb_midi_send_sysex(buf2, len + 2);
}

/** Write a midi byte to the serial interface (MIDI OUT). **/
#if 0
void midi_uart_putc(uint8_t  byte) {
  PRINTF_APP("out: %x\n", byte);
  uart_putc(byte);
}
#endif

/**
 ** Handle an incoming USB MIDI message. Running status is commented
 ** out.
 **/
int usb_midi_handle_out_msg(uint8_t ep, usb_midi_message_t *out_msg) {
  static uint8_t  out_last_status = 0;

  PRINTF_APP("out_msg: cin: %x, b0: %x, b1: %x, b2: %x\n",
	     out_msg->cin, out_msg->msg[0], out_msg->msg[1], out_msg->msg[2]);

  if (out_msg->cable != 0) {
    PRINTF_APP("No such cable: %d\n", out_msg->cable);
    return 0;
  }

  switch (out_msg->cin) {
  case CIN_SINGLE_BYTE:
  case CIN_SYSCOMMON_1BYTE:
    if (!MIDI_IS_REALTIME_STATUS_BYTE(out_msg->msg[0]))
      out_last_status = 0;

    midi_uart_putc(out_msg->msg[0]);
    break;

  case CIN_SYSCOMMON_2BYTES:
  case CIN_SYSEX_END_2BYTES:
  case CIN_PROGRAM_CHANGE:
  case CIN_CHANNEL_PRESSURE:
#if 0
    /** This would be running status. **/
    if (MIDI_IS_STATUS_BYTE(out_msg->msg[0]) &&
	(out_last_status != out_msg->msg[0])) {
      midi_uart_putc(out_msg->msg[0]);
      out_last_status = out_msg->msg[0];
    }
#endif
    midi_uart_putc(out_msg->msg[0]);
    midi_uart_putc(out_msg->msg[1]);
    break;

  case CIN_SYSCOMMON_3BYTES:
  case CIN_SYSEX_START:
  case CIN_SYSEX_END_3BYTES:
  case CIN_NOTEON:
  case CIN_NOTEOFF:
  case CIN_POLYKEYPRESS:
  case CIN_CONTROL_CHANGE:
  case CIN_PITCHBEND_CHANGE:
#if 0
    /** This vould be running status. **/
    if (MIDI_IS_STATUS_BYTE(out_msg->msg[0]) &&
	(out_last_status != out_msg->msg[0])) {
      midi_uart_putc(out_msg->msg[0]);
      out_last_status = out_msg->msg[0];
    }
#endif
    midi_uart_putc(out_msg->msg[0]);
    midi_uart_putc(out_msg->msg[1]);
    midi_uart_putc(out_msg->msg[2]);
    break;

  case CIN_MISC:
  case CIN_CABLE:
  default:
    PRINTF_APP("Ignoring undefined midi request\n");
    break;
  }

  return 1;
}

