#include <MidiClockPage.h>
#include <MDPitchEuclidSketch.h>

PitchEuclidSketch sketch;

void setup() {
  sketch.setup();
  GUI.setSketch(&sketch);

  initClockPage();
}

void loop() {
}
