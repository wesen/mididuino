//
//  TOP LEVEL SKETCH
//
#include <MidiClockPage.h>
OctatrackLiveSketch sketch;
SketchSwitchPage sketchSwitchPage(NULL, &sketch, NULL, NULL, NULL);

void setup() {
  sketch.setupMonster(true); 
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
