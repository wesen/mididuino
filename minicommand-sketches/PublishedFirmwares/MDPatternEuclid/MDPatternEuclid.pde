#include <MD.h>
#include <MDPatternEuclidSketch.h>

MDPatternEuclidSketch sketch;

void setup() {
  initMDTask();
  MDTask.verbose = false;
  
  sketch.setup();
  GUI.setSketch(&sketch);
}

