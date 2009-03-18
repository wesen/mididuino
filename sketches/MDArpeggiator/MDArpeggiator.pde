#include <MD.h>

bool loadedKit = false;

bool triggerRecording = false;
bool recording = false;
int recordStart = 0;
int recordLength = 32;
bool endRecording = false;

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
