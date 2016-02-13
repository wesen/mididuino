#include <MidiEuclidSketch.h>
#include <MidiClockPage.h>

PitchEuclidSketch sketch2;

void setup() {
  sketch2.setup();
  GUI.setSketch(&sketch2);

  initClockPage();
}


void loop() {
}
