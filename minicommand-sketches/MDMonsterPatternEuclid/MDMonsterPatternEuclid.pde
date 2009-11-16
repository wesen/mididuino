#include <MDWesenLivePatchSketch.h>
#include <MDPatternEuclidSketch.h>
#include <MelodyHelperSketch.h>

#include <MidiClockPage.h>

MDWesenLivePatchSketch sketch;
MDPatternEuclidSketch sketch2;

SketchSwitchPage sketchSwitchPage(NULL, &sketch, &sketch2);

void setup() {
  initMDTask();
  
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

