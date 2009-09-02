#include <MNMWesenLivePatchSketch.h>
#include <MidiEuclidSketch.h>
#include <MidiClockPage.h>

MNMWesenLivePatchSketch sketch2;
PitchEuclidSketch sketch3;
SketchSwitchPage sketchSwitchPage(NULL, &sketch2, &sketch3);

void setup() {
  initMNMTask();
  
  sketch2.setupMonster(true);
  sketch3.setupMonster(true);
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
