#include "MNM.h"
#include "Vector.hh"

void mnmTaskOnStatusResponseCallback(uint8_t type, uint8_t value);
void mnmTaskOnKitCallback();
void mnmTaskOnGlobalCallback();

void MNMTaskClass::setup(uint16_t _interval, bool _autoLoadKit, bool _autoLoadGlobal, bool _reloadGlobal) {
  interval = _interval;
  autoLoadKit = _autoLoadKit;
  autoLoadGlobal = _autoLoadGlobal;
  reloadGlobal = _reloadGlobal;

  MNMSysexListener.setup();
  MNMSysexListener.addOnStatusResponseCallback(mnmTaskOnStatusResponseCallback);
  MNMSysexListener.setOnGlobalMessageCallback(mnmTaskOnGlobalCallback);
  MNMSysexListener.setOnKitMessageCallback(mnmTaskOnKitCallback);
}

void MNMTaskClass::destroy() {
  MNMSysexListener.removeOnStatusResponseCallback(mnmTaskOnStatusResponseCallback);
  MNMSysexListener.setOnGlobalMessageCallback(NULL);
  MNMSysexListener.setOnKitMessageCallback(NULL);
}

void MNMTaskClass::onStatusResponse(uint8_t type, uint8_t value) {
  switch (type) {
  case MNM_CURRENT_KIT_REQUEST:
    if (MNM.currentKit != value) {
      MNM.currentKit = value;
      if (autoLoadKit) {
	MNM.requestKit(MNM.currentKit);
      } else {
	callKitCallbacks();
      }
    }
    if (reloadKit) {
      MNM.requestKit(MNM.currentKit);
      reloadKit = false;
    }
    break;
    
  case MNM_CURRENT_GLOBAL_SLOT_REQUEST:
    if (MNM.currentGlobal != value) {
      MNM.currentGlobal = value;
      if (autoLoadGlobal) {
	MNM.requestGlobal(MNM.currentGlobal);
      } else {
	callGlobalCallbacks();
      }
    }
    if (reloadGlobal) {
      MNM.requestGlobal(MNM.currentGlobal);
      reloadGlobal = false;
    }
    break;

  case MNM_CURRENT_PATTERN_REQUEST:
    if (MNM.currentPattern != value) {
      MNM.currentPattern = value;
      callPatternCallbacks();
    }
    break;

  case MNM_CURRENT_AUDIO_TRACK_REQUEST:
    if (MNM.currentTrack != value) {
      MNM.currentTrack = value;
      callCurrentTrackCallbacks();
    }
    break;
  }
  redisplay = true; 
}

MNMTaskClass MNMTask(3000);

void mnmTaskOnGlobalCallback() {
  MNM.loadedGlobal = false;
  if (MNM.global.fromSysex(MidiSysex.data, MidiSysex.recordLen)) {
    MNM.loadedGlobal = true;
    MNMTask.callGlobalCallbacks();
  }
}

void mnmTaskOnKitCallback() {
  MNM.loadedKit = false;
  if (MNM.kit.fromSysex(MidiSysex.data, MidiSysex.recordLen)) {
    MNM.loadedKit = true;
    MNMTask.callKitCallbacks();
  } else {
  }
}

void mnmTaskOnStatusResponseCallback(uint8_t type, uint8_t value) {
  MNMTask.onStatusResponse(type, value);
}
