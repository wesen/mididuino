#include <MDArpeggiatorSketch.h>
#include <MDWesenLivePatchSketch.h>
#include <MDPitchEuclidSketch.h>
#include <MidiClockPage.h>

ArpeggiatorSketch sketch;
MDWesenLivePatchSketch sketch2;
PitchEuclidSketch sketch3;
SketchSwitchPage sketchSwitchPage(NULL, &sketch, &sketch2, &sketch3);

void setup() {
  initMDTask();
  
  sketch.setup();
  sketch2.setup();
  sketch3.setup();
  GUI.setSketch(&sketch);
  GUI.setPage(&sketchSwitchPage);
  GUI.addEventHandler(handleEvent);

  initClockPage();
}

bool handleEvent(gui_event_t *event) {
  return sketchSwitchPage.handleGlobalEvent(event);
}

void loop() {
}
