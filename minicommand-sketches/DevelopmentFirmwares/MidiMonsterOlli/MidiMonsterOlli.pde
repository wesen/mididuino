#include <MidiClockPage.h>
#include <MidiEuclidSketch.h>
#include <AutoMidiController.h>

PitchEuclidSketch sketch;
AutoMidiControllerSketch sketch2;
SketchSwitchPage sketchSwitchPage(NULL, &sketch, &sketch2);

void setup() {
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
