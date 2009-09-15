#include <MidiClockPage.h>
#include <LeoSketch.h>

LeoSnareNotesSketch sketch;

void setup() {
  initMDTask();
  sketch.setup();
  GUI.setSketch(&sketch);
  initClockPage();
}

void loop() {
}
