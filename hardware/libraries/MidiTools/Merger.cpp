#include <Midi.h>
#include <MidiUart.h>
#include "Merger.h"

void _merge_on2ByteCallback(uint8_t *msg);
void _merge_on3ByteCallback(uint8_t *msg);

void MergerSysexListener::end() {
  MidiUart.sendCommandByte(0xF0);
  MidiUart.puts(MidiSysex.data, MidiSysex.recordLen);
  MidiUart.sendCommandByte(0xF7);
}

void Merger::setMergeMask(uint8_t _mask) {
  mask = _mask;

  if (mask & MERGE_CC_MASK) {
    Midi2.addOnControlChangeCallback(_merge_on3ByteCallback);
  } else {
    Midi2.removeOnControlChangeCallback(_merge_on3ByteCallback);
  }
  if (mask & MERGE_NOTE_MASK) {
    Midi2.addOnNoteOnCallback(_merge_on3ByteCallback);
    Midi2.addOnNoteOffCallback(_merge_on3ByteCallback);
  } else {
    Midi2.removeOnNoteOnCallback(_merge_on3ByteCallback);
    Midi2.removeOnNoteOffCallback(_merge_on3ByteCallback);
  }
  if (mask & MERGE_AT_MASK) {
    Midi2.addOnAfterTouchCallback(_merge_on3ByteCallback);
  } else {
    Midi2.removeOnAfterTouchCallback(_merge_on3ByteCallback);
  }
  if (mask & MERGE_PRGCHG_MASK) {
    Midi2.addOnProgramChangeCallback(_merge_on2ByteCallback);
  } else {
    Midi2.removeOnProgramChangeCallback(_merge_on2ByteCallback);
  }
  if (mask & MERGE_CHANPRESS_MASK) {
    Midi2.addOnChannelPressureCallback(_merge_on2ByteCallback);
  } else {
    Midi2.removeOnChannelPressureCallback(_merge_on2ByteCallback);
  }
  if (mask & MERGE_PITCH_MASK) {
    Midi2.addOnPitchWheelCallback(_merge_on3ByteCallback);
  } else {
    Midi2.removeOnPitchWheelCallback(_merge_on3ByteCallback);
  }
  if (mask & MERGE_SYSEX_MASK) {
    MidiSysex.addSysexListener(&mergerSysexListener);
  } else {
    MidiSysex.removeSysexListener(&mergerSysexListener);
  }
}

void _merge_on2ByteCallback(uint8_t *msg) {
  MidiUart.sendMessage(msg[0], msg[1]);
}

void _merge_on3ByteCallback(uint8_t *msg) {
  MidiUart.sendMessage(msg[0], msg[1], msg[2]);
}
