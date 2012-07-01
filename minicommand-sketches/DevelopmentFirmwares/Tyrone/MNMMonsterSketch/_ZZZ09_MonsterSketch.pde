//
//  TOP LEVEL SKETCH
//
#include <MidiClockPage.h>
MNMMonoPolySketch sketch2;
MonomachineLiveSketch sketch;
SketchSwitchPage sketchSwitchPage(NULL, &sketch, &sketch2, NULL, NULL);

void setup() {
  initMNMTask();
  
  sketch.setupMonster(true);
  sketch2.setupMonster(true);
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
