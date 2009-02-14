#include <MD.h>
#include <GUI.h>

bool loadedKit = false;

void setup() {
  setupMidi();
  setupMDSysex();
  setupPages();
  GUI.setLine(GUI.LINE1);
  GUI.put_p_string_fill(PSTR("RELOAD KIT"));
}

void loop() {
  GUI.updatePage();
  GUI.update();
}

void handleGui() {
  handleGuiSysex();
  handleGuiPages();
}
