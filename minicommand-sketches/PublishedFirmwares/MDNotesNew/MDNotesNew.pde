#include <MDNotesRecordSketch.h>
#include <MidiClockPage.h>

MDNotesRecordSketch sketch;

void setup() {
  initMDTask();
  
  sketch.setup();
  GUI.setSketch(&sketch);

  initClockPage();
}

void loop() {
}

