#include <MD.h>
#include <Vector.hh>

void mdTaskOnStatusResponseCallback(uint8_t type, uint8_t value);
void mdTaskOnKitCallback();
void mdTaskOnGlobalCallback();

void MDTaskClass::setup(uint16_t _interval, bool _autoLoadKit, bool _autoLoadGlobal, bool _reloadGlobal) {
  interval = _interval;
  autoLoadKit = _autoLoadKit;
  autoLoadGlobal = _autoLoadGlobal;
  reloadGlobal = _reloadGlobal;
  
  MDSysexListener.setup();
  MDSysexListener.addOnStatusResponseCallback(mdTaskOnStatusResponseCallback);
  MDSysexListener.setOnGlobalMessageCallback(mdTaskOnGlobalCallback);
  MDSysexListener.setOnKitMessageCallback(mdTaskOnKitCallback);
}

void MDTaskClass::destroy() {
  MDSysexListener.removeOnStatusResponseCallback(mdTaskOnStatusResponseCallback);
  MDSysexListener.setOnGlobalMessageCallback(NULL);
  MDSysexListener.setOnKitMessageCallback(NULL);
}

void MDTaskClass::onStatusResponse(uint8_t type, uint8_t value) {
  switch (type) {
  case MD_CURRENT_KIT_REQUEST:
    if (MD.currentKit != value) {
      MD.currentKit = value;
      if (autoLoadKit) {
	MD.requestKit(MD.currentKit);
      } else {
	callKitCallbacks();
      }
    }
    if (reloadKit) {
      MD.requestKit(MD.currentKit);
      reloadKit = false;
    }
    break;
    
  case MD_CURRENT_GLOBAL_SLOT_REQUEST:
    if (MD.currentGlobal != value) {
      MD.currentGlobal = value;
      if (autoLoadGlobal) {
	MD.requestGlobal(MD.currentGlobal);
      } else {
	callGlobalCallbacks();
      }
    }
    if (reloadGlobal) {
      MD.requestGlobal(MD.currentGlobal);
      reloadGlobal = false;
    }
    break;

  case MD_CURRENT_PATTERN_REQUEST:
    if (MD.currentPattern != value) {
      MD.currentPattern = value;
      callPatternCallbacks();
    }
    break;
  }
  redisplay = true;
}

MDTaskClass MDTask(3000);

void mdTaskOnGlobalCallback() {
  MD.loadedGlobal = false;
  if (MD.global.fromSysex(MidiSysex.data, MidiSysex.recordLen)) {
    MD.loadedGlobal = true;
    MDTask.callGlobalCallbacks();
  }
}

void mdTaskOnKitCallback() {
  MD.loadedKit = false;
  if (MD.kit.fromSysex(MidiSysex.data, MidiSysex.recordLen)) {
    MD.loadedKit = true;
    MDTask.callKitCallbacks();
  } else {
    //    GUI.flash_strings_fill("FROM SYSEX", "ERROR");
  }
}

void mdTaskOnStatusResponseCallback(uint8_t type, uint8_t value) {
  MDTask.onStatusResponse(type, value);
}
