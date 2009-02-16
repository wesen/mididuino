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
  
  if (MidiUart.rxRb.overflow > 0) {
    setLed();
  }
  GUI.update();
}

void handleGui() {
  handleGuiSysex();
  handleGuiPages();
}
