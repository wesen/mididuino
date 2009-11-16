#include "MD.h"
#include "TurboMidi.hh"

void setup() {
  MDSysexListener.setup();
  TurboMidi.setup();
  GUI.addEventHandler(&handleEvent);
}

bool handleEvent(gui_event_t *evt) {
  if (EVENT_PRESSED(evt, Buttons.BUTTON1)) {
    TurboMidi.startTurboMidi();
  } else if (EVENT_PRESSED(evt, Buttons.BUTTON2)) {
    TurboMidi.stopTurboMidi();
  } else if (EVENT_PRESSED(evt, Buttons.BUTTON3)) {
    getPatterns();
  }
}

MDPattern pattern;
  
void getPatterns() {
  GUI.flash_strings_fill("GET PATTERNS", "HELLO");
  
  for (uint8_t i = 0; i < 10; i++) {
    if (MD.getBlockingPattern(i, 10000)) {
      if (pattern.fromSysex(MidiSysex.data + 5, MidiSysex.recordLen - 5)) {
        GUI.flash_strings_fill("PATTERN", "0");
        GUI.setLine(GUI.LINE2);
        GUI.flash_put_value(0, i);
      } else {
        GUI.flash_strings_fill("PATTERN ERROR", "0");
        GUI.setLine(GUI.LINE2);
        GUI.flash_put_value(0, i);
      }
    } else {
      GUI.flash_strings_fill("TIMEOUT", "0");
        GUI.setLine(GUI.LINE2);
        GUI.flash_put_value(0, i);
    }
  }
}

