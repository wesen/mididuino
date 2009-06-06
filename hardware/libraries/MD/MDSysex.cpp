#include "helpers.h"
#include "MD.h"
#include "MDSysex.hh"

MachineDrumSysexClass MDSysex(mididuino_sysex_data, sizeof(mididuino_sysex_data));

void MachineDrumSysexClass::start() {
  isMachineDrumSysex = true;
  MididuinoSysexClass::start();
}

uint8_t MDSysexBuf[8192];

void MachineDrumSysexClass::handleByte(uint8_t byte) {
  if ((len < sizeof(machinedrum_sysex_hdr)) &&
      (byte != machinedrum_sysex_hdr[len])) {
    isMachineDrumSysex = false;
  } else if (isMachineDrumSysex) {
    if (len == sizeof(machinedrum_sysex_hdr)) {
      msgType = byte;
      switch (byte) {
      case MD_GLOBAL_MESSAGE_ID:
	startRecord(MDSysexBuf, sizeof(MDSysexBuf));
	break;

      case MD_KIT_MESSAGE_ID:
	startRecord(MDSysexBuf, sizeof(MDSysexBuf));
	break;

      case MD_STATUS_RESPONSE_ID:
	startRecord();
	break;

      case MD_PATTERN_MESSAGE_ID:
	startRecord(MDSysexBuf, sizeof(MDSysexBuf));
	break;

      case MD_SONG_MESSAGE_ID:
	startRecord(MDSysexBuf, sizeof(MDSysexBuf));
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
	onGlobalMessageCallback();
      break;
      
    case MD_KIT_MESSAGE_ID:
      if (onKitMessageCallback != NULL)
	onKitMessageCallback();
      break;

    case MD_PATTERN_MESSAGE_ID:
      if (onPatternMessageCallback != NULL)
	onPatternMessageCallback();
      break;

    case MD_SONG_MESSAGE_ID:
      if (onSongMessageCallback != NULL)
	onSongMessageCallback();
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
    MD.requestKit(MD.currentKit);
  }
}

void getCurrentKitOnGlobalMessageCallback() {
  if (MDSysex.mdGetCurrentKitStatus == MD_GET_GLOBAL) {
    //    MD.baseChannel = global->baseChannel;
    MDSysex.mdGetCurrentKitStatus = MD_DONE;
    if (MDSysex.onCurrentKitCallback != NULL)
      MDSysex.onCurrentKitCallback();
  }
}

void getCurrentKitOnKitMessageCallback() {
  if (MDSysex.mdGetCurrentKitStatus == MD_GET_KIT) {
    //    m_memcpy(MD.name, kit->name, sizeof(MD.name));
    //    m_memcpy(MD.trackModels, kit->trackModels, sizeof(MD.trackModels));
    MDSysex.mdGetCurrentKitStatus = MD_GET_GLOBAL;
    MD.requestGlobal(MD.currentGlobal);
  }
}

void MachineDrumSysexClass::getCurrentKit(md_callback_t callback) {
  setup();
  //  setOnStatusResponseCallback(getCurrentKitOnStatusResponseCallback);
  //  setOnKitMessageCallback(getCurrentKitOnKitMessageCallback);
  //  setOnGlobalMessageCallback(getCurrentKitOnGlobalMessageCallback);
  setOnCurrentKitCallback(callback);
  mdGetCurrentKitStatus = MD_GET_CURRENT_KIT;
  MD.sendRequest(MD_STATUS_REQUEST_ID, MD_CURRENT_KIT_REQUEST);
}

