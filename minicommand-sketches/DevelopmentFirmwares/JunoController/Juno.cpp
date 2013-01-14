/*
 * MidiCtrl - Juno class
 *
 * (c) January 2013 - Manuel Odendahl - wesen@ruinwesen.com
 */

#include "Platform.h"
#include "Juno.h"
#include "helpers.h"

#include "MidiUartParent.hh"

void JunoClass::sendParameter(uint8_t channel, uint8_t idx, uint8_t value) {
  kit.patches[channel].parameters[idx] = value;
  uint8_t msg[] = {
      0xF0, ROLAND_MANUFACTURER_ID, JUNO_SYSEX_MSG_CONTROL_CHANGE, channel, idx, value, 0xF7
  };
  MidiUart.puts(msg, sizeof(msg));
}

void JunoClass::setModulation(uint8_t channel, bool modulation) {
  kit.patches[channel].modulation = modulation;
  MidiUart.sendCC(channel, JUNO_CC_MODULATION, modulation ? 0x7F : 0x00);
}

void JunoClass::setPedal(uint8_t channel, bool pedal) {
  kit.patches[channel].pedal = pedal;
  MidiUart.sendCC(channel, JUNO_CC_PEDAL, pedal ? 0x7F : 0x00);
}

void JunoClass::sendPatch(uint8_t channel) {
  setModulation(channel, kit.patches[channel].modulation);
  setPedal(channel, kit.patches[channel].pedal);
  for (uint8_t i = 0; i < JUNO_MAX_PARAMETERS; i++) {
    sendParameter(channel, i, kit.patches[channel].parameters[i]);
  }
}

void JunoClass::sendKit() {
  for (uint8_t channel = 0; channel < 16; channel++) {
    sendPatch(channel);
  }
}
