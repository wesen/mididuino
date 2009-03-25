#include "MD.h"
#include "MDSysex.hh"
#include "GUI.h"

MachineDrumSysexClass MDSysex(mididuino_sysex_data, sizeof(mididuino_sysex_data));

void MachineDrumSysexClass::start() {
  isMachineDrumSysex = true;
  handle_kit_state = handle_kit_name;
  MididuinoSysexClass::start();
}

void MachineDrumSysexClass::handleGlobalDump(uint8_t c) {
  if (len == 0xAC)
    lastReceivedGlobal.baseChannel = c;
}

void MachineDrumSysexClass::handleKitDump(uint8_t c) {
  switch (handle_kit_state) {
  case handle_kit_name:
    if (len > (0x0A - 2)) {
      lastReceivedKit.name[len - 0x09] = c;
    }
    if (len == (0x1A - 2))
      handle_kit_state = handle_kit_params;
    break;

  case handle_kit_params:
    if (len > (0x1A - 2)) {
      *((uint8_t *)(MD.trackParams) + (len - (0x1A - 2 +  1))) = c;
    }
    if (len == (0x19A - 2))
      handle_kit_state = handle_kit_levels;
    break;

  case handle_kit_levels:
    if (len > (0x19A - 2)) {
      MD.trackLevels[len - (0x19A - 2 + 1)] = c;
    }
    if (len == (0x1AA - 2))
      handle_kit_state = handle_kit_models;
    break;

  case handle_kit_models:
    if (len == 0x1A9)
      startRecord();
    else if (len == 0x1F4) {
      stopRecord();
      
      uint8_t tmp[16 * 4];
      sysex_to_data_elektron(data, tmp, 74);
      
      uint8_t i;
      for (i = 0; i < 16; i++) {
	lastReceivedKit.trackModels[i] = tmp[i * 4 + 3];
      }
      handle_kit_state = handle_kit_none;
    }
    break;

  default:
    break;
  }
}

#ifdef MIDIDUINO_EXTERNAL_RAM
uint8_t MDSysexBuf[16384] EXTRAM;
#endif

void MachineDrumSysexClass::handleByte(uint8_t byte) {
  if ((len < sizeof(machinedrum_sysex_hdr)) &&
      (byte != machinedrum_sysex_hdr[len])) {
    isMachineDrumSysex = false;
  } else if (isMachineDrumSysex) {
    if (len == sizeof(machinedrum_sysex_hdr)) {
      msgType = byte;
      switch (byte) {
      case MD_GLOBAL_MESSAGE_ID:
	break;

      case MD_KIT_MESSAGE_ID:
#ifdef MIDIDUINO_EXTERNAL_RAM
	startRecord(MDSysexBuf, sizeof(MDSysexBuf));
#endif
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
#ifndef MIDIDUINO_EXTERNAL_RAM
	handleKitDump(byte);
#endif
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

void getCurrentKitOnStatusResponseCallback(uint8_t type, uint8_t value) {
  if (type == MD_CURRENT_KIT_REQUEST && 
      MDSysex.mdGetCurrentKitStatus == MD_GET_CURRENT_KIT) {
    MDSysex.mdGetCurrentKitStatus = MD_GET_CURRENT_GLOBAL;
    MD.sendRequest(MD_STATUS_REQUEST_ID, MD_CURRENT_GLOBAL_SLOT_REQUEST);
  } else if (type == MD_CURRENT_GLOBAL_SLOT_REQUEST && 
             MDSysex.mdGetCurrentKitStatus == MD_GET_CURRENT_GLOBAL) {
    MDSysex.mdGetCurrentKitStatus = MD_GET_KIT;
       MD.sendRequest(MD_KIT_REQUEST_ID, MD.currentKit);
  }
}

void getCurrentKitOnGlobalMessageCallback(md_global_t *global) {
  if (MDSysex.mdGetCurrentKitStatus == MD_GET_GLOBAL) {
    MD.baseChannel = global->baseChannel;
    MDSysex.mdGetCurrentKitStatus = MD_DONE;
    if (MDSysex.onCurrentKitCallback != NULL)
      MDSysex.onCurrentKitCallback();
  }
}

void getCurrentKitOnKitMessageCallback(md_kit_t *kit) {
  if (MDSysex.mdGetCurrentKitStatus == MD_GET_KIT) {
    m_memcpy(MD.name, kit->name, sizeof(MD.name));
    m_memcpy(MD.trackModels, kit->trackModels, sizeof(MD.trackModels));
    MDSysex.mdGetCurrentKitStatus = MD_GET_GLOBAL;
    MD.sendRequest(MD_GLOBAL_REQUEST_ID, MD.currentGlobal);
  }
}

void MachineDrumSysexClass::getCurrentKit(md_callback_t callback) {
  setup();
  setOnStatusResponseCallback(getCurrentKitOnStatusResponseCallback);
  setOnKitMessageCallback(getCurrentKitOnKitMessageCallback);
  setOnGlobalMessageCallback(getCurrentKitOnGlobalMessageCallback);
  setOnCurrentKitCallback(callback);
  mdGetCurrentKitStatus = MD_GET_CURRENT_KIT;
  MD.sendRequest(MD_STATUS_REQUEST_ID, MD_CURRENT_KIT_REQUEST);
}

