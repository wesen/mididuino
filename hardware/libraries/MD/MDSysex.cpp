#include "helpers.h"
#include "MD.h"
#include "MDSysex.hh"
#include "MDMessages.hh"

MDSysexListenerClass MDSysexListener;

void MDSysexListenerClass::start() {
}

void MDSysexListenerClass::handleByte(uint8_t byte) {
  if (MidiSysex.len == sizeof(machinedrum_sysex_hdr)) {
    msgType = byte;
    switch (byte) {
    case MD_GLOBAL_MESSAGE_ID:
      MidiSysex.startRecord();
      break;
      
    case MD_KIT_MESSAGE_ID:
      MidiSysex.startRecord();
      break;
      
    case MD_STATUS_RESPONSE_ID:
      MidiSysex.startRecord();
      break;
      
    case MD_PATTERN_MESSAGE_ID:
      MidiSysex.startRecord();
      break;
      
    case MD_SONG_MESSAGE_ID:
      MidiSysex.startRecord();
      break;
    }
  }
}

void MDSysexListenerClass::end() {
  switch (msgType) {
  case MD_STATUS_RESPONSE_ID:
    switch (MidiSysex.data[1]) {
    case MD_CURRENT_KIT_REQUEST:
      MD.currentKit = MidiSysex.data[2];
      break;
      
    case MD_CURRENT_GLOBAL_SLOT_REQUEST:
      MD.currentGlobal = MidiSysex.data[2];
      break;
    }
    
    if (onStatusResponseCallback != NULL)
      onStatusResponseCallback(MidiSysex.data[1], MidiSysex.data[2]);
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

void MDSysexListenerClass::setup() {
  MidiSysex.addSysexListener(this);
}

void getCurrentKitOnStatusResponseCallback(uint8_t type, uint8_t value) {
  if (type == MD_CURRENT_KIT_REQUEST && 
      MDSysexListener.mdGetCurrentKitStatus == MD_GET_CURRENT_KIT) {
    MDSysexListener.mdGetCurrentKitStatus = MD_GET_CURRENT_GLOBAL;
    MD.sendRequest(MD_STATUS_REQUEST_ID, MD_CURRENT_GLOBAL_SLOT_REQUEST);
  } else if (type == MD_CURRENT_GLOBAL_SLOT_REQUEST && 
             MDSysexListener.mdGetCurrentKitStatus == MD_GET_CURRENT_GLOBAL) {
    MDSysexListener.mdGetCurrentKitStatus = MD_GET_KIT;
    MD.requestKit(MD.currentKit);
  }
}

void getCurrentKitOnGlobalMessageCallback() {
  if (MDSysexListener.mdGetCurrentKitStatus == MD_GET_GLOBAL) {
    MD.global.fromSysex(MidiSysex.data, MidiSysex.recordLen);
    MD.baseChannel = MD.global.baseChannel;
    MDSysexListener.mdGetCurrentKitStatus = MD_DONE;
    if (MDSysexListener.onCurrentKitCallback != NULL)
      MDSysexListener.onCurrentKitCallback();
  }
}

void getCurrentKitOnKitMessageCallback() {
  if (MDSysexListener.mdGetCurrentKitStatus == MD_GET_KIT) {
    MD.kit.fromSysex(MidiSysex.data, MidiSysex.recordLen);
    MDSysexListener.mdGetCurrentKitStatus = MD_GET_GLOBAL;
    MD.requestGlobal(MD.currentGlobal);
  }
}

void MDSysexListenerClass::getCurrentKit(md_callback_t callback) {
  setup();
  setOnStatusResponseCallback(getCurrentKitOnStatusResponseCallback);
  setOnKitMessageCallback(getCurrentKitOnKitMessageCallback);
  setOnGlobalMessageCallback(getCurrentKitOnGlobalMessageCallback);
  setOnCurrentKitCallback(callback);
  mdGetCurrentKitStatus = MD_GET_CURRENT_KIT;
  MD.sendRequest(MD_STATUS_REQUEST_ID, MD_CURRENT_KIT_REQUEST);
}

