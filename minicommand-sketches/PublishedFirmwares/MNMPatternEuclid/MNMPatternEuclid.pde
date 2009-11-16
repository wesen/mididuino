#include <MidiClockPage.h>
#include <MNM.h>
#include <MidiEuclidSketch.h>
#include <MNMPatternEuclidSketch.h>

MNMPatternEuclidSketch sketch;
// MDWesenLivePatchSketch sketch2;

void setup() {
  MNM.currentPattern = 0;
  initMNMTask();
  MNMTask.autoLoadKit = true;
  MNMTask.reloadGlobal = false;
  MNMTask.autoLoadGlobal = false;
  MNMTask.autoLoadPattern = true;
  MNMTask.verbose = false;
  
  sketch.setup();
  GUI.setSketch(&sketch);

  initClockPage();
}

