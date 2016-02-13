//
//  TOP LEVEL SKETCH
//
#include <MidiClockPage.h>
#include <MidiEuclidSketch.h>
OctatrackLiveSketch sketch;
OctatrackTransposeSketch sketch2;
PitchEuclidSketch sketch3;
TuneUpSketch sketch4;
SketchSwitchPage sketchSwitchPage(NULL, &sketch, &sketch2, &sketch3, &sketch4);

void setup() {
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
