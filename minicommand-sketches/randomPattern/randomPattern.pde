#include <MD.h>
#include <MDPatternEuclidSketch.h>
#include <MDWesenLivePatchSketch.h>

MDPatternEuclidSketch sketch;
MDWesenLivePatchSketch sketch2;

void setup() {
  initMDTask();
  MDTask.verbose = false;
  
  sketch.setup();
  GUI.setSketch(&sketch);
}

void loop() {
}

