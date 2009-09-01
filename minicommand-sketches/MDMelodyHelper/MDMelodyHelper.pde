#include <MelodyHelperSketch.h>

MelodyHelperSketch sketch;

void setup() {
  initMDTask();
  
  sketch.setup();
  GUI.setSketch(&sketch);
}

void loop() {
}
