#include <avr/interrupt.h>
#include <util/delay.h>

#include <midi-common.hh>
#include <MidiUart.h>

#include "USBMidi.h"

const midi_to_usb_t USBMidi::midi_to_usb[] = {
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
  { 0, (midi_state_t)0, 0}
};

void USBMidi::init() {
  last_status = 0;
  in_state = midi_ignore_message;
  in_msg.cable = 0x00;
  midi_buf_len = 0;
}

void USBMidi::handleByte(uint8_t byte)  {
 again:
  /* realtime messages have to be sent directly */
  if (MIDI_IS_REALTIME_STATUS_BYTE(byte)) {
    usb_midi_message_t msg;
    msg.cable = 0x00;
    msg.cin = CIN_SINGLE_BYTE;
    msg.msg[0] = byte;
    msg.msg[1] = msg.msg[2] = 0x00;
    sendMessage(&msg);
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

      sendMessage(&in_msg);
      in_state = midi_wait_status;
      return;
    }

    in_msg_len++;
    if (in_msg_len >= 3) {
      in_msg.cin = CIN_SYSEX_START;
      sendMessage(&in_msg);
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
	    sendMessage(&in_msg);
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
    sendMessage(&in_msg);
    in_state = midi_wait_status;
    break;

  case midi_wait_byte_2:
    in_msg.msg[in_msg_len++] = byte;
    in_state = midi_wait_byte_1;
    break;
  }
}

void USBMidi::sendMessage(usb_midi_message_t *msg) {
 again:
  if (midi_buf_len + 4 <= MIDI_BUF_SIZE) {
    midi_buf[midi_buf_len++] = (msg->cin) | (msg->cable << 4);
    midi_buf[midi_buf_len++] = msg->msg[0];
    midi_buf[midi_buf_len++] = msg->msg[1];
    midi_buf[midi_buf_len++] = msg->msg[2];
    return;
  } else {
    sendBuf();
      
    goto again;
  }
}


uint8_t USBMidi::handleUsbMessage(usb_midi_message_t *out_msg)  {
  static uint8_t  out_last_status = 0;

  if (out_msg->cable != 0) {
    return 0;
  }

  switch (out_msg->cin) {
  case CIN_SINGLE_BYTE:
  case CIN_SYSCOMMON_1BYTE:
    if (!MIDI_IS_REALTIME_STATUS_BYTE(out_msg->msg[0]))
      out_last_status = 0;

    uart->putByte(out_msg->msg[0]);
    break;
    
  case CIN_SYSCOMMON_2BYTES:
  case CIN_SYSEX_END_2BYTES:
  case CIN_PROGRAM_CHANGE:
  case CIN_CHANNEL_PRESSURE:
#if 0
    /** This would be running status. **/
    if (MIDI_IS_STATUS_BYTE(out_msg->msg[0]) &&
	(out_last_status != out_msg->msg[0])) {
      uart->putByte(out_msg->msg[0]);
      out_last_status = out_msg->msg[0];
    }
#endif
    uart->putByte(out_msg->msg[0]);
    uart->putByte(out_msg->msg[1]);
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
      uart->putByte(out_msg->msg[0]);
      out_last_status = out_msg->msg[0];
    }
#endif
    uart->putByte(out_msg->msg[0]);
    uart->putByte(out_msg->msg[1]);
    uart->putByte(out_msg->msg[2]);
    break;

  case CIN_MISC:
  case CIN_CABLE:
  default:
    break;
  }

  return 1;
}

