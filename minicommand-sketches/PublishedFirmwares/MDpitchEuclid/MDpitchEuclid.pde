#include <MidiClockPage.h>
#include <MDPitchEuclidSketch.h>

MDPitchEuclidSketch sketch;

void setup() {
  initMDTask();
  
  sketch.setup();
  GUI.setSketch(&sketch);

  initClockPage();
}

void loop() {
}
