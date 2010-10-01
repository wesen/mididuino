#include <MD.h>
#include <MDStudioPages.h>
#include <MDStudioSketch.h>

MDStudioSketch sketch;

void setup() {
  initMDTask();
  
  sketch.setup();
  GUI.setSketch(&sketch);
}

void loop() {
}
