#include <MDArpeggiatorSketch.h>
#include <MidiClockPage.h>

ArpeggiatorSketch sketch;

void setup() {
  initMDTask();
  
  sketch.setup();
  GUI.setSketch(&sketch);

  initClockPage();
}

void loop() {
}
