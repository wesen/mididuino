#include "MD.h"
#include "MDSysex.hh"

MachineDrumSysexClass MDSysex(mididuino_sysex_data, sizeof(mididuino_sysex_data));

void MachineDrumSysexClass::start() {
  isMachineDrumSysex = true;
  MididuinoSysexClass::start();
}

void MachineDrumSysexClass::handleGlobalDump(uint8_t c) {
  if (len == 0xAC)
    lastReceivedGlobal.baseChannel = c;
}

void MachineDrumSysexClass::handleKitDump(uint8_t c) {
  if (len >= 0x08 && len < 0x18) {
    lastReceivedKit.name[len - 0x08] = c;
  } else if (len == 0x1A8) {
    startRecord();
  } else if (len == 0x1F3) {
    uint8_t tmp[16 * 4];
    stopRecord();
    sysex_to_data_elektron(data, tmp, 74);

    uint8_t i;
    for (i = 0; i < 16; i++) {
      lastReceivedKit.trackModels[i] = tmp[i * 4 + 3];
    }
  }
}

void MachineDrumSysexClass::handleByte(uint8_t byte) {
  if ((len < sizeof(machinedrum_sysex_hdr)) &&
      (byte != machinedrum_sysex_hdr[len])) {
    isMachineDrumSysex = false;
  }
  
  if (isMachineDrumSysex) {
    if (len == sizeof(machinedrum_sysex_hdr)) {
      msgType = byte;
      switch (byte) {
      case MD_GLOBAL_MESSAGE_ID:
	break;

      case MD_KIT_MESSAGE_ID:
	break;

      case MD_STATUS_RESPONSE_ID:
	startRecord();
	break;
      }
    } else {
      switch (msgType) {
      case MD_GLOBAL_MESSAGE_ID:
	handleGlobalDump(byte);
	break;

      case MD_KIT_MESSAGE_ID:
	handleKitDump(byte);
	break;

      default:
	break;
      }
    }
  }
  
  MididuinoSysexClass::handleByte(byte);
}

void MachineDrumSysexClass::end() {
  if (isMachineDrumSysex) {
    switch (msgType) {
    case MD_STATUS_RESPONSE_ID:
      switch (data[1]) {
      case MD_CURRENT_KIT_REQUEST:
	MD.currentKit = data[2];
	break;

      case MD_CURRENT_GLOBAL_SLOT_REQUEST:
	MD.currentGlobal = data[2];
	break;
      }
      if (onStatusResponseCallback != NULL)
	onStatusResponseCallback(data[1], data[2]);
      break;
      
      
    case MD_GLOBAL_MESSAGE_ID:
      if (onGlobalMessageCallback != NULL)
	onGlobalMessageCallback(&lastReceivedGlobal);
      break;
      
    case MD_KIT_MESSAGE_ID:
      if (onKitMessageCallback != NULL)
	onKitMessageCallback(&lastReceivedKit);
      break;
    }
  }

  MidiSysexClass::end();
}

void MachineDrumSysexClass::setup() {
  Midi.setSysex(this);
}
