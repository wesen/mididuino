#include <MD.h>
bool loadedKit = false;

void handleGuiSysex() {
}

void setup() {
  setupMidi();
  setupPages();
}

void loop() {
  GUI.updatePage();
  
  loopPages();
  
  GUI.update();
}

void handleGui() {
  handleGuiSysex();
  handleGuiPages();
}
