#include <MD.h>
#include <MDStudioSketch.h>
#include <MDStudioPages.h>

MDStudioSketch sketch;

void setup() {
  initMDTask();
  
  sketch.setup();
  GUI.setSketch(&sketch);
}

void loop() {
}

