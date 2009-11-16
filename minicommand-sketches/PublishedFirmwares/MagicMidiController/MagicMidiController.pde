#include <AutoMidiController.h>
#include <MidiClockPage.h>

AutoMidiControllerSketch sketch;

void setup() {
  sketch.setup();
  GUI.setSketch(&sketch);
  initClockPage();
}

