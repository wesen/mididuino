#include <MidiClockPage.h>
#include <LeoSketch.h>

LeoSketch sketch;

void setup() {
  initMDTask();
  sketch.setup();
  GUI.setSketch(&sketch);
}

void loop() {
}
