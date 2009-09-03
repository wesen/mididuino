#include <MidiClockPage.h>
#include <MDPitchEuclidSketch.h>

PitchEuclidSketch sketch;

void setup() {
  initMDTask();
  
  sketch.setup();
  GUI.setSketch(&sketch);

  initClockPage();
}

void loop() {
}
