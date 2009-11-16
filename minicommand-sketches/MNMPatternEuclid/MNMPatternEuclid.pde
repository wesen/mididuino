#include <MNM.h>
#include <MidiEuclidSketch.h>
#include <MNMPatternEuclidSketch.h>

MNMPatternEuclidSketch sketch;
// MDWesenLivePatchSketch sketch2;

void setup() {
  initMNMTask();
  MNMTask.verbose = false;
  
  sketch.setup();
  GUI.setSketch(&sketch);
}

void loop() {
}

