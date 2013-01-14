/*
 * MidiCtrl - Juno Sysex interface
 *
 * (c) January 2013 - Manuel Odendahl - wesen@ruinwesen.com
 */

#include "Platform.h"
#include "helpers.h"
#include "Juno.h"
#include "JunoSysex.h"

JunoSysexListenerClass JunoSysexListener;

void JunoSysexListenerClass::start() {
  msgType = 255;
}

void JunoSysexListenerClass::handleByte(uint8_t byte) {
  if (MidiSysex.len == 1) {
    msgType = byte;
  }
}

void JunoSysexListenerClass::end() {
  switch (msgType) {
  case JUNO_SYSEX_MSG_CONTROL_CHANGE:
    onControlChangeCallbacks.call(MidiSysex.data[1], MidiSysex.data[2], MidiSysex.data[3]);
    break;

  case JUNO_SYSEX_MSG_BANK_CHANGE:
    onBankChangeCallbacks.call(MidiSysex.data[1], MidiSysex.data[2], &Juno.kit.patches[0]);
    break;

  case JUNO_SYSEX_USER_MANUAL_MODE:
    onManualModeCallbacks.call(MidiSysex.data[1], MidiSysex.data[2], &Juno.kit.manual);
    break;

  default:
    break;
  }
}

void JunoSysexListenerClass::setup() {
  MidiSysex.addSysexListener(this);
}
