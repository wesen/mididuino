// race condition on restore from reverse@!!

#include <MidiClockPage.h>
#include <MDWesenLivePatchSketch.h>

MDWesenLivePatchSketch sketch;

void setup() {
  initMDTask();
  
//  enableProfiling();
  sketch.setup();
  GUI.setSketch(&sketch);

  initClockPage();
}

void loop() {
}
