#include <MidiClockPage.h>
#include <MNM.h>
#include <MidiEuclidSketch.h>
#include <MNMPatternEuclidSketch.h>
#include <MNMWesenLivePatchSketch.h>

MNMPatternEuclidSketch sketch;
MNMWesenLivePatchSketch sketch2;
SketchSwitchPage sketchSwitchPage(NULL, &sketch, &sketch2);

void setup() {
  MNM.currentPattern = 0;
  initMNMTask();
  MNMTask.autoLoadKit = true;
  MNMTask.reloadGlobal = false;
  MNMTask.autoLoadGlobal = true;
  MNMTask.autoLoadPattern = true;
  MNMTask.verbose = false;
  
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

