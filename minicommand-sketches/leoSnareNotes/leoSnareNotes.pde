#include <MidiClockPage.h>
#include <LeoSketch.h>

LeoSnareNotesSketch sketch;

void setup() {
  initMDTask();
  sketch.setup();
  GUI.setSketch(&sketch);
}

void loop() {
}
