#include "WProgram.h"
#include "helpers.h"

#include "Midi.h"

#ifndef HOST_MIDIDUINO
#include "MidiClock.h"
#endif

// #include "GUI.h"

#define MIDI_NOTE_OFF_CB    0
#define MIDI_NOTE_ON_CB     1
#define MIDI_AT_CB          2
#define MIDI_CC_CB          3
#define MIDI_PRG_CHG_CB     4
#define MIDI_CHAN_PRESS_CB  5
#define MIDI_PITCH_WHEEL_CB 6

const midi_parse_t midi_parse[] = {
  { MIDI_NOTE_OFF,         midi_wait_byte_2 },
  { MIDI_NOTE_ON,          midi_wait_byte_2 },
  { MIDI_AFTER_TOUCH,      midi_wait_byte_2 },
  { MIDI_CONTROL_CHANGE,   midi_wait_byte_2 },
  { MIDI_PROGRAM_CHANGE,   midi_wait_byte_1 },
  { MIDI_CHANNEL_PRESSURE, midi_wait_byte_1 },
  { MIDI_PITCH_WHEEL,      midi_wait_byte_2 },
  /* special handling for SYSEX */
  { MIDI_MTC_QUARTER_FRAME, midi_wait_byte_1 },
  { MIDI_SONG_POSITION_PTR, midi_wait_byte_2 },
  { MIDI_SONG_SELECT,       midi_wait_byte_1 },
  { MIDI_TUNE_REQUEST,      midi_wait_status },
  { 0, midi_ignore_message}
};

MidiClass::MidiClass(MidiUartParent *_uart) {
  midiActive = true;
  uart = _uart;
  receiveChannel = 0xFF;
  init();
  for (int i = 0; i < 7; i++) {
    callbacks[i] = NULL;
  }
}

void MidiClass::init() {
  last_status = running_status = 0;
  in_state = midi_ignore_message;
}

void MidiClass::handleByte(uint8_t byte) {
 again:
  if (MIDI_IS_REALTIME_STATUS_BYTE(byte)) {
#ifndef HOST_MIDIDUINO
    USE_LOCK();
    SET_LOCK();
    
    if (MidiClock.mode == MidiClock.EXTERNAL_MIDI) {
      switch (byte) {
      case MIDI_CLOCK:
	// handled in interrupt routine
	break;
	
      case MIDI_START:
	//	MidiClock.handleMidiStart();
	break;
	
      case MIDI_STOP:
	//	MidiClock.handleMidiStop();
	break;
      }
    }
    CLEAR_LOCK();
#endif
    return;
  }

  if (!midiActive)
    return;

  switch (in_state) {
  case midi_ignore_message:
    if (MIDI_IS_STATUS_BYTE(byte)) {
      in_state = midi_wait_status;
      goto again;
    } else {
      /* ignore */
    }
    break;

  case midi_wait_sysex:
    if (MIDI_IS_STATUS_BYTE(byte)) {
      if (byte != MIDI_SYSEX_END) {
	in_state = midi_wait_status;
	MidiSysex.abort();
	goto again;
      } else {
	MidiSysex.end();
      }
    } else {
      MidiSysex.handleByte(byte);
    }
    break;

  case midi_wait_status:
    {
      if (byte == MIDI_SYSEX_START) {
	in_state = midi_wait_sysex;
	MidiSysex.reset();
	last_status = running_status = 0;
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

      uint8_t status = last_status;
      if (MIDI_IS_VOICE_STATUS_BYTE(status)) {
	status = MIDI_VOICE_TYPE_NIBBLE(status);
      }

      uint8_t i;
      for (i = 0; midi_parse[i].midi_status != 0; i++) {
	if (midi_parse[i].midi_status == status) {
	  in_state = midi_parse[i].next_state;
	  msg[0] = last_status;
	  in_msg_len = 1;
	  break;
	}
      }
      callback = i;

      if (midi_parse[i].midi_status == 0) {
	in_state = midi_ignore_message;
	return;
      }
      if (running_status)
	goto again;
    }
    break;

  case midi_wait_byte_1:
    msg[in_msg_len++] = byte;
    //    /* XXX check callback, note off, params, etc... */
    if (midi_parse[callback].midi_status == MIDI_NOTE_ON && msg[2] == 0) {
      callback = 0; // XXX ugly hack to recgnize NOTE on with velocity 0 as Note Off
    }
    /*
    if (callback < 7 && callbacks[callback] != NULL) {
      callbacks[callback](msg);
    }
    */
    if (callback < 7) {
      for (int i = 0 ; i < midiCallbacks[callback].size; i++) {
	if (midiCallbacks[callback].arr[i] != NULL)
	  midiCallbacks[callback].arr[i](msg);
      }
    } else if (msg[0] == MIDI_SONG_POSITION_PTR) {
#ifndef HOST_MIDIDUINO
      MidiClock.handleSongPositionPtr(msg);
#endif
    }
    in_state = midi_wait_status;
    break;

  case midi_wait_byte_2:
    msg[in_msg_len++] = byte;
    in_state = midi_wait_byte_1;
    break;
  }
}

void MidiClass::addCallback(uint8_t num, midi_callback_t cb) {
  midiCallbacks[num].add(cb);
}
void MidiClass::removeCallback(uint8_t num, midi_callback_t cb) {
  midiCallbacks[num].remove(cb);
}
void MidiClass::setCallback(uint8_t num, midi_callback_t cb) {
  if (callbacks[num] != NULL) {
    removeCallback(num, callbacks[num]);
  }
  callbacks[num] = cb;
  addCallback(num, cb);
}

void MidiClass::addOnControlChangeCallback(midi_callback_t cb) {
  addCallback(MIDI_CC_CB, cb);
}
void MidiClass::removeOnControlChangeCallback(midi_callback_t cb) {
  removeCallback(MIDI_CC_CB, cb);
}
void MidiClass::setOnControlChangeCallback(midi_callback_t cb) {
  setCallback(MIDI_CC_CB, cb);
}

void MidiClass::addOnNoteOnCallback(midi_callback_t cb) {
  addCallback(MIDI_NOTE_ON_CB, cb);
}

void MidiClass::removeOnNoteOnCallback(midi_callback_t cb) {
  removeCallback(MIDI_NOTE_ON_CB, cb);
}
void MidiClass::setOnNoteOnCallback(midi_callback_t cb) {
  setCallback(MIDI_NOTE_ON_CB, cb);
}

void MidiClass::addOnNoteOffCallback(midi_callback_t cb) {
  addCallback(MIDI_NOTE_OFF_CB, cb);
}
void MidiClass::removeOnNoteOffCallback(midi_callback_t cb) {
  removeCallback(MIDI_NOTE_OFF_CB, cb);
}
void MidiClass::setOnNoteOffCallback(midi_callback_t cb) {
  setCallback(MIDI_NOTE_OFF_CB, cb);
}

void MidiClass::addOnAfterTouchCallback(midi_callback_t cb) {
  addCallback(MIDI_AT_CB, cb);
}
void MidiClass::removeOnAfterTouchCallback(midi_callback_t cb) {
  removeCallback(MIDI_AT_CB, cb);
}
void MidiClass::setOnAfterTouchCallback(midi_callback_t cb) {
  setCallback(MIDI_AT_CB, cb);
}

void MidiClass::addOnProgramChangeCallback(midi_callback_t cb) {
  addCallback(MIDI_PRG_CHG_CB, cb);
}
void MidiClass::removeOnProgramChangeCallback(midi_callback_t cb) {
  removeCallback(MIDI_PRG_CHG_CB, cb);
}
void MidiClass::setOnProgramChangeCallback(midi_callback_t cb) {
  setCallback(MIDI_PRG_CHG_CB, cb);
}

void MidiClass::addOnChannelPressureCallback(midi_callback_t cb) {
  addCallback(MIDI_CHAN_PRESS_CB, cb);
}
void MidiClass::removeOnChannelPressureCallback(midi_callback_t cb) {
  removeCallback(MIDI_CHAN_PRESS_CB, cb);
}
void MidiClass::setOnChannelPressureCallback(midi_callback_t cb) {
  setCallback(MIDI_CHAN_PRESS_CB, cb);
}

void MidiClass::addOnPitchWheelCallback(midi_callback_t cb) {
  addCallback(MIDI_PITCH_WHEEL_CB, cb);
}
void MidiClass::removeOnPitchWheelCallback(midi_callback_t cb) {
  removeCallback(MIDI_PITCH_WHEEL_CB, cb);
}
void MidiClass::setOnPitchWheelCallback(midi_callback_t cb) {
  setCallback(MIDI_PITCH_WHEEL_CB, cb);
}

