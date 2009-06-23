#include <MD.h>
#include <Vector.hh>

void mdTaskOnStatusResponseCallback(uint8_t type, uint8_t value);
void mdTaskOnKitCallback();
void mdTaskOnGlobalCallback();

void MDTaskClass::setup() {
  MDSysexListener.setup();
  MDSysexListener.addOnStatusResponseCallback(mdTaskOnStatusResponseCallback);
  MDSysexListener.setOnGlobalMessageCallback(mdTaskOnGlobalCallback);
  MDSysexListener.setOnKitMessageCallback(mdTaskOnKitCallback);
}

void MDTaskClass::onStatusResponse(uint8_t type, uint8_t value) {
  switch (type) {
  case MD_CURRENT_KIT_REQUEST:
    if (MD.currentKit != value) {
      MD.currentKit = value;
      if (autoLoadKit) {
	MD.requestKit(MD.currentKit);
      }
    }
    break;
    
  case MD_CURRENT_GLOBAL_SLOT_REQUEST:
    if (MD.currentGlobal != value) {
      MD.currentGlobal = value;
      if (autoLoadGlobal) {
	MD.requestGlobal(MD.currentGlobal);
      }
    }
    break;

  case MD_CURRENT_PATTERN_REQUEST:
    MD.currentPattern = value;
    break;
  }
  redisplay = true;
}

MDTaskClass MDTask(1000);

void mdTaskOnGlobalCallback() {
  MD.global.fromSysex(MidiSysex.data, MidiSysex.recordLen);
}

void mdTaskOnKitCallback() {
  MD.kit.fromSysex(MidiSysex.data, MidiSysex.recordLen);
}

void mdTaskOnStatusResponseCallback(uint8_t type, uint8_t value) {
  MDTask.onStatusResponse(type, value);
}
