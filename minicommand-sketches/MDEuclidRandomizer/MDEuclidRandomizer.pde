#include <MDEuclidRandomize.h>
#include <MidiClockPage.h>

MDEuclidRandomizeSketch sketch;

void setup() {
  initMDTask();
  
  sketch.setup();
  GUI.setSketch(&sketch);

  initClockPage();
}

void loop() {
}

