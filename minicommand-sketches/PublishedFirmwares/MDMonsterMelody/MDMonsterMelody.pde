#include <MDArpeggiatorSketch.h>
#include <MDWesenLivePatchSketch.h>
#include <MDPitchEuclidSketch.h>
#include <MelodyHelperSketch.h>
#include <MidiClockPage.h>

ArpeggiatorSketch sketch;
//SketchSwitchPage sketchSwitchPage(NULL, &sketch);
MDWesenLivePatchSketch sketch2;
MDPitchEuclidSketch sketch3;
MelodyHelperSketch sketch4;
SketchSwitchPage sketchSwitchPage(NULL, &sketch, &sketch2, &sketch3, &sketch4);

void setup() {
  initMDTask();
  
  sketch.setupMonster(true);
  sketch2.setupMonster(true);
  sketch3.setupMonster(true);
  sketch4.setupMonster(true);
  GUI.setSketch(&_defaultSketch);
  GUI.setPage(&sketchSwitchPage);
  GUI.addEventHandler(handleEvent);

  initClockPage();
}

bool handleEvent(gui_event_t *event) {
  return sketchSwitchPage.handleGlobalEvent(event);
}

void loop() {
}
