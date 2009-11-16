#include <MDRandomizeSketch.h>
#include <MidiClockPage.h>

MDRandomizeSketch sketch;

void setup() {
  initMDTask();
  
  sketch.setup();
  GUI.setSketch(&sketch);

  initClockPage();
}

void loop() {
}

